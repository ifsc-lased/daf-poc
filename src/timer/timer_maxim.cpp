/*
 * TimerMaxim.cpp
 *
 *  Created on: 6 de mai de 2021
 *      Author: rgnagel
 */

#include "timer_maxim.h"
#include <MAX32552.h>
#include <mml_tmr.h>

int TimerMaxim::enable_callback(uint32_t timeout_ms,
		handler_t handler) {

	// primeiro pega timer que esta disponivel para ser usado
	mml_tmr_id_t id;
	if (mml_tmr_get_free_timer(&id) != NO_ERROR)
		return -1;

	// config comum a todos os timers
	mml_tmr_config_t timer_conf;
	timer_conf.count = 1;
	timer_conf.pwm_value = 0;
	timer_conf.clock = TMRn_CN_PRES_div4;
	timer_conf.mode = TMRn_CN_TMODE_oneShot;
	timer_conf.polarity = TMRn_CN_TPOL_activeLo;
	// handler
	timer_conf.handler = handler;

	if (mml_tmr_init(id, &timer_conf))
		return -2;
	if (mml_tmr_interrupt_enable(id))
		return -3;
	if (mml_tmr_set_timeout_us(id, timeout_ms * 1000))
		return -4;
	if (mml_tmr_enable(id))
		return -5;

	return (int)id;
}

int TimerMaxim::disable_callback(int id) {
	mml_tmr_interrupt_clear((mml_tmr_id_t)id);
	mml_tmr_disable((mml_tmr_id_t)id);
	return mml_tmr_close((mml_tmr_id_t)id);
}
