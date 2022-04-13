/*
 * test_timer.cpp
 *
 *  Created on: 4 de mai de 2021
 *      Author: rgnagel
 */

#include "timer.h"
#include "../unity/unity.h"
#include "../debug.h"
#include "../pdafcdc_utils.h"

void setUp(void) {}
void tearDown(void) {}

volatile bool handler1_done = false;

void handler1(void) {
	handler1_done = true;
}

void test_timers(void) {
	callback_t cb1 = {
			.timeout_ms = 1000,
			.elapsed_ms = 0,
			.callback = handler1
	};

	TEST_ASSERT_EQUAL_INT(1, Timer::add_callback(&cb1));

	int t1, t2, diff;

	t1 = PDAFCDC_Utils::get_time_as_ms();
	while (handler1_done == false) {};
	t2 = PDAFCDC_Utils::get_time_as_ms();

	DEBUG_PRINTF("Internal handler counter: %i\n", Timer::internal_handler_ctr);

	diff = t2 - t1;
	TEST_ASSERT_INT_WITHIN(10, 1000, diff);

}



int main(void) {
	DEBUG_PRINTF_INIT();

	UNITY_BEGIN();

	RUN_TEST(test_timers);

	return UNITY_END();
}
