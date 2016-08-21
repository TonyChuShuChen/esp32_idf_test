
#include <stdio.h>
#include "string.h"
#include <ctype.h>

#include <stdlib.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "soc/dport_reg.h"
#include "lwip/opt.h"
#include "netif/ppp/ppp_impl.h"

#define BZERO(s, n)		memset(s, 0, n)


#define GPIO_TEST       (1)
#define SIGMADELTA_TEST (1)
#define PCNT_TEST       (1)
#define RMT_TEST        (1)
#define LEDC_TEST       (1)

#define UART_TEST       (0)
#define I2C_TEST        (0)
#define MPWM            (0)
#define EPWM            (0)

#define I2S_TEST        (0)
#define SPI_TEST        (0)


#if GPIO_TEST
#include "gpio_api_test.h"
#include "driver/gpio.h"
#endif
#if SIGMADELTA_TEST
#include "sigmadelta_api_test.h"
#include "driver/sigmadelta.h"
#endif
#if LEDC_TEST
#include "ledc_pwm_api_test.h"
#include "driver/ledc_pwm.h"
#endif
#if PCNT_TEST
#include "pcnt_api_test.h"
#endif
#if RMT_TEST
#include "rmt_ctrl_api_test.h"
#endif

int UartRxString(uint8_t* dst, uint8_t max_length);

static void trim_trailing_space(char* str)
{
    char* end = str + strlen(str) - 1;
    while (end >= str && isspace((int) *end))
    {
        *end = 0;
        --end;
    }
}

void all_drive_init()
{
    SET_PERI_REG_MASK(PERIP_CLK_EN,DPORT_LEDC_CLK_EN);
    CLEAR_PERI_REG_MASK(PERIP_RST_EN,DPORT_LEDC_RST);
    //csc add  
    SET_PERI_REG_MASK(PERIP_CLK_EN,DPORT_RMT_CLK_EN);
    CLEAR_PERI_REG_MASK(PERIP_RST_EN,DPORT_RMT_RST);
     
	
    SET_PERI_REG_MASK(PERIP_CLK_EN,DPORT_PCNT_CLK_EN);
    CLEAR_PERI_REG_MASK(PERIP_RST_EN,DPORT_PCNT_RST);

	SET_PERI_REG_MASK(PERIP_CLK_EN,DPORT_SPI_CLK_EN);
    CLEAR_PERI_REG_MASK(PERIP_RST_EN,DPORT_SPI_RST);

	 SET_PERI_REG_MASK(PERIP_CLK_EN,DPORT_SPI_CLK_EN_1);
    CLEAR_PERI_REG_MASK(PERIP_RST_EN,DPORT_SPI_RST_1);

	SET_PERI_REG_MASK(PERIP_CLK_EN,DPORT_SPI_CLK_EN_2);
    CLEAR_PERI_REG_MASK(PERIP_RST_EN,DPORT_SPI_RST_2);

    SET_PERI_REG_MASK(PERIP_CLK_EN,DPORT_I2S0_CLK_EN);
    CLEAR_PERI_REG_MASK(PERIP_RST_EN,DPORT_I2S0_RST);

	SET_PERI_REG_MASK(PERIP_CLK_EN,DPORT_I2S1_CLK_EN);
    CLEAR_PERI_REG_MASK(PERIP_RST_EN,DPORT_I2S1_RST);

	SET_PERI_REG_MASK(PERIP_CLK_EN,DPORT_UART_CLK_EN);
    CLEAR_PERI_REG_MASK(PERIP_RST_EN,DPORT_UART_RST);

	SET_PERI_REG_MASK(PERIP_CLK_EN,DPORT_UART1_CLK_EN);
    CLEAR_PERI_REG_MASK(PERIP_RST_EN,DPORT_UART1_RST);

	SET_PERI_REG_MASK(PERIP_CLK_EN,DPORT_I2C_EXT0_CLK_EN);
    CLEAR_PERI_REG_MASK(PERIP_RST_EN,DPORT_I2C_EXT0_RST);
	
	SET_PERI_REG_MASK(PERIP_CLK_EN,DPORT_I2C_EXT1_CLK_EN);
    CLEAR_PERI_REG_MASK(PERIP_RST_EN,DPORT_I2C_EXT1_RST);

	SET_PERI_REG_MASK(PERIP_CLK_EN,DPORT_PWM0_CLK_EN);
    CLEAR_PERI_REG_MASK(PERIP_RST_EN,DPORT_PWM0_RST);
	SET_PERI_REG_MASK(PERIP_CLK_EN,DPORT_PWM1_CLK_EN);
    CLEAR_PERI_REG_MASK(PERIP_RST_EN,DPORT_PWM1_RST);
	SET_PERI_REG_MASK(PERIP_CLK_EN,DPORT_PWM2_CLK_EN);
    CLEAR_PERI_REG_MASK(PERIP_RST_EN,DPORT_PWM2_RST);
	SET_PERI_REG_MASK(PERIP_CLK_EN,DPORT_PWM3_CLK_EN);
    CLEAR_PERI_REG_MASK(PERIP_RST_EN,DPORT_PWM3_RST);

}
char cmd[128]={0};
int32_t param[10]={0};
#define DBG_LINE(var,...) //ets_printf("[%s##%u]\n",__FUNCTION__,__LINE__)
void driver_test(void* arg)
{
   
	 all_drive_init();
	
    while(1){
		char cmdline[256]={0};
		char buffer[10]={0};
	    char *pos=NULL;
		char * last_pos=cmdline;
		uint8_t i=0;
		uint8_t space_cnt=0;
		char* pc=NULL;
        UartRxString((uint8_t*) cmdline, sizeof(cmdline) - 1);
		trim_trailing_space(cmdline);
		if (strlen(cmdline) == 0)
        {
        	DBG_LINE();
            continue;
        }
		//ets_printf("data=0x%x\n",strtol("0x12345678",NULL,0));
		ets_printf("cmdline:%s\n",cmdline);

		BZERO(cmd,sizeof(cmd));
		BZERO(param,sizeof(param));
		BZERO(buffer,sizeof(buffer));
		/*Find Space Key Cnt*/
		pc=cmdline;
		while(pc)
		{
			pc = strchr(pc,' ');
			if(pc)
			{
				space_cnt++;
				pc+=1;
			}
		}
        if(0==space_cnt){
             MEMCPY(cmd,cmdline,sizeof(cmdline));
		}
		else{
            for(i=0;i<space_cnt+1;i++){
				
                pos=strchr(last_pos,' ');
				if(0==i){
                    MEMCPY(cmd,cmdline,pos-cmdline);
				}
				else if(i==space_cnt){//Find the last space_cnt
				    BZERO(buffer,sizeof(buffer));
                    MEMCPY(buffer,last_pos,strlen(last_pos));				
			        param[i-1]=strtol(buffer,NULL,0);
				}
				else{
					BZERO(buffer,sizeof(buffer));
				    MEMCPY(buffer,last_pos,pos-last_pos);
			        param[i-1]=strtol(buffer,NULL,0);
				    
				}
				last_pos=pos+1;
			}
		}

		DBG_LINE();
	#define DBG_PARAM_ANYLISE (0)
	#if DBG_PARAM_ANYLISE
		ets_printf("cmd:%s\n",cmd);
		for(i=0;i<10;i++){
            ets_printf("param[%d]=0x%x\n",i,param[i]);
		}
	#endif
    #if GPIO_TEST
        gpio_test(cmd,param);
    #endif
    #if SIGMADELTA_TEST
        sigmadelta_test(cmd,param);
    #endif
    #if LEDC_TEST
        ledc_test(cmd,param);
    #endif
	#if RMT_TEST
	    nec_test(cmd,param);
	#endif
	#if PCNT_TEST
        pcnt_test(cmd,param);
	#endif
	
	    
   }

    vTaskDelete(NULL);
}

