#include <stdio.h>
#include <stddef.h>
#include <MAX325xx.h>

#include "../pdafcdc.h"
#include "../usbcdc.h"
#include "../daf.h"
#include "../debug.h"
#include "../pdafcdc_utils.h"


typedef enum {
	INICIO, RX_DADOS, TX_DADOS
} estado_t;

int main(void) {

	// configura_uart();
	DEBUG_PRINTF_INIT();

	DEBUG_PRINTF("\n\nIniciando DAF\n");

//	PDAFCDC pdafcdc;
//	DAF daf;
	USBCDC usbcdc;


	size_t bytesParaLer = 0;

	estado_t estado = INICIO;
	uint8_t byte;
	uint32_t ts1, ts2; // timestamps

	static uint8_t bufferTx[5000];
	memset(bufferTx, 0xAA, sizeof(bufferTx));

	/* Wait for events */
	while (1) {

		
		USBCDC::check_event_flag();

		bytesParaLer = usbcdc.bytes_para_ler(0);
		if (!bytesParaLer)
			continue;

		usbcdc.le_bytes_bloqueante(&byte, 1);

		switch (estado) {
		case INICIO: {
			static uint32_t counterInicio = 0;
			if (byte == 0x12)
				counterInicio++;

			if (counterInicio == 3) {
				DEBUG_PRINTF("\nIniciando leitura dos dados\n");
				counterInicio = 0;
				estado = RX_DADOS;
				ts1 = PDAFCDC_Utils::get_time_as_ms();
			}

		}
			break;
		case RX_DADOS: {
			static uint32_t counterRxDados = 0;
			if (byte == 0xAA) {
				counterRxDados++;
			} else {
				// erro
				DEBUG_PRINTF("\nErro!\n");
				counterRxDados = 0;
				estado = INICIO;
			}

			if (counterRxDados == 5000) {
				// OK, mensura tempo
				counterRxDados = 0;
				estado = TX_DADOS;
				ts2 = PDAFCDC_Utils::get_time_as_ms();
				uint32_t diff = ts2 - ts1;
				ts1 = ts2;
				DEBUG_PRINTF("\nOK! Periodo RX: %i ms\n", diff);
				DEBUG_PRINTF("\nStarting TX...");
			}

		}
			break;
		case TX_DADOS: {
			static uint32_t bytesTxed = 0;

			uint32_t written = usbcdc.escreve_bytes_bloqueante(
					&bufferTx[bytesTxed], sizeof(bufferTx) - bytesTxed);
			bytesTxed += written;

			if (bytesTxed >= sizeof(bufferTx)) {
				bytesTxed = 0;
				usbcdc.esvazia_leitura();
				ts2 = PDAFCDC_Utils::get_time_as_ms();
				uint32_t diff = ts2 - ts1;
				DEBUG_PRINTF("\nDados txed. Periodo TX: %i ms\n", diff);
				estado = INICIO;
			}

		}
			break;
		}
	}
}

#ifdef __SCPA_FWK__
#error "This example does not handle SCPA Target"
#endif //__SCPA_FWK__
