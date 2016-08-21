
#include "rom/ets_sys.h"
#include "driver/gpio.h"

#include "driver/ledc_pwm_struct.h"
#include "driver/ledc_pwm.h"


#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"
#include "freertos/task.h"
#include "string.h"


#define LEDC_CHANNEL0_GPIO (16)

void ledc_pwm_handler(void* arg)
{
    uint32_t intr_st=0;
	uint16_t i=0;
	intr_st=ledc_reg_s->LEDC_INT_ST.val;
	ledc_reg_s->LEDC_INT_CLR.val=intr_st;
	for(i=0;i<8;i++){
        if(intr_st&BIT(8+i)){
            ets_printf("Ledc intr channel %u\n",i);
		}
	}
}
void ledc_pwm_check_reg(uint8_t channel)
{
    ets_printf("channel %u \n",channel);
	//CHANNEL
	ets_printf("LEDC_HSCH%u_CONF0_REG=0x%08x\n",channel,READ_PERI_REG(LEDC_HSCHX_CONF0_REG(channel)));
	ets_printf("LEDC_HSCH%u_HPOINT_REG=0x%08x\n",channel,READ_PERI_REG(LEDC_HSCHX_HPOINT_REG(channel)));
	ets_printf("LEDC_HSCH%u_CONF1_REG=0x%08x\n",channel,READ_PERI_REG(LEDC_HSCHX_CONF1_REG(channel)));
	ets_printf("LEDC_HSCH%u_DUTY_REG=0x%08x\n",channel,READ_PERI_REG(LEDC_HSCHX_DUTY_REG(channel)));
    ets_printf("LEDC_HSCH%u_DUTY_R_REG=0x%08x\n",channel,READ_PERI_REG(LEDC_HSCHX_DUTY_R_REG(channel)));
    //TIME
    uint32_t time_select=0;
	time_select=READ_PERI_REG(LEDC_HSCHX_CONF0_REG(channel))&0x03;
	ets_printf("LEDC_HSTIMER%u_CONF_REG=0x%08x\n",time_select,READ_PERI_REG(LEDC_HSTIMERX_CONF_REG(time_select)));
	ets_printf("LEDC_HSTIMER%u_VALUE_REG=0x%08x\n",time_select,READ_PERI_REG(LEDC_HSTIMERX_VALUE_REG(time_select)));
    //Status
	ets_printf("LEDC_INT_RAW_REG=0x%08x\n",READ_PERI_REG(LEDC_INT_RAW_REG));
	ets_printf("LEDC_INT_ST_REG=0x%08x\n",READ_PERI_REG(LEDC_INT_ST_REG));
	ets_printf("LEDC_INT_ENA_REG=0x%08x\n",READ_PERI_REG(LEDC_INT_ENA_REG));
}
/*------------------------------------------
        ledc demo
------------------------------------------*/
//gpio_num,fre,duty
uint32_t ledc_test_table[][3]={
{12,2000,1000},//TIME0
{13,1,1000},//TIME1
{14,4*1000,1000},//TIME2
{15,3*1000,5},//TIME3

{16,2000,4000},
{17,1,3000},
{18,4*1000,2000},
{19,3*1000,1000},

};
#define TABLE_CNT(TABLE) (sizeof(TABLE)/sizeof(TABLE[0]))

void ledc_test_task(void* arg){
   uint8_t i=0;
   while(1){
       vTaskDelay(10000);
	   ets_printf("ledc stop\n");
	   for(i=0;i<TABLE_CNT(ledc_test_table);i++){
	       ledc_pwm_stop(0,i,i&0x01);
	   }
	   vTaskDelay(10000);
      for(i=0;i<TABLE_CNT(ledc_test_table);i++){
	       ledc_pwm_stop(0,i,i&0x01);
	   } 
	   
	   
   };
}

void ledc_pwm_demo()
{   
    uint8_t i=2;
	for(i=0;i<TABLE_CNT(ledc_test_table);i++)
	{
        ledc_channel_config ledc_channel_config0;
        ledc_channel_config0.gpio_num=ledc_test_table[i][0];
        ledc_channel_config0.channel=i;
        ledc_channel_config0.duty=ledc_test_table[i][2];
        ledc_channel_config0.fre_hz=ledc_test_table[i][1];
        ledc_channel_config0.intr_enable=false;
        ledc_channel_config0.ledc_pre_depth=LEDC_PWM_DUTY_DEPTH_12_BIT;
        ledc_channel_config0.time=i&3;
        //ets_printf("fre=%u table_fre=%u\n",ledc_channel_config0.fre_hz,ledc_test_table[2][1]);
        ledc_pwm_channel_config(0,&ledc_channel_config0);
        ledc_pwm_intr_register(ledc_pwm_handler,NULL);
        ETS_LEDC_INTR_ENABLE();
        ledc_pwm_start(0,i);
	}
	//xTaskCreate(ledc_test_task,"ledc_test_task",512,NULL,5,NULL);
}
void ledc_all_channel_set_duty(uint32_t duty)
{
    uint8_t i=0;
	do{
        ledc_pwm_set_duty(0,i,duty);		   
		ledc_pwm_start(0,i);
	}while(++i<=LEDC_PWM_CHANNEL7);
}
void ledc_all_channel_set_fre(uint32_t fre)
{
    uint8_t i=0;
	do{
        ledc_pwm_set_fre(0,i,fre);
	}while(++i<=LEDC_PWM_CHANNEL7);

}
void ledc_all_channel_set_stop(uint8_t level)
{
    uint8_t i=0;
	do{
        ledc_pwm_stop(0,i,level);
	}while(++i<=LEDC_PWM_CHANNEL7); 
}
void ledc_all_channel_show_fre()
{
    uint8_t i=0;
	do{
        ets_printf("LEDC Channel%u Fre %u\n",i,ledc_pwm_get_fre_hz(0,i));
	}while(++i<=LEDC_PWM_CHANNEL7);
}
void ledc_all_channel_show_duty()
{
    uint8_t i=0;
	do{
        ets_printf("LEDC Channel%u Duty %u\n",i,ledc_pwm_get_duty(0,i));
	}while(++i<=LEDC_PWM_CHANNEL7);
}

void ledc_test(char* cmd ,int* param)
{   

    /*The Demo Test*/
    if(strcmp(cmd,"ledc_demo")==0){
        //ets_printf("ledc_pwm_demo-2\n");
        ledc_pwm_demo();
    }

	/*Duty and Fre Test*/
	else if(strcmp(cmd,"ledc_set_duty")==0){
        ets_printf("ledc_set_duty ok ,chanel %d duty %u\n",param[0],param[1]);
		if(param[0]<=LEDC_PWM_CHANNEL7)//Single Channel
		{		   
           ledc_pwm_set_duty(0,param[0],param[1]);
		   ledc_pwm_start(0,param[0]);
		}
		else{
            ledc_all_channel_set_duty(param[1]);
		}
	}
	else if(strcmp(cmd,"ledc_set_fre")==0){
		 ets_printf("ledc_set_fre ok ,chanel %d fre %u\n",param[0],param[1]);
		if(param[0]<=LEDC_PWM_CHANNEL7)//Single Channel
		{
		    ledc_pwm_set_fre(0,param[0],param[1]);
		}
		else{
            ledc_all_channel_set_fre(param[1]);
		}

	}
	else if(strcmp(cmd,"ledc_get_duty")==0){
       ledc_all_channel_show_duty();
	}
	else if(strcmp(cmd,"ledc_get_fre")==0){
       ledc_all_channel_show_fre();
	}
	else if(strcmp(cmd,"ledc_set_gradient")==0){
		ets_printf("ledc_set_gradient channel=%u  duty=%u direction=%u gradule_num=%u duty_cycle_num=%u duty_scale=%u\n",\
                                        param[0], param[1],   param[2],      param[3],         param[4],        param[5]);
        ledc_pwm_set_gradient(0,param[0],param[1],param[2],param[3],param[4],param[5]);
		ledc_pwm_start(0,param[0]);
	}
	else if(strcmp(cmd,"ledc_stop")==0){
       ledc_pwm_stop(0,param[0],param[1]);
	}
	else if(strcmp(cmd,"ledc_check")==0){
        ledc_pwm_check_reg(param[1]);
	}
}
