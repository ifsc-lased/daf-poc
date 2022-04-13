/*
 * timer.cpp
 *
 *  Created on: 4 de mai de 2021
 *      Author: rgnagel
 */

#include "timer.h"
#include <MAX32552.h>
#include <mml_tmr.h>

mml_tmr_config_t Timer::timer_conf;
size_t Timer::internal_handler_ctr = 0;

std::array<callback_t*, MAX_CALLBACKS> Timer::callbacks = { nullptr };

size_t Timer::callbacks_n = 0;

void Timer::internal_handler(void) {
	internal_handler_ctr++;
	for (auto i = callbacks.begin(); i != callbacks.end(); i++) {
		if (*i == nullptr)
			continue;

		callback_t *cb = *i;

		cb->elapsed_ms += PERIOD_MS;

		if (cb->elapsed_ms >= cb->timeout_ms) {
			// chama o callback
			cb->callback();
			// remove o callback
			cb = nullptr;
			callbacks_n--;
		}
	}

	if (callbacks_n == 0)
		stop_timer();
	else
		init_timer();
}

int Timer::add_callback(callback_t *cb) {
	cb->elapsed_ms = 0;

	if (cb->timeout_ms < PERIOD_MS)
		return -1;

	if ((callbacks_n + 1) == callbacks.size())
		return -2;

	// add callback na primeira posicao livre que for encontrada
	for (auto i = callbacks.begin(); i != callbacks.end(); i++) {
		if (*i == nullptr) {
			(*i) = cb;
			callbacks_n++;
			break;
		}
	}

	if (callbacks_n == 1)
		init_timer();

	return callbacks_n;
}

int Timer::clear_callback(callback_t *cb) {

	for (auto i = callbacks.begin(); i != callbacks.end(); i++) {
		if (*i == cb) {
			*i = nullptr;
			callbacks_n--;
			if (callbacks_n == 0)
				stop_timer();
			return callbacks_n;
		}
	}

	return -1;
}

bool Timer::init_timer(void) {

	static bool configured = false;

	if (!configured) {
		timer_conf.count = 1;
		timer_conf.pwm_value = 0;
		timer_conf.clock = TMRn_CN_PRES_div4;
		timer_conf.mode = TMRn_CN_TMODE_oneShot;
		timer_conf.polarity = TMRn_CN_TPOL_activeLo;
		/* Base count in milliseconds */
		timer_conf.handler = internal_handler;

		if (mml_tmr_init(MML_TMR_DEV0, &timer_conf))
			return false;

		/* Once initialized, enable timer's interruption ... */
		if (mml_tmr_interrupt_enable(MML_TMR_DEV0))
			return false;

		/* Set timeout to be 0.1 seconds */
		mml_tmr_set_timeout_us(MML_TMR_DEV0, PERIOD_MS * 1000);

		configured = true;
	}

	/* ... then enable timer itself. */
	if (mml_tmr_enable(MML_TMR_DEV0))
		return false;

	return true;
}

void Timer::stop_timer(void) {
	mml_tmr_interrupt_clear(MML_TMR_DEV0);
	mml_tmr_disable(MML_TMR_DEV0);
}
