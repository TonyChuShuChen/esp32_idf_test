#include "rom/ets_sys.h"

#include "driver/gpio.h"

#include "driver/rmt_ctrl.h"
#include "rmt_ctrl_api_test.h"

#include "string.h"


#define DBG_RMT_CTRL_NEC(format,...) //ets_printf(format,##__VA_ARGS__)
#define DBG_RMT_CTRL_ENABLE (0)

LOCAL  uint32_t nec_rx_decode(enum rmt_channel chan_num);
LOCAL void nec_show_buffer2(uint32_t* buffer,uint16_t cnt);

void rmt_ctrl_intr(void* arg)
{

	uint32_t intr_st=READ_PERI_REG(RMT_INT_ST_REG);
	uint32_t i=0;
	uint8_t channel=0;
	uint8_t int_type=0;

    DBG_RMT_CTRL_NEC("Intr st:0x%08x\n",intr_st);
	for(i=0;i<24;i++){
		
		if(intr_st&(BIT(0))){
	        channel=i/3;
			switch(int_type=i%3){			
            	case 0://TX END and WILL HAVE CONTINUE TX
					ets_printf("channel %d tx end\n",channel);
                	WRITE_PERI_REG(RMT_INT_CLR_REG, BIT(channel*3)); 			 
                	//CLEAR_PERI_REG_MASK(RMT_INT_ENA_REG, BIT(channel*3+0)|BIT(channel*3+2));
                    rmt_ctrl_memory_rw_rst(channel);
					break;
                
               case 1://RX_END and DECODE
				   ets_printf("channel %d rx end\n",channel);
                   WRITE_PERI_REG(RMT_INT_CLR_REG, BIT(channel*3+1));
                   CLEAR_PERI_REG_MASK(RMT_INT_ENA_REG, BIT(channel*3+1)|BIT(channel*3+2));
                   nec_rx_decode(channel);
				   SET_PERI_REG_MASK(RMT_INT_ENA_REG, BIT(channel*3+1)|BIT(channel*3+2));
				   rmt_ctrl_memory_rw_rst(channel); 
                   break;
    			case 2://ERR
				   ets_printf("channel %d err\n",channel);
    			   WRITE_PERI_REG(RMT_INT_CLR_REG, BIT(channel*3+2));
				   
				   rmt_ctrl_memory_rw_rst(channel); 
				   SET_PERI_REG_MASK(RMT_INT_ENA_REG,BIT(channel*3+2));
				   break;
    			default:
    				
    				break;		
	     }
	 }
	 intr_st>>=1;
   }

}
#define NEC_CHECK_MACRO_VALUE (0X7FFF)
#define NEC_CHECK_MACRO_ERR(A) (A>NEC_CHECK_MACRO_VALUE)
#define NEC_ERR_PRINT(format,...)   do{\
		ets_printf("[error][%s#%u]",__FUNCTION__,__LINE__);\
		ets_printf(format,##__VA_ARGS__);\
	}while(0)
void nec_check_macro()
{
    ets_printf("nec_check_macro\n");
    //Check Start Macro
    if(NEC_CHECK_MACRO_ERR(START_FLAG_DURATION_HIGH_TCIK_CNT)){
       NEC_ERR_PRINT("START_FLAG_DURATION_HIGH_TCIK_CNT=0x%x\n",START_FLAG_DURATION_HIGH_TCIK_CNT);
	}
	if(NEC_CHECK_MACRO_ERR(START_FLAG_DURATION_LOW_TCIK_CNT)){
       NEC_ERR_PRINT("START_FLAG_DURATION_LOW_TCIK_CNT=0x%x\n",START_FLAG_DURATION_LOW_TCIK_CNT);
	}
	//Check Ture
	if(NEC_CHECK_MACRO_ERR(LOGIC_TRUE_DURATION_HIGH_TICK_CNT)){
       NEC_ERR_PRINT("LOGIC_TRUE_DURATION_HIGH_TICK_CNT=0x%x\n",LOGIC_TRUE_DURATION_HIGH_TICK_CNT);
	}
	if(NEC_CHECK_MACRO_ERR(LOGIC_TRUE_DURATION_LOW_TICK_CNT)){
       NEC_ERR_PRINT("LOGIC_TRUE_DURATION_LOW_TICK_CNT=0x%x\n",LOGIC_TRUE_DURATION_LOW_TICK_CNT);
	}
	//Check False
	if(NEC_CHECK_MACRO_ERR(LOGIC_FALSE_DURATION_LOW_TICK_CNT)){
       NEC_ERR_PRINT("LOGIC_FALSE_DURATION_LOW_TICK_CNT=0x%x\n",LOGIC_FALSE_DURATION_LOW_TICK_CNT);
	}
	if(NEC_CHECK_MACRO_ERR(LOGIC_FALSE_DURATION_HIGH_TICK_CNT)){
       NEC_ERR_PRINT("LOGIC_FALSE_DURATION_HIGH_TICK_CNT=0x%x\n",LOGIC_FALSE_DURATION_HIGH_TICK_CNT);
	}
	//Check Idle REPEAT_STEP3_TICK_CNT
	if(NEC_CHECK_MACRO_ERR(REPEAT_STEP3_TICK_CNT)){
       NEC_ERR_PRINT("REPEAT_STEP3_TICK_CNT=0x%x\n",REPEAT_STEP3_TICK_CNT);
	}
	
}
/*-----------------------------------------------------------------
                                   RMT CTRL TX DEMO TEST
-----------------------------------------------------------------*/
LOCAL uint32_t nec_start_bit(void){
    uint32_t result;
    return result=(((START_FLAG_DURATION_LOW_TCIK_CNT)<<16)|(START_FLAG_DURATION_HIGH_TCIK_CNT+0x8000));
}

LOCAL uint32_t nec_logic_high(void){ 
   uint32_t result=0;
   return result=((LOGIC_TRUE_DURATION_LOW_TICK_CNT<<16)|(LOGIC_TRUE_DURATION_HIGH_TICK_CNT+0x8000));          
}

LOCAL uint32_t nec_logic_low(void){
    uint32_t result;
    return result=(LOGIC_FALSE_DURATION_LOW_TICK_CNT<<16)|(LOGIC_FALSE_DURATION_HIGH_TICK_CNT+0x8000);  
}

LOCAL uint32_t nec_get_idle(uint16_t address,uint16_t cmd)
{
   // uint16_t i=0;
    uint8_t pop_cnt=0;
    uint32_t idle_value=0;
    uint32_t seg_men=LOGIC_TRUE_DURATION_HIGH_TICK_CNT+0x8000;
	
    pop_cnt=__builtin_popcount(address); 
    pop_cnt+=__builtin_popcount(cmd);
    
    idle_value=NEC_PERIOD_TICK_CNT-START_FLAG_DURATION_TICK_CNT-(pop_cnt)*(LOGIC_TRUE_TICK_CNT)-(32-pop_cnt)*(LOIGC_FALSE_TICK_CNT);
    return idle_value=(seg_men)|(idle_value<<16);
}
//9ms_h+2.25ms_l+0.56ms_h+low 
LOCAL uint64_t nec_repeat_complete(void)
{
    uint64_t repeat_val_complete=0;
    uint64_t start_val=0;
    uint64_t repeat_val=0;
    start_val=(REPEAT_STEP0_TICK_CNT|0X8000)|((REPEAT_STEP1_TICK_CNT)<<16);
    repeat_val=(REPEAT_STEP2_TICK_CNT|0x8000)|((REPEAT_STEP3_TICK_CNT)<<16);
	
    return (repeat_val_complete=(start_val<<32)|(repeat_val));
}
void nec_cofnig_tx_data(enum rmt_channel channel,uint16_t address,uint16_t cmd,uint8_t repeat_num)
{
    uint32_t start_value=nec_start_bit();
    uint32_t value_true=nec_logic_high();
    uint32_t value_false=nec_logic_low();
    uint32_t idle_value=nec_get_idle(address,cmd);
    uint32_t i=0;
    uint32_t memory_data[RMT_CHANNEL_MEMORY_WORD_CNT*8]={0};
	uint32_t mem_cnt=0;
	uint32_t mem_channel_cnt=(RMT_GET_REG_MEM_SIZE_CHX(channel));	 
    //start flag
    memory_data[mem_cnt++]=start_value;
    //write data
    for(i=0;i<16;i++){
	    //logic 1
        if((address>>i)&BIT(0)){
            memory_data[mem_cnt++]=value_true;//
	    }
	    //logic 0
	    else{
            memory_data[mem_cnt++]=value_false;//
		}
    }
    // write data
    for(i=0;i<16;i++){
	     //logic 1
        if((cmd>>i)&BIT(0)){
            memory_data[mem_cnt++]=value_true;//
	    }
	    //logic 0
	    else{
            memory_data[mem_cnt++]=value_false;//
	    }
    }
   switch(repeat_num){
       case 0:
	       memory_data[mem_cnt++]=idle_value;//
	       memory_data[mem_cnt++]=0x00;
	       break;
	  default:
	  {
           memory_data[mem_cnt++]=idle_value;//
	       uint64_t data=0;
	       data=nec_repeat_complete();
           uint32_t i=0;
	       for(i=0;i<repeat_num;i++){
	           memory_data[mem_cnt++]=data>>32;//data
	           memory_data[mem_cnt++]=data&(0XFFFFFFFF);//data	   
	       }
		   
	       memory_data[mem_cnt++]=0x00;
           break;
	  }
	    
   }
   if(i>=mem_channel_cnt*RMT_CHANNEL_MEMORY_WORD_CNT){
       ets_printf("RMT_CHANNEL MEMORY WORD CNT ERR mem_cnt %u\n",mem_channel_cnt);
	   return ;
   }
   //nec_show_buffer2(memory_data,64);
  // ets_printf("---------------------------------------\n");
   rmt_ctrl_tx_wr_memory(channel,memory_data,64);
  
}

void nec_tx_init()
{
    rmt_ctrl_channel_config rmt_ctrl_config;
	rmt_ctrl_config.channel=RMT_CTRL_CHANNEL0;
	rmt_ctrl_config.gpio_num=23;
	rmt_ctrl_config.intr_enable=1;
	rmt_ctrl_config.channel_mem_cnt=1;
	rmt_ctrl_config.rmt_mode=RMT_TX_MODE;
	rmt_ctrl_config.mode_config.tx_config.carrier_fre_Hz=33*1000;
	rmt_ctrl_config.mode_config.tx_config.carrier_duty=50;
	rmt_ctrl_config.mode_config.tx_config.carrier_level=RMT_CARRIER_HIGH_LEVEL;
	rmt_ctrl_config.mode_config.tx_config.idle_level=RMT_CARRIER_LOW_LEVEL;
	rmt_ctrl_init(&rmt_ctrl_config);
    rmt_ctrl_intr_register(rmt_ctrl_intr,NULL);
	ETS_RMT_CTRL_ENABLE();
}
void nec_tx_dbg_memory(enum rmt_channel channel)
{
    uint32_t buffer[64*8]={0};

	rmt_ctrl_dbg_memory(channel,buffer);
	nec_show_buffer2(buffer,64);
}
void nec_tx_test()
{
    nec_tx_init();
    nec_cofnig_tx_data(0,0x1234,0x5678,1);
    rmt_ctrl_tx_start(RMT_CTRL_CHANNEL0);
	nec_tx_dbg_memory(0);
}

/*-----------------------------------------------------------------
                                   RMT CTRL RX DEMO TEST
-----------------------------------------------------------------*/
LOCAL void nec_show_buffer2(uint32_t* buffer,uint16_t cnt)
{
    uint16_t i=0;
	ets_printf("--------------nec_show_buffer2-----------------------------------\n");
	for(i=0;i<cnt;i++)
	{
		if(0==i%10){
           ets_printf("\n");
	   }
       ets_printf("0x%04x 0x%04x\n",buffer[i]&0xffff,(buffer[i]>>16)&0xffff);

	}
	ets_printf("\n");
}
LOCAL void nec_show_buffer(uint16_t* buffer ,uint16_t cnt)
{
	uint16_t i=0;
	ets_printf("\n-------------------------------------------------------------------------------------\n");
	for(i=0;i<cnt;i++){
		if(i%20==0){
			ets_printf("\n");   
		}
		ets_printf("0x%04x ",buffer[i]);
	}
	ets_printf("\n");
}

//array:the array which saved data
//len	:will visit array depth 
//pos	:will get array pos
LOCAL bool nec_decode_find_start_flag(uint16_t* arry,uint16_t len,uint16_t* pos,uint16_t offset)
{
	uint16_t i=0;
	for(i=0;i<len;i+=2){
		if((START_FLAG_DURATION_HIGH_TCIK_CNT-offset<=*(arry+i))&&((*(arry+i)<=START_FLAG_DURATION_HIGH_TCIK_CNT+offset))){
			if((START_FLAG_DURATION_LOW_TCIK_CNT-offset<=*(arry+i+1))&&((*(arry+i+1)<=START_FLAG_DURATION_LOW_TCIK_CNT+offset))){
			  *pos=i+1;
		return true;
		 }
	 } 		  
	}
	return false;
}
//array:the array which saved data
//len	:will visit array depth 
//pos	:will get array pos
LOCAL bool nec_decode_find_repeat_flag(uint16_t* arry,uint16_t len,uint16_t offset)
{
	uint16_t i=0;
	for(i=0;i<len;i+=3){
		if((REPEAT_STEP0_TICK_CNT-offset<=*(arry+i))&&((*(arry+i)<=REPEAT_STEP0_TICK_CNT+offset))){
			if((REPEAT_STEP1_TICK_CNT-offset<=*(arry+i+1))&&((*(arry+i+1)<=REPEAT_STEP1_TICK_CNT+offset))){
			if((REPEAT_STEP2_TICK_CNT-offset<=*(arry+i+2))&&((*(arry+i+2)<=REPEAT_STEP2_TICK_CNT+offset)))
				return true;
		 }
	 } 		  
	}
	return false;
}
// array:the array save content
//offset :rmt_ctrl can toletate err
//	data :the data analyse data if return true .
//data_len:the data_bit len
LOCAL bool nec_decode_analyse_data(uint16_t * arry,uint16_t* pos,uint16_t offset,uint8_t* data,uint8_t data_len)
{
	uint16_t i=0;
	*data=0;
	for(i=0;i<data_len;i+=2){
	  
	 // logic '1' high level
	 if((LOGIC_TRUE_DURATION_HIGH_TICK_CNT-offset<=*(arry+i))&&((*(arry+i)<=LOGIC_TRUE_DURATION_HIGH_TICK_CNT+offset))){
		//logic '1' low level 
		if((LOGIC_TRUE_DURATION_LOW_TICK_CNT-offset<=*(arry+i+1))&&((*(arry+i+1)<=LOGIC_TRUE_DURATION_LOW_TICK_CNT+offset))){
			//ets_printf("1 ");
				  (*data)=((*data)>>1)|(0x80);
		}
		else if((LOGIC_FALSE_DURATION_LOW_TICK_CNT-offset<=*(arry+i+1))&&((*(arry+i+1)<=LOGIC_FALSE_DURATION_LOW_TICK_CNT+offset))){
				 // ets_printf("0 ");
			 (*data)=(*data)>>1;
		}
		else{
				   DBG_RMT_CTRL_NEC("1Rmt_decode_analyse_data Err, it is not logic '0' not logic '1',data=0x%x!\n",*(arry+i+1));
			  return false;
		}
		   
	 }
	  else{
		     DBG_RMT_CTRL_NEC("2Rmt_decode_analyse_data Err, it is not logic '0' not logic '1',data=0x%x!\n",*(arry+i+1));
		  return false;
	  }
	}
	//when	for end, the i =pos+1
	(*pos)=i-1;
	return true;
}
LOCAL uint32_t nec_rx_decode_status_machine(uint16_t* mem_array,uint16_t len)
{
	
	 enum rmt_decode_status decode_status=FIND_START;
	 uint16_t* current_array_pos=mem_array;
	 uint16_t arry_pos=0;
	 uint8_t address0=0;
	 uint8_t address1=0;
	 uint8_t command0=0;
	 uint8_t command1=0;
	 uint32_t result=0;
	 LOCAL uint32_t last_result=0;
	switch(decode_status){
		case FIND_START:
			DBG_RMT_CTRL_NEC("STA:FIND_START\n");
			if(nec_decode_find_start_flag(mem_array,len,&arry_pos,RMT_RECV_DURATION_OFFSET_TICK_CNT)){
				DBG_RMT_CTRL_NEC("STA:FIND_START_END pos=%d\n",arry_pos);
				current_array_pos=mem_array+arry_pos+1;
		  decode_status=FIND_ADDRESS0;
			}
		else if(nec_decode_find_repeat_flag(mem_array,len,RMT_RECV_DURATION_OFFSET_TICK_CNT)){
			   DBG_RMT_CTRL_NEC("STA:FIND_REPEAT_END pos=%d\n");
			return last_result;
		 }
		   else{
				DBG_RMT_CTRL_NEC("STA:FIND_START_ERR pos=%d\n",arry_pos);
				decode_status=ERR;
		  goto __ERR__;
			}
	   //notice: this break ,shoulde delet
		  //  break;
		case FIND_ADDRESS0:
			DBG_RMT_CTRL_NEC("STA:FIND_ADDRESS0\n");
			if(nec_decode_analyse_data(current_array_pos,&arry_pos,RMT_RECV_DURATION_OFFSET_TICK_CNT,&address0,RMT_RECV_DATA_BIT_LEN*2)){
				DBG_RMT_CTRL_NEC("STA:FIND_ADDRESS0_END pos=%d comand0=0x%x\n",arry_pos,address0);
				current_array_pos=current_array_pos+(arry_pos+1);
		 // DBG_RMT_CTRL_NEC("absolte pos=%d\n",current_array_pos-mem_array);
				decode_status=FIND_ADDRESS1;
			}
			else{
				DBG_RMT_CTRL_NEC("STA:FIND_ADDRESS0_ERR pos=%d\n",arry_pos);
				decode_status=ERR;
		   goto __ERR__;
			}
	   //notice: this break ,shoulde delet
		 //   break;
		case FIND_ADDRESS1:
			DBG_RMT_CTRL_NEC("STA:FIND_ADDRESS1\n");
			if(nec_decode_analyse_data(current_array_pos,&arry_pos,RMT_RECV_DURATION_OFFSET_TICK_CNT,&address1,RMT_RECV_DATA_BIT_LEN*2)){
				DBG_RMT_CTRL_NEC("STA:FIND_ADDRESS1_END pos=%d comand0=0x%x\n",arry_pos,address1);
				current_array_pos=current_array_pos+(arry_pos+1);
		  //DBG_RMT_CTRL_NEC("absolte pos=%d\n",current_array_pos-mem_array);
				decode_status=FIND_COMMAND0;
			}
			else{
				DBG_RMT_CTRL_NEC("STA:FIND_ADDRESS1_ERR pos=%d\n",arry_pos);
				decode_status=ERR;
		   goto __ERR__;
			}
	   //notice: this break ,shoulde delet
		 //   break;
		case FIND_COMMAND0:
			DBG_RMT_CTRL_NEC("STA:FIND_COMMAND0\n");
			if(nec_decode_analyse_data(current_array_pos,&arry_pos,RMT_RECV_DURATION_OFFSET_TICK_CNT,&command0,RMT_RECV_DATA_BIT_LEN*2)){
				DBG_RMT_CTRL_NEC("STA:FIND_COMMAND0_END pos=%d comand0=0x%x\n",arry_pos,command0);
				current_array_pos+=(arry_pos+1);
		 // DBG_RMT_CTRL_NEC("absolte pos=%d\n",current_array_pos-mem_array);
				decode_status=FIND_COMMAND0;
		 
			}
			else{
				DBG_RMT_CTRL_NEC("STA:FIND_COMMAND0_ERR pos=%d\n",arry_pos);
				decode_status=ERR;
		   goto __ERR__;
			}
		  //notice: this break ,shoulde delet
		 //   break;
		case FIND_COMMAND1:
			DBG_RMT_CTRL_NEC("STA:FIND_COMMAND1\n");
			if(nec_decode_analyse_data(current_array_pos,&arry_pos,RMT_RECV_DURATION_OFFSET_TICK_CNT,&command1,RMT_RECV_DATA_BIT_LEN*2)){
				DBG_RMT_CTRL_NEC("STA:FIND_COMMAND1_END pos=%d comand0=0x%x\n",arry_pos,command1);
				current_array_pos+=(arry_pos+1);
				  //DBG_RMT_CTRL_NEC("absolte pos=%d\n",current_array_pos-mem_array);
				decode_status=FIND_COMMAND1;
		 
			}
			else{
				DBG_RMT_CTRL_NEC("STA:FIND_COMMAND1_ERR pos=%d\n",arry_pos);
				decode_status=ERR;
		  goto __ERR__;
			}
		  //notice: this break ,shoulde exit for break default
		  break;

		default:
			DBG_RMT_CTRL_NEC("COME IN default,STA:ERR!!!\n");
			break;
	}
  
	  result = ((command1<<24)|(command0<<16)|(address1<<8)|address0);
	  last_result=result;
	  return result;
	  __ERR__:
		  DBG_RMT_CTRL_NEC("STA:ERR\n");
		  return 0x00;
}
void nec_memry_tran_half_word(uint16_t* des_mem,uint32_t* sour_mem,uint16_t sour_mem_cnt)
{
    uint16_t i=0;
	for(i=0;i<sour_mem_cnt;i++){
        des_mem[i*2]=sour_mem[i]&0x7FFF;
		des_mem[i*2+1]=(sour_mem[i])>>16&0x7FFF;
	}
}
/*deal the receive data*/
LOCAL  uint32_t nec_rx_decode(enum rmt_channel chan_num)
{ 
	
	uint32_t result=0;
    uint32_t mem_array[64]={0};
	if(NULL==mem_array){
		ets_printf("mem_arry null\n");
		return 0;
	}
	//read out channel receive data and store in mem_array
	// there code will copy memory of channels to user buffer,untail the memoy not is zero. 
	uint16_t arry_pos=0;
	uint16_t ram_content_len=0;
	ram_content_len=rmt_ctrl_rx_rd_memory(chan_num,mem_array);
	DBG_RMT_CTRL_NEC("ram content len=%d Byte\n",ram_content_len*2);
	
   //Filter ,if have only one date,in interrupt time,it is	clutter
	if(ram_content_len<=1){
		return 0;
	}

    
    uint16_t mem_array_half_word[128]={0};
    nec_memry_tran_half_word(mem_array_half_word,mem_array,64);
	

#if DBG_RMT_CTRL_ENABLE
	nec_show_buffer2(mem_array,128);
	nec_show_buffer(mem_array_half_word,RMT_EACH_CHANNEL_RAM_SIZE/sizeof(uint16_t));
#endif

	//the status machine of decod
	result=nec_rx_decode_status_machine(mem_array_half_word,RMT_EACH_CHANNEL_RAM_SIZE/sizeof(uint16_t));
    ets_printf("nec_decode 0x%08x\n",result);

	return result;
	 
 }

void nec_rx_init()
{
    rmt_ctrl_channel_config rmt_ctrl_config;
	rmt_ctrl_config.channel=RMT_CTRL_CHANNEL1;
	rmt_ctrl_config.gpio_num=16;
	rmt_ctrl_config.intr_enable=1;
	rmt_ctrl_config.channel_mem_cnt=1;
	rmt_ctrl_config.rmt_mode=RMT_RX_MODE;
	rmt_ctrl_config.mode_config.rx_config.filter_tick_cnt=RMT_RECV_FILTER_THRESHOLD_TICK_CNT;
	rmt_ctrl_config.mode_config.rx_config.rx_threshold_tick_cnt=RMT_RECV_INTERRUPT_TICK_CNT;
	rmt_ctrl_init(&rmt_ctrl_config);
    rmt_ctrl_intr_register(rmt_ctrl_intr,NULL);
	ETS_RMT_CTRL_ENABLE();
	nec_check_macro();
	rmt_ctrl_rx_start(1);
	//nec_tx_dbg_memory(1);
}
void nec_rx_test()
{
    nec_rx_init();
	//rmt_ctrl_check_reg(1);
	//rmt_ctrl_rx_start(RMT_CTRL_CHANNEL1);
 
}
void nec_test(char* cmd ,int* param)
{
    if(strcmp(cmd,"nec_rx_demo")==0){
        ets_printf("nec_rx_demo ok\n");
	    nec_rx_init();
		
	}
	else if(strcmp(cmd,"nec_tx_demo")==0){
       ets_printf("nec_tx_demo ok\n");
	   nec_tx_test();
	}
	else if(strcmp(cmd,"nec_tx_data")==0){
        ets_printf("nec_tx_data ok\n");
		ets_printf("channel:%u,address:%u data:%u repeat_num:%u\n",param[0],param[1],param[2],param[3]);
		nec_cofnig_tx_data(param[0],param[1],param[2],param[3]);	
		rmt_ctrl_tx_start(param[0]);
	}
}
