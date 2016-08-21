#include "driver/gpio.h"
#include "driver/pcnt.h" 
#include "string.h"
#include "rom/ets_sys.h"
uint8_t pulsecnt_test_table[][5]={
{PulseCntChannel0,19,20,21,22},
{PulseCntChannel1},	
{PulseCntChannel2},
{PulseCntChannel3},
{PulseCntChannel4},
{PulseCntChannel5}, 
{PulseCntChannel6},
{PulseCntChannel7},

};

#define PCNT_TEST_CHANNEL (0)
#define PCNT_GATE0_CTRL_GPIO_NUM (19)
#define PCNT_GATE0_SIG_GPIO_NUM (20)
void pcnt_demo()
{
    Pcnt_channel_config channel;
	pcnt_gate_t pcnt_gate,pcnt_gate1;
	memset(&channel,0,sizeof(channel));
	//Init common param
	channel.H_limit=300;
	channel.L_limit=-300;
	channel.channel=PCNT_TEST_CHANNEL;
	channel.Filter_tick=0x100;// us=0x100/80=3.2us
    //init gate0
	//channel.Gate0=(pcnt_gate_t*)os_malloc(sizeof(pcnt_gate_t));
	channel.Gate0=&pcnt_gate;
	channel.Gate0->ctrl_pin_num=PCNT_GATE0_CTRL_GPIO_NUM;
	channel.Gate0->sig_pin_num=PCNT_GATE0_SIG_GPIO_NUM;
	channel.Gate0->ctrl_high_level=HIGH_LEVEL_DECREASE;
	channel.Gate0->ctrl_low_level=LOW_LEVEL_DECREASE;
	channel.Gate0->sig_positive_edge=POSITIVE_EDGE_INCREASE;
	channel.Gate0->sig_negative_edge=NEGATIVE_EDGE_FORBIIDEN;
    //init gate1
	//channel.Gate1=(pcnt_gate_t*)os_malloc(sizeof(pcnt_gate_t));
	channel.Gate1=NULL;    
    pcnt_init_channel(&channel);

	Pcnt_intr_config intr_con;
	intr_con.channel=PCNT_TEST_CHANNEL;
	intr_con.thres0=-200;
	intr_con.thres1=-250;
	intr_con.PcntIntrEnMask=ENABLE_THRES1_INTR|\
		                    ENABLE_THRES0_INTR|\
		                    ENABLE_L_LIMIT_INTR|\
		                    ENABLE_H_LIMIT_INTR|\
		                    ENABLE_THR_ZERO_INTR;
	
    pcnt_set_intr(&intr_con);
	pcnt_intr_register(pcnt_intr_handle_demo,NULL);
	ETS_PCNT_INTR_ENABLE();
	pcnt_start_counter(channel.channel);
}
void pcnt_get_counters_value(enum_pcnt_channel_t channel)
{
    ets_printf("PcntChannel%d Counter %d\n",channel,pcnt_get_counter_value(channel));
}


void pcnt_test(char *cmd ,int* param)
{
    if(strcmp(cmd,"pcnt_demo")==0){
        ets_printf("pcnt_demo ok\n");
	    pcnt_demo();	
	}
	else if(strcmp(cmd,"pcnt_get_counter_value")==0){
       ets_printf("pcnt_get_counter_value ok\n");
	   ets_printf("channel %d pcnt_value=%d",param[0],pcnt_get_counter_value(param[0]));
	}
}

