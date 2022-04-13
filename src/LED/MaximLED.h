/*
 * MaximLED.h
 *
 *  Created on: 25 de mai de 2021
 *      Author: rgnagel
 */

#ifndef LED_MAXIMLED_H_
#define LED_MAXIMLED_H_

#include <MAX32552.h>
//#include "ILED.h"
#include "mml_gpio.h"

class MaximLED {
public:
	typedef enum {
		DESLIGADO, LIGADO
	} estado_t;
	MaximLED() {
		_devGPIO = MML_GPIO_DEV0;
		mml_gpio_config_t led_config;
		led_config.gpio_direction = MML_GPIO_DIR_OUT;
		led_config.gpio_function = MML_GPIO_NORMAL_FUNCTION;
		led_config.gpio_pad_config = MML_GPIO_PAD_NORMAL;
		mml_gpio_init(_devGPIO, 12, 1, led_config);
		this->desliga();
	}
	~MaximLED() {
		this->desliga();
		mml_gpio_close(_devGPIO);
	}
	void liga() {
		this->estado = LIGADO;
		mml_gpio_pin_output(_devGPIO, 12, MML_GPIO_OUT_LOGIC_ZERO);
	}
	void desliga() {
		this->estado = DESLIGADO;
		mml_gpio_pin_output(_devGPIO, 12, MML_GPIO_OUT_LOGIC_ONE);
	}
	void toggle() {
		if (this->estado == LIGADO)
			this->desliga();
		else
			this->liga();
	}

private:
	mml_gpio_id_t _devGPIO;
	estado_t estado;
};

#endif /* LED_MAXIMLED_H_ */
