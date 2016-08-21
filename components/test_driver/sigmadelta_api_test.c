
#include <esp_types.h>
#include "rom/ets_sys.h"

#include "driver/sigmadelta.h"
#include "sigmadelta_api_test.h"
#include "soc/gpio_sd_reg.h"

#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"
#include "freertos/task.h"
#include "string.h"


void sigmadelta_check_register(enum_sigmadelta_chanl_t sigmadelta_chal)
{
    if(sigmadelta_chal>SIGMADELTA_CHANNEL7){
        ets_printf("the sigmadelta channel:%d not exits\n",sigmadelta_chal);
        return;   
    }
	ets_printf("---------sigmadelta_chal %d reg----------\n",sigmadelta_chal);
	ets_printf("SIGMADELTA%u=0x%x\n",sigmadelta_chal,READ_PERI_REG(GPIO_SIGMADELTA0_REG+sigmadelta_chal*4));
}


/*------------------------------------------
        sigmadelta demo
------------------------------------------*/
#define SIGMADELTA0__GPIO_NUM     (12)
#define SIGMADELTA0_DUTY          (45)
#define SIGMADELTA0_PRESCALE      (40)

#define SIGMADELTA1__GPIO_NUM     (13)
#define SIGMADELTA1_DUTY          (45)
#define SIGMADELTA1_PRESCALE      (40)

#define SIGMADELTA2__GPIO_NUM     (14)
#define SIGMADELTA2_DUTY          (45)
#define SIGMADELTA2_PRESCALE      (40)

#define SIGMADELTA3__GPIO_NUM     (15)
#define SIGMADELTA3_DUTY          (45)
#define SIGMADELTA3_PRESCALE      (40)

#define SIGMADELTA4__GPIO_NUM     (16)
#define SIGMADELTA4_DUTY          (45)
#define SIGMADELTA4_PRESCALE      (40)

#define SIGMADELTA5__GPIO_NUM     (17)
#define SIGMADELTA5_DUTY          (45)
#define SIGMADELTA5_PRESCALE      (40)

#define SIGMADELTA6__GPIO_NUM     (18)
#define SIGMADELTA6_DUTY          (45)
#define SIGMADELTA6_PRESCALE      (40)

#define SIGMADELTA7__GPIO_NUM     (19)
#define SIGMADELTA7_DUTY          (45)
#define SIGMADELTA7_PRESCALE      (40)

#define TABLE_CNT(TABLE) (sizeof(TABLE)/sizeof(TABLE[0]))

//Gpio_num,Prescale,Duty
uint8_t sigmadelte_test_table[][3]={
	{SIGMADELTA0__GPIO_NUM,SIGMADELTA0_DUTY,SIGMADELTA0_PRESCALE},//Channel0
	{SIGMADELTA1__GPIO_NUM,SIGMADELTA1_DUTY,SIGMADELTA1_PRESCALE},//Channel1
	{SIGMADELTA2__GPIO_NUM,SIGMADELTA2_DUTY,SIGMADELTA2_PRESCALE},//Channel2
	{SIGMADELTA3__GPIO_NUM,SIGMADELTA3_DUTY,SIGMADELTA3_PRESCALE},//Channel3
	{SIGMADELTA4__GPIO_NUM,SIGMADELTA4_DUTY,SIGMADELTA4_PRESCALE},//Channel4
	{SIGMADELTA5__GPIO_NUM,SIGMADELTA5_DUTY,SIGMADELTA5_PRESCALE},//Channel5
	{SIGMADELTA6__GPIO_NUM,SIGMADELTA6_DUTY,SIGMADELTA6_PRESCALE},//Channel6
	{SIGMADELTA7__GPIO_NUM,SIGMADELTA7_DUTY,SIGMADELTA7_PRESCALE},//Channel7
};
void sigmadelta_demo()
{ 
    uint8_t i=0;
	for(i=0;i<TABLE_CNT(sigmadelte_test_table);i++){
		sigmadelta_bind_gpio_init(i,sigmadelte_test_table[i][0]);
		ets_printf("channel %u gpio %u\n",i,sigmadelte_test_table[i][0]);
		sigmadelta_set_duty(i,sigmadelte_test_table[i][1]);
		sigmadelta_set_prescale(i,sigmadelte_test_table[i][2]);
		sigmadelta_start(i);
	}
}

void sigmadelta_task(void *arg)
{
    static uint8_t i=0;
    uint8_t j=0;
    for(;;){
         i++;
			 vTaskDelay(500);
		
		 if(i>=20){
		 	ets_printf("Sigmadelta stop\n");
             for(j=0;j<TABLE_CNT(sigmadelte_test_table);j++){
                sigmadelta_stop(j);
			 }
			 vTaskDelete(NULL);
		 }
	}
}
void sigmadelta_test(char* cmd ,int* param)
{
    if(strcmp(cmd,"sigmadelta_demo")==0){
        sigmadelta_demo();
	    xTaskCreate(sigmadelta_task,"sigmadelta_task",200,NULL,6,NULL);
    }
}
