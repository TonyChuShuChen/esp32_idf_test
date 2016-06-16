#include "c_types.h"
#include "ets_sys.h"
#include "osapi.h"

#include "soc/dport_reg.h"
#include "soc/io_mux_reg.h"
#include "coreattr.h"
#include "eagle_soc.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"
#include "freertos/portmacro.h"

#include "periphdates.h"
#include "heap_alloc_caps.h"

static void IRAM_ATTR user_start_cpu0(void);
static void IRAM_ATTR call_user_start_cpu1();
static void IRAM_ATTR user_start_cpu1(void);
void Cache_Read_Enable();
extern void ets_setup_syscalls(void);


extern int __cpu1_entry_point;
extern int _bss_start;
extern int _bss_end;
extern int _init_start;
extern int _init_end;
extern int _iram_romjumptable_start;
extern int _iram_romjumptable_end;
extern int _iram_text_start;
extern int _iram_text_end;

/*
We arrive here after the bootloader finished loading the program from flash. The hardware is mostly uninitialized,
flash cache is down and the app CPU is in reset. We do have a stack, so we can do the initialization in C.
*/

void Uart_Init(int no);
void uartAttach();
void ets_set_appcpu_boot_addr(uint32_t ent);
int ets_getAppEntry();

void IRAM_ATTR call_user_start_cpu0() {
	volatile int *p;
	volatile int i;
	//160MHz
//	SET_PERI_REG_MASK(RTC_CPU_PERIOD_CONF, RTC_CNTL_RTC_CPUPERIOD_SEL);

	//Kill wdt
	REG_CLR_BIT(0x3ff4808c, BIT(10)); //RTCCNTL+8C RTC_WDTCONFIG0 RTC_
	REG_CLR_BIT(0x6001f048, BIT(14)); //DR_REG_BB_BASE+48

	//Move exception vectors to IRAM
	asm volatile (\
		"wsr	%0, vecbase\n" \
		::"r"(&_init_start));


//	uartAttach();
//	Uart_Init(0);
//	ets_install_uart_printf();
	os_printf("%s()\n", __FUNCTION__);
	
/*
	//Each MAP line is for a 256K block; the 1st 2 are DROM, the rest is IROM.
	//Remap IROM flash cache to SPI flash 0-512K, IRAM to SPI flah 0-3.5M
    REG_WRITE(PRODPORT_PRO_CACHE_MAP_A, (0 << PRODPORT_PROCACHE_MAP0_S) 
                              |(1 << PRODPORT_PROCACHE_MAP1_S)
                              |(0 << PRODPORT_PROCACHE_MAP2_S)
                              |(1 << PRODPORT_PROCACHE_MAP3_S)
                              |(2 << PRODPORT_PROCACHE_MAP4_S)
                              );
    REG_WRITE(PRODPORT_PRO_CACHE_MAP_B, (3 << PRODPORT_PROCACHE_MAP5_S)
                              |(4 << PRODPORT_PROCACHE_MAP6_S)
                              |(5 << PRODPORT_PROCACHE_MAP7_S)
                              |(6 << PRODPORT_PROCACHE_MAP8_S)
                              |(7 << PRODPORT_PROCACHE_MAP9_S)
                              );
    REG_WRITE(PRODPORT_PRO_CACHE_MAP_C, (8 << PRODPORT_PROCACHE_MAP10_S) 
                              |(9 << PRODPORT_PROCACHE_MAP11_S)
                              |(10 << PRODPORT_PROCACHE_MAP12_S)
                              |(11 << PRODPORT_PROCACHE_MAP13_S)
                              |(12 << PRODPORT_PROCACHE_MAP14_S)
                              );
    REG_WRITE(PRODPORT_PRO_CACHE_MAP_D, (13 << PRODPORT_PROCACHE_MAP15_S));
	Cache_Read_Enable();
*/


	//Make page 0 access raise an exception
	//Also some other unused pages so we can catch weirdness
	//ToDo: this but nicer.
	asm volatile (\
		"movi a4,0x00000000\n" \
		"movi a5,0xf\n" \
		"wdtlb a5,a4\n" \
		"witlb a5,a4\n" \
		"movi a4,0x80000000\n" \
		"wdtlb a5,a4\n" \
		"witlb a5,a4\n" \
		"movi a4,0xa0000000\n" \
		"wdtlb a5,a4\n" \
		"witlb a5,a4\n" \
		"movi a4,0xc0000000\n" \
		"wdtlb a5,a4\n" \
		"witlb a5,a4\n" \
		"movi a4,0xe0000000\n" \
		"wdtlb a5,a4\n" \
		"witlb a5,a4\n" \
		"movi a4,0x20000000\n" \
		"movi a5,0x0\n" \
		"wdtlb a5,a4\n" \
		"witlb a5,a4\n" \
		"movi a4,0x40000000\n" \
		"movi a5,0x2\n" \
		"wdtlb a5,a4\n" \
		"witlb a5,a4\n" \
		"isync\n" \
		:::"a4","a5");



	//Clear bss
	for(p = &_bss_start;  p < &_bss_end; *p++ = 0 );

	//Initialize heap allocator
	heap_alloc_caps_init();

	os_printf("Pro cpu up.\n");
	os_printf("Running app cpu, entry point is %p\n", call_user_start_cpu1);
	ets_delay_us(60000);

	SET_PERI_REG_MASK(APPCPU_CTRL_REG_B, DPORT_APPCPU_CLKGATE_EN);
	SET_PERI_REG_MASK(APPCPU_CTRL_REG_A, DPORT_APPCPU_RESETTING);
	CLEAR_PERI_REG_MASK(APPCPU_CTRL_REG_A, DPORT_APPCPU_RESETTING);


	for (i=0; i<20; i++) ets_delay_us(40000);
	ets_set_appcpu_boot_addr((uint32_t)call_user_start_cpu1);

	ets_delay_us(10000);
	
//	while (ets_getAppEntry()==(int)call_user_start_cpu1) ;
	//Because of Reasons (tm), the pro cpu cannot use the SPI flash while the app cpu is booting.
//	while(((READ_PERI_REG(RTC_STORE7))&BIT(31)) == 0) ; // check APP boot complete flag
	ets_delay_us(50000);
#if ! defined ONLY_PRINT_CPU || ONLY_PRINT_CPU == 1 //make 0 to not switch back to pro cpu and keep outputting app uart data
	//Swap back to pro cpu
//	CLEAR_PERI_REG_MASK(PRODPORT_HOST_INF_SEL, 1<< PRODPORT_PERI_IO_SWAP_S);
#else
	//No swap back
//	SET_PERI_REG_MASK(PRODPORT_HOST_INF_SEL, 1<< PRODPORT_PERI_IO_SWAP_S);
#endif
	ets_delay_us(50000);
	os_printf("\n\nBack to pro cpu.\n");
	//Run start routine.
	user_start_cpu0();
}


extern int xPortGetCoreID();

extern int _init_start;

/*
We arrive here because the pro CPU pulled us from reset. IRAM is in place, cache is still disabled, we can execute C code.
*/
void IRAM_ATTR call_user_start_cpu1() {
	//We need to do this ASAP because otherwise the structure to catch the SYSCALL instruction, which
	//we abuse to do ROM calls, won't work.

	asm volatile (\
		"wsr	%0, vecbase\n" \
		::"r"(&_init_start));

	//Enable SPI flash
//	PIN_FUNC_SELECT(PERIPHS_IO_MUX_SD_DATA3_U, FUNC_SD_DATA3_SPIWP); // swap PIN SDDATA3 from uart1 to spi, because cache need spi
/*
	SET_PERI_REG_MASK(RTC_STORE7, BIT(31));// set APP CPU boot complete falg
	REG_WRITE(APPDPORT_APP_CACHE_MAP_A, (0 << APPDPORT_APPCACHE_MAP0_S) 
                              |(1 << APPDPORT_APPCACHE_MAP1_S)
                              |(0 << APPDPORT_APPCACHE_MAP2_S)
                              |(1 << APPDPORT_APPCACHE_MAP3_S)
                              );
	REG_WRITE(APPDPORT_APP_CACHE_MAP_B, (2 << APPDPORT_APPCACHE_MAP4_S) 
                              |(3 << APPDPORT_APPCACHE_MAP5_S)
                              |(4 << APPDPORT_APPCACHE_MAP6_S)
                              |(5 << APPDPORT_APPCACHE_MAP7_S)
                              );
	REG_WRITE(APPDPORT_APP_CACHE_MAP_C, (6 << APPDPORT_APPCACHE_MAP8_S) 
                              |(7 << APPDPORT_APPCACHE_MAP9_S)
                              |(8 << APPDPORT_APPCACHE_MAP10_S)
                              |(9 << APPDPORT_APPCACHE_MAP11_S)
                              );
	REG_WRITE(APPDPORT_APP_CACHE_MAP_D, (10 << APPDPORT_APPCACHE_MAP12_S));
	Cache_Read_Enable();
*/
	
	os_printf("App cpu up\n");
	
	//Make page 0 access raise an exception
	//Also some other unused pages so we can catch weirdness
	//ToDo: this but nicer.
	asm volatile (\
		"movi a4,0x00000000\n" \
		"movi a5,0xf\n" \
		"wdtlb a5,a4\n" \
		"witlb a5,a4\n" \
		"movi a4,0x80000000\n" \
		"wdtlb a5,a4\n" \
		"witlb a5,a4\n" \
		"movi a4,0xa0000000\n" \
		"wdtlb a5,a4\n" \
		"witlb a5,a4\n" \
		"movi a4,0xc0000000\n" \
		"wdtlb a5,a4\n" \
		"witlb a5,a4\n" \
		"movi a4,0xe0000000\n" \
		"wdtlb a5,a4\n" \
		"witlb a5,a4\n" \
		"movi a4,0x20000000\n" \
		"movi a5,0x0\n" \
		"wdtlb a5,a4\n" \
		"witlb a5,a4\n" \
		"movi a4,0x40000000\n" \
		"movi a5,0x2\n" \
		"wdtlb a5,a4\n" \
		"witlb a5,a4\n" \
		"isync\n" \
		:::"a4","a5");
	
	user_start_cpu1();
}



extern volatile int port_xSchedulerRunning;
extern int xPortStartScheduler();

void user_start_cpu1(void) {
	uartAttach();
	ets_install_uart_printf();

	os_printf("App cpu is running!\n");
	//Wait for the freertos initialization is finished on CPU0
	while (port_xSchedulerRunning == 0) ;
	os_printf("Core0 started initializing FreeRTOS. Jumping to scheduler.\n");
	//Okay, start the scheduler!
	xPortStartScheduler();
}

extern void (*__init_array_start)(void);
extern void (*__init_array_end)(void);

extern void app_main();
static void do_global_ctors(void) {
    void (**p)(void);
    for(p = &__init_array_start; p != &__init_array_end; ++p)
        (*p)();
}


void user_start_cpu0(void) {
	//Double-check peripherial time stamps vs header time stamps
	checkPeriphDates();
	ets_setup_syscalls();
	do_global_ctors();
	app_main();
	vTaskStartScheduler();
}

