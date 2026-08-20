#include <zephyr/drivers/gpio.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(Task1, LOG_LEVEL_DBG);
#define STACK_SIZE 1024

#define SLEEP_TIME_MS 1000


#define PRIO_COOP    (-1)
#define PRIO_MED     5
#define PRIO_HIGH    3
#define PRIO_LOW     7


/* ---------- Cooperative thread (priority < 0) ---------- */

void t_coop_fn(void *p1, void *p2, void *p3)
{
	while (1) {
		for (int i = 0; i < 5; i++) {
			LOG_INF("T_COOP busy work iteration %d", i + 1);
			/* Busy-wait to simulate work without yielding the CPU.
			 * Because this thread is cooperative (prio -1), no
			 * preemptive thread (prio >= 0) can interrupt it here.
			 */
			for (volatile int j = 0; j < 500000; j++) {
				/* burn cycles */
			}
		}
		LOG_INF("T_COOP yielding");
		k_yield();
	}
}


void med_fn(void *p1, void *p2, void *p3)
{
    LOG_INF("[MED] starting - will run 3 steps without yielding");

    for (int i = 0; i < 3; i++) {
        k_busy_wait(40000);   
        LOG_INF("[MED] step %d/3 - still holding CPU  tick=%u",
                i + 1, k_uptime_get_32());
    }

    LOG_INF("[MED] yielding now - HIGH and LOW can run");
    //k_yield();
    k_msleep(100);

    LOG_INF("[MED] done");
}


void high_fn(void *p1, void *p2, void *p3)
{
    LOG_INF("[HIGH] entering main loop - will preempt LOW whenever Ready");

    for (int i = 0; i < 10; i++) {
        LOG_INF("[HIGH] step %d  tick=%u", i, k_uptime_get_32());
        k_msleep(200);
    }

    LOG_INF("[HIGH] done");
}


void low_fn(void *p1, void *p2, void *p3)
{
    LOG_INF("[LOW] started");

    for (int i = 0; i < 10; i++) {
        LOG_INF("[LOW] step %d  tick=%u", i, k_uptime_get_32());

        k_msleep(300);
    }

    LOG_INF("[LOW] done");
}


K_THREAD_DEFINE(t_med, STACK_SIZE, med_fn, NULL, NULL, NULL, PRIO_MED, 0, 0);
K_THREAD_DEFINE(t_high, STACK_SIZE, high_fn, NULL, NULL, NULL, PRIO_HIGH, 0, 0);
K_THREAD_DEFINE(t_low,  STACK_SIZE, low_fn,  NULL, NULL, NULL, PRIO_LOW,  0, 0);
K_THREAD_DEFINE(t_coop, STACK_SIZE, t_coop_fn, NULL, NULL, NULL, PRIO_COOP, 0, 0);

int main(void)
{
    LOG_INF("=== L1 Task1 Ready v/s Waiting   k_yield v/s k_sleep ===");
   // LOG_INF("COOP prio=%d (cooperative)  HIGH prio=%d  LOW prio=%d",
    LOG_INF("MEDIUM prio=%d  HIGH prio=%d  LOW prio=%d",
            PRIO_MED, PRIO_HIGH, PRIO_LOW);
    return 0;
}
