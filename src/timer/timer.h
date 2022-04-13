/*
 * timer.h
 *
 *  Created on: 4 de mai de 2021
 *      Author: rgnagel
 */

#ifndef TIMER_TIMER_H_
#define TIMER_TIMER_H_

#include <array>
#include <functional>

#include <MAX32552.h>
#include <mml_tmr.h>

#define MAX_CALLBACKS 8

typedef struct {
	uint32_t timeout_ms;
	volatile uint32_t elapsed_ms;
	std::function<void(void)> callback;
} callback_t;

class Timer {
public:
	static size_t internal_handler_ctr;
	static const uint32_t PERIOD_MS = 100;
	/**
	 *
	 * @param timeout_ms tempo em milisegundos para disparar o callback
	 * @param callback funcao a ser chamada quando timeout ocorrer
	 * @return menor que zero: erro
	 * @return maior que zero: ID do callback (usado para cancelar o callback se necessario)
	 */
	static int add_callback(callback_t*);

	/**
	 *
	 * @param id id retornado na funcao que add o callback
	 */
	static int clear_callback(callback_t *);

private:
	static std::array<callback_t*, MAX_CALLBACKS> callbacks;

	static bool init_timer(void);
	static void stop_timer(void);

	static size_t callbacks_n;
	static void internal_handler(void);
	static mml_tmr_config_t timer_conf;

};

#endif /* TIMER_TIMER_H_ */
