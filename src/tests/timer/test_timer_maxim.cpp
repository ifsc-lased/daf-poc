#include "../../unity/unity.h"
#include "../../debug.h"
#include "../../pdafcdc_utils.h"
#include "timer_maxim.h"

void setUp(void) {
}
void tearDown(void) {
}

int id1, id2;
volatile bool handler1_done = false;
volatile bool handler2_done = false;

void handler1(void) {
	handler1_done = true;
	TimerMaxim::disable_callback(id1);
}

void handler2(void) {
	handler2_done = true;
	TimerMaxim::disable_callback(id2);
}

void test_timers(void) {

	id1 = TimerMaxim::enable_callback(1000, handler1);
	id2 = TimerMaxim::enable_callback(2000, handler2);

	TEST_ASSERT_GREATER_OR_EQUAL(0, id1);
	TEST_ASSERT_GREATER_OR_EQUAL(0, id2);

	int t1, t2, diff;

	t1 = PDAFCDC_Utils::get_time_as_ms();
	while (handler1_done == false) {
	};
	t2 = PDAFCDC_Utils::get_time_as_ms();
	diff = t2 - t1;
	TEST_ASSERT_INT_WITHIN(10, 1000, diff);

	t1 = PDAFCDC_Utils::get_time_as_ms();
	while (handler2_done == false) {
	};
	t2 = PDAFCDC_Utils::get_time_as_ms();
	diff = t2 - t1;
	TEST_ASSERT_INT_WITHIN(10, 1000, diff);

}

int main(void) {
	DEBUG_PRINTF_INIT();

	UNITY_BEGIN();

	RUN_TEST(test_timers);

	return UNITY_END();
}
