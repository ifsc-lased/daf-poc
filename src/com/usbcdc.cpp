#include <MAX325xx.h>
#include <mml_usb.h>
#include <usb_event.h>
#include <descriptors.h>
#include <cdc_acm.h>
#include <enumerate.h>
#include <mml_uart.h>
#include <stdio.h>

#include "usbcdc.h"

static volatile int usb_read_complete;

/* Global Data */
volatile int configured;
volatile unsigned int event_flags;
int remote_wake_en;

int acm_set_line_coding_callback(void)
{
    DEBUG_PRINTF("acm_set_line_coding_callback\n");
    return 0;
}

int setconfig_callback(usb_setup_pkt *sud, void *cbdata)
{
	(void) cbdata;


    /* This EP assignment must match the Configuration Descriptor */
    const acm_cfg_t acm_cfg = {
        1,                  /* EP OUT */
        MML_USB_MAX_PACKET, /* OUT max packet size */
        2,                  /* EP IN */
        MML_USB_MAX_PACKET, /* IN max packet size */
        3,                  /* EP Notify */
        MML_USB_MAX_PACKET, /* Notify max packet size */
    };

    /* Confirm the configuration value */
    if (sud->wValue == config_descriptor.config_descriptor.bConfigurationValue) {
        configured = 1;
        DEBUG_PRINTF("Enumeration complete. Waiting for characters...\n");
        return acm_configure(&acm_cfg); /* Configure the device class */
    } else if (sud->wValue == 0) {
        configured = 0;
        return acm_deconfigure();
    }

    return -1;
}

int setfeature_callback(usb_setup_pkt *sud, void *cbdata)
{
	(void) cbdata;

    if(sud->wValue == FEAT_REMOTE_WAKE) {
        remote_wake_en = 1;
    } else {
        // Unknown callback
        return -1;
    }

    return 0;
}

int clrfeature_callback(usb_setup_pkt *sud, void *cbdata)
{
	(void) cbdata;

    if(sud->wValue == FEAT_REMOTE_WAKE) {
        remote_wake_en = 0;
    } else {
        // Unknown callback
        return -1;
    }

    return 0;
}

int event_callback(maxusb_event_t evt, void *data)
{
	(void) data;

    /* Set event flag */
    MXC_SETBIT(&event_flags, evt);

    switch (evt) {
        case MAXUSB_EVENT_NOVBUS:
            usb_event_disable(MAXUSB_EVENT_BRST);
            usb_event_disable(MAXUSB_EVENT_SUSP);
            usb_event_disable(MAXUSB_EVENT_DPACT);
            mml_usb_disconnect();
            configured = 0;
            enum_clearconfig();
            acm_deconfigure();
            break;
        case MAXUSB_EVENT_VBUS:
            usb_event_clear(MAXUSB_EVENT_BRST);
            usb_event_enable(MAXUSB_EVENT_BRST, event_callback, NULL);
            usb_event_clear(MAXUSB_EVENT_SUSP);
            usb_event_enable(MAXUSB_EVENT_SUSP, event_callback, NULL);
            mml_usb_connect();
            break;
        case MAXUSB_EVENT_BRST:
            enum_clearconfig();
            acm_deconfigure();
            configured = 0;
            break;
        default:
            break;
    }

    return 0;
}

int usb_read_callback(void)
{
    usb_read_complete = 1;
    return 0;
}

void USB_IRQHandler(void)
{
    usb_event_handler();
}

USBCDC::USBCDC(void)
{
    configured = 0;
    event_flags = 0;
    remote_wake_en = 0;

    /* Initialize the usb module */
    if (mml_usb_init(NULL) != 0) {
    	DEBUG_PRINTF("mml_usb_init() failed\n");
        while (1);
    }

    /* Initialize the enumeration module */
    if (enum_init() != 0) {
    	DEBUG_PRINTF("enum_init() failed\n");
        while (1);
    }

    /* Register enumeration data */
    enum_register_descriptor(ENUM_DESC_DEVICE, (uint8_t*)&device_descriptor, 0);
    enum_register_descriptor(ENUM_DESC_CONFIG, (uint8_t*)&config_descriptor, 0);
    enum_register_descriptor(ENUM_DESC_STRING, lang_id_desc, 0);
    enum_register_descriptor(ENUM_DESC_STRING, mfg_id_desc, 1);
    enum_register_descriptor(ENUM_DESC_STRING, prod_id_desc, 2);

    /* Handle configuration */
    enum_register_callback(ENUM_SETCONFIG, setconfig_callback, NULL);

    /* Handle feature set/clear */
    enum_register_callback(ENUM_SETFEATURE, setfeature_callback, NULL);
    enum_register_callback(ENUM_CLRFEATURE, clrfeature_callback, NULL);

    /* Initialize the class driver */
    if (acm_init() != 0) {
    	DEBUG_PRINTF("acm_init() failed\n");
        while (1);
    }

    /* Register callbacks */
    usb_event_enable(MAXUSB_EVENT_NOVBUS, event_callback, NULL);
    usb_event_enable(MAXUSB_EVENT_VBUS, event_callback, NULL);
    // acm_register_callback(ACM_CB_SET_LINE_CODING, configure_uart);
    acm_register_callback(ACM_CB_SET_LINE_CODING, acm_set_line_coding_callback);
    acm_register_callback(ACM_CB_READ_READY, usb_read_callback);
    usb_read_complete = 0;

    /* Start with USB in low power mode */
    NVIC_SetPriority(USB_IRQn, MML_INTC_PRIO_2);
    NVIC_EnableIRQ(USB_IRQn);
    NVIC_EnableIRQ(UART0_IRQn);

	/* Configure VBUS_DET P1.8 */
	mml_gpio_config_t vbus_det;
	vbus_det.gpio_direction = MML_GPIO_DIR_IN;
	vbus_det.gpio_function = MML_GPIO_SECONDARY_ALT_FUNCTION;
	vbus_det.gpio_pad_config = MML_GPIO_PAD_PULLUP;
	vbus_det.gpio_intr_mode = MML_GPIO_INT_MODE_EDGE_TRIGGERED;
	vbus_det.gpio_intr_polarity = MML_GPIO_INT_POL_FALLING;
	/* Initialize BUTTONs */
	mml_gpio_init(MML_GPIO_DEV1, 8, 1, vbus_det);
}

void USBCDC::check_event_flag(void) {

	if (event_flags) {
		/* Display events */
		if (MXC_GETBIT(&event_flags, MAXUSB_EVENT_NOVBUS)) {
			MXC_CLRBIT(&event_flags, MAXUSB_EVENT_NOVBUS);
			DEBUG_PRINTF("VBUS Disconnect\n");
		} else if (MXC_GETBIT(&event_flags, MAXUSB_EVENT_VBUS)) {
			MXC_CLRBIT(&event_flags, MAXUSB_EVENT_VBUS);
			DEBUG_PRINTF("VBUS Connect\n");
		} else if (MXC_GETBIT(&event_flags, MAXUSB_EVENT_BRST)) {
			MXC_CLRBIT(&event_flags, MAXUSB_EVENT_BRST);
			DEBUG_PRINTF("Bus Reset\n");
		} else if (MXC_GETBIT(&event_flags, MAXUSB_EVENT_SUSP)) {
			MXC_CLRBIT(&event_flags, MAXUSB_EVENT_SUSP);
			DEBUG_PRINTF("Suspended\n");
		} else if (MXC_GETBIT(&event_flags, MAXUSB_EVENT_DPACT)) {
			MXC_CLRBIT(&event_flags, MAXUSB_EVENT_DPACT);
			DEBUG_PRINTF("Resume\n");
		} else if (MXC_GETBIT(&event_flags, MAXUSB_NUM_EVENTS)) {
			MXC_CLRBIT(&event_flags, MAXUSB_NUM_EVENTS);
			DEBUG_PRINTF("Enumeration complete. Waiting for characters...\n");
		}
	}

}

void USBCDC::esvazia_leitura(void) {
    uint8_t buffer[128];
    size_t pendentes;

    pendentes = acm_canread();

    if (pendentes > 128)
        pendentes = 128;

    acm_read(buffer, pendentes);
}