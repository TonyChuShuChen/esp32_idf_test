/*
 Test for multicore FreeRTOS. This test spins up threads, fiddles with queues etc.
*/

#include <esp_types.h>
#include <stdio.h>
#include <stdlib.h>
#include "rom/ets_sys.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"
#include "freertos/xtensa_api.h"
#include "unity.h"
#include "soc/uart_reg.h"
#include "soc/dport_reg.h"
#include "soc/io_mux_reg.h"


volatile static int done;
volatile static int error;

static void tskTestRand(void *pvParameters) {
	int l;
	srand(0x1234);
	vTaskDelay((int)pvParameters/ portTICK_PERIOD_MS);
	l=rand();
	printf("Rand1: %d\n", l);
	if (l!=869320854) error++;
	vTaskDelay((int)pvParameters/ portTICK_PERIOD_MS);
	l=rand();
	printf("Rand2: %d\n", l);
	if (l!=1148737841) error++;
	done++;
	vTaskDelete(NULL);
}



// TODO: split this thing into separate orthogonal tests
TEST_CASE("Test for per-task non-reentrant tasks", "[freertos]")
{
	done=0;
	error=0;
	xTaskCreatePinnedToCore(tskTestRand, "tsk1", 2048, (void*)100, 3, NULL, 0);
	xTaskCreatePinnedToCore(tskTestRand, "tsk2", 2048, (void*)200, 3, NULL, 0);
	xTaskCreatePinnedToCore(tskTestRand, "tsk3", 2048, (void*)300, 3, NULL, 1);
	xTaskCreatePinnedToCore(tskTestRand, "tsk4", 2048, (void*)400, 3, NULL, 0);
	while(done!=4) {
		vTaskDelay(1000 / portTICK_PERIOD_MS);
	}
	TEST_ASSERT(error==0);
}

