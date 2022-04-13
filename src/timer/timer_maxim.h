/*
 * TimerMaxim.h
 *
 *  Created on: 6 de mai de 2021
 *      Author: rgnagel
 */

#ifndef TIMERMAXIM_H_
#define TIMERMAXIM_H_
#include <functional>

class TimerMaxim {
public:
	typedef void (*handler_t)(void);

	/*
	 * @brief habilita o timer
	 *
	 * @param[in] timeout_ms	tempo do timer
	 * @param[in] handler		funcao callback
	 */
	static int enable_callback(uint32_t timeout_ms, handler_t handler);

	/*
	 * @brief desabilita o timer
	 *
	 */
	static int disable_callback(int);
};

#endif /* TIMERMAXIM_H_ */
