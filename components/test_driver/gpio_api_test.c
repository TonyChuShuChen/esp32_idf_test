#include "driver/gpio.h"
#include "rom/ets_sys.h"
#include "string.h"

#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"
#include "freertos/task.h"


/*******************************************************
                     gpio check register 
*******************************************************/
void gpio_check_register(enum_gpio_num_t gpio_num)
{
    if(gpio_num>=GPIO_PIN_COUNT||0==GPIO_PIN_MUX_REG[gpio_num]){
        ets_printf("io_num=%d not exits\n",gpio_num);
		return;
	}
    ets_printf("---------gpio_num %d reg----------\n",gpio_num);
	ets_printf("GPIO_IOMUX_%d=0x%08x\n",gpio_num,READ_PERI_REG(GPIO_PIN_MUX_REG[gpio_num]));
    ets_printf("GPIO_PIN%d_ADDR=0x%08x\n",gpio_num,READ_PERI_REG(GPIO_PIN_ADDR(gpio_num)));
	ets_printf("GPIO_OUT_REG=0x%08x\n",READ_PERI_REG(GPIO_OUT_REG));
	ets_printf("GPIO_OUT1_REG=0x%08x\n",READ_PERI_REG(GPIO_OUT1_REG));
	ets_printf("GPIO_ENABLE_REG=0x%08x\n",READ_PERI_REG(GPIO_ENABLE_REG));
	ets_printf("GPIO_ENABLE1_REG=0x%08x\n",READ_PERI_REG(GPIO_ENABLE1_REG));
	ets_printf("GPIO_IN_REG=0x%08x\n",READ_PERI_REG(GPIO_IN_REG));
	ets_printf("GPIO_IN1_REG=0x%08x\n",READ_PERI_REG(GPIO_IN1_REG));
	ets_printf("GPIO_STATUS_REG=0x%08x\n",READ_PERI_REG(GPIO_STATUS_REG));
	ets_printf("GPIO_STATUS1_REG=0x%08x\n",READ_PERI_REG(GPIO_STATUS1_REG));
}

/******************************************************
                      gpio intr demo
******************************************************/
#define GPIO_TEST_INTR_NUM (34)
void gpio_intr_handler_demo(void* arg)
{

    /*GPIO intr process*/	
    uint32_t gpio_intr_status=0;
    uint32_t gpio_intr_status_h=0;
	uint32_t gpio_num=0;
	ETS_GPIO_INTR_DISABLE();//Disable intr
	ets_printf("-----------Gpio Intr---------------\n");//Demo User ,when use the code ,Please delet the log
	gpio_intr_status=READ_PERI_REG(GPIO_STATUS_REG);
	gpio_intr_status_h=READ_PERI_REG(GPIO_STATUS1_REG);
	SET_PERI_REG_MASK(GPIO_STATUS_W1TC_REG,gpio_intr_status);//Clear intr for gpio0-gpio31
	SET_PERI_REG_MASK(GPIO_STATUS1_W1TC_REG,gpio_intr_status_h);//Clear intr for gpio32-39
	do{
        if(gpio_num<32){
            if(gpio_intr_status&BIT(gpio_num)){//gpio0-gpio31
                ets_printf("Intr Gpio%d\n",gpio_num);
            }			
		}
		else{
            if(gpio_intr_status_h&BIT(gpio_num-32)){
               ets_printf("Intr Gpio%d\n",gpio_num);
			}
		}
	}while(++gpio_num<GPIO_PIN_COUNT);
	ETS_GPIO_INTR_ENABLE();//Disable intr
	ets_printf("Exit Gpio handle\n");
}

static void gpio_intr_init(uint8_t gpio_num)
{
    gpio_config_t gpio_test;
	memset(&gpio_test,0,sizeof(gpio_test));
    gpio_test.GPIO_Pin=(uint64_t)((uint64_t)(1)<<gpio_num);
	
	gpio_test.GPIO_Mode=GPIO_Mode_Input;
	gpio_test.GPIO_IntrType=GPIO_PIN_INTR_ANYEDGE;
	gpio_test.GPIO_Pullup=GPIO_PullUp_DIS;
	gpio_test.GPIO_Pulldown=GPIO_PullDown_DIS;
    gpio_config(&gpio_test);
	//Register gpio handler
	gpio_intr_handler_register(gpio_intr_handler_demo,NULL);
	//Enable gpio intr
	ETS_GPIO_INTR_ENABLE(); //Enable intr
	
}
void gpio_intr_demo()
{
    gpio_intr_init(GPIO_TEST_INTR_NUM);
	ets_printf("gpio_intr_demo\n");
}


/******************************************************
          gpio input and output demo
******************************************************/
#define TABLE_ELEMENT_CNT(table) ((sizeof(table))/(sizeof(table[0])))
struct gpio_test_info{
uint8_t* gpio_test_table;
uint8_t gpio_test_num;
void* time_s;
};
uint8_t gpio_test_table[]={0,2,4,5,12,13,14,15,16,17,18,19,20,21,22,23,25,26,27,33,34,35,36,37,38,39};
void t1_callback(void* arg)
{
    static uint8_t level=0;
	static uint8_t cnt=0;
	uint8_t err_flag=0;
	struct gpio_test_info *gpio_test=(struct gpio_test_info*)(arg);
	uint8_t i=0;
	while(1){
        level=~level;
        ets_printf("Test cnt %u, level %u\n",cnt+1,level&0x01);
        for(i=0;i<gpio_test->gpio_test_num;i++){
            gpio_set_output_level(gpio_test->gpio_test_table[i],level&0x01);
        	if(gpio_get_input_level(gpio_test->gpio_test_table[i])!=(level&0x01))
        	{
        	    err_flag=1;
                ets_printf("[ERR] GPIO%u set_level %u get_level %u\n",gpio_test->gpio_test_table[i],level&0x01,gpio_get_input_level(gpio_test->gpio_test_table[i]));
        	}
			else{
               ets_printf("GPIO%u OK\n",gpio_test->gpio_test_table[i]);
			}
        }
        cnt++;
		if(err_flag==0){
          ets_printf("cnt %u test ok\n",cnt);  
		}
		err_flag=0;
        if(cnt>=10){
           ets_printf("Gpio input and output test end\n");
           vTaskDelete(NULL);
        }
		
		//vTaskDelay(2*1000);
		
	}
	
	
}
void gpio_input_output_demo()
{

	uint64_t gpio_bits=0;
    uint8_t i=0;
	for(i=0;i<TABLE_ELEMENT_CNT(gpio_test_table);i++){
        gpio_bits=(((uint64_t)1)<<gpio_test_table[i])|gpio_bits;
	}
    gpio_config_t gpio_test;
	memset(&gpio_test,0,sizeof(gpio_test));
    gpio_test.GPIO_Pin=gpio_bits;
	gpio_test.GPIO_Mode=GPIO_Mode_Input_OutPut;
	gpio_test.GPIO_IntrType=GPIO_PIN_INTR_DISABLE;
	gpio_test.GPIO_Pullup=GPIO_PullUp_EN;
	gpio_test.GPIO_Pulldown=GPIO_PullDown_EN;
    gpio_config(&gpio_test);

    static struct gpio_test_info gpio_test_infor; 

	TimerHandle_t t1=NULL;
   //t1=xTimerCreate("t1_time",(1000/portTICK_PERIOD_MS),pdTRUE,&gpio_test_infor,t1_callback);
   	do{
        gpio_test_infor.gpio_test_table=gpio_test_table;
		gpio_test_infor.gpio_test_num=TABLE_ELEMENT_CNT(gpio_test_table);
		gpio_test_infor.time_s=t1;
	}while(0);
    xTaskCreate(t1_callback,"t1_callback",1024,&gpio_test_infor,6,NULL);
	ets_printf("gpio_input_output_demo\n");
}

void gpio_test(char* cmd ,int* param)
{
    if(0==strcmp(cmd,"gpio_demo_in_out")){
		ets_printf("gpio_demo_in_out ok\n");
        gpio_input_output_demo();//
	}
    else if(0==strcmp(cmd,"gpio_demo_intr")){
		ets_printf("gpio_demo_intr ok\n");
        gpio_intr_demo();
		ets_printf("gpio 21 as intr sig\n");
		pin_func_as_gpio(21);
		gpio_set_output_level(21,0);
		ets_delay_us(100);
		gpio_set_output_level(21,1);
    }
	//else if()
	else if(0==strcmp(cmd,"gpio_check_reg")){
       // ets_printf();
       gpio_check_register(param[0]);
	}
	
}




