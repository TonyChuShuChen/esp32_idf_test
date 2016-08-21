#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "unity.h"


#include "driver_test_entry.h"

void unityTask(void *pvParameters) 
{
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    unity_run_menu();
    while(1);
}

void app_main() 
{
   // xTaskCreatePinnedToCore(unityTask, "unityTask", 2048, NULL, 5, NULL, 0);
    xTaskCreate(driver_test,"driver_test",2048,NULL,5,NULL);

	
}

