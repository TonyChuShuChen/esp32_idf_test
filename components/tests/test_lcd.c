/*
 Testbed for multicore FreeRTOS. This is a small app_main that spins up threads, fiddles with queues etc.

*/

#include <stdio.h>
#include "c_types.h"

#include "rom/ets_sys.h"
#include "rom/lldesc.h"
#include "rom/gpio.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"
#include "freertos/xtensa_api.h"

#include "soc/uart_register.h"
#include "soc/dport_reg.h"
#include "soc/io_mux_reg.h"
#include "soc/gpio_sig_map.h"
#include "soc/gpio_reg.h"
#include "soc/i2s_reg.h"

#include <math.h>
#include "unity.h"

#include "periph_i2s.h"


static int qsintab[256]={
0x8000,0x80c9,0x8192,0x825b,0x8324,0x83ee,0x84b7,0x8580,
0x8649,0x8712,0x87db,0x88a4,0x896c,0x8a35,0x8afe,0x8bc6,
0x8c8e,0x8d57,0x8e1f,0x8ee7,0x8fae,0x9076,0x913e,0x9205,
0x92cc,0x9393,0x945a,0x9521,0x95e7,0x96ad,0x9773,0x9839,
0x98fe,0x99c4,0x9a89,0x9b4d,0x9c12,0x9cd6,0x9d9a,0x9e5e,
0x9f21,0x9fe4,0xa0a7,0xa169,0xa22b,0xa2ed,0xa3af,0xa470,
0xa530,0xa5f1,0xa6b1,0xa770,0xa830,0xa8ef,0xa9ad,0xaa6b,
0xab29,0xabe6,0xaca3,0xad5f,0xae1b,0xaed7,0xaf92,0xb04d,
0xb107,0xb1c0,0xb27a,0xb332,0xb3ea,0xb4a2,0xb559,0xb610,
0xb6c6,0xb77c,0xb831,0xb8e5,0xb999,0xba4d,0xbb00,0xbbb2,
0xbc64,0xbd15,0xbdc6,0xbe76,0xbf25,0xbfd4,0xc082,0xc12f,
0xc1dc,0xc288,0xc334,0xc3df,0xc489,0xc533,0xc5dc,0xc684,
0xc72c,0xc7d3,0xc879,0xc91f,0xc9c3,0xca67,0xcb0b,0xcbae,
0xcc4f,0xccf1,0xcd91,0xce31,0xced0,0xcf6e,0xd00b,0xd0a8,
0xd144,0xd1df,0xd279,0xd313,0xd3ac,0xd443,0xd4db,0xd571,
0xd606,0xd69b,0xd72f,0xd7c2,0xd854,0xd8e5,0xd975,0xda05,
0xda93,0xdb21,0xdbae,0xdc3a,0xdcc5,0xdd4f,0xddd9,0xde61,
0xdee9,0xdf6f,0xdff5,0xe07a,0xe0fd,0xe180,0xe202,0xe283,
0xe303,0xe382,0xe400,0xe47d,0xe4fa,0xe575,0xe5ef,0xe668,
0xe6e0,0xe758,0xe7ce,0xe843,0xe8b7,0xe92b,0xe99d,0xea0e,
0xea7e,0xeaed,0xeb5b,0xebc8,0xec34,0xec9f,0xed09,0xed72,
0xedda,0xee41,0xeea7,0xef0b,0xef6f,0xefd1,0xf033,0xf093,
0xf0f2,0xf150,0xf1ad,0xf209,0xf264,0xf2be,0xf316,0xf36e,
0xf3c4,0xf41a,0xf46e,0xf4c1,0xf513,0xf564,0xf5b3,0xf602,
0xf64f,0xf69b,0xf6e6,0xf730,0xf779,0xf7c1,0xf807,0xf84d,
0xf891,0xf8d4,0xf916,0xf956,0xf996,0xf9d4,0xfa11,0xfa4d,
0xfa88,0xfac1,0xfafa,0xfb31,0xfb67,0xfb9c,0xfbd0,0xfc02,
0xfc33,0xfc63,0xfc92,0xfcc0,0xfcec,0xfd17,0xfd42,0xfd6a,
0xfd92,0xfdb8,0xfdde,0xfe01,0xfe24,0xfe46,0xfe66,0xfe85,
0xfea3,0xfec0,0xfedb,0xfef5,0xff0e,0xff26,0xff3c,0xff52,
0xff66,0xff79,0xff8a,0xff9b,0xffaa,0xffb8,0xffc4,0xffd0,
0xffda,0xffe3,0xffeb,0xfff1,0xfff6,0xfffa,0xfffd,0xffff,
};

//Returns value -32K...32K
static const int isin(int i) {
	i=(i&1023);
	if (i>=512) return -isin(i-512);
	if (i>=256) i=(511-i);
	return qsintab[i]-0x8000;
}

static const int icos(int i) {
	return isin(i+256);
}


/*
static int rotGetPixel(int x, int y, int rot) {
	int rx=(x*icos(rot)-y*isin(rot))>>16;
	int ry=(y*icos(rot)+x*isin(rot))>>16;

	if (((rx>>4)+(ry>>4))&1) return -1; else return 0xffff;
}
*/

#define DPORT_I2S0_CLK_EN   (BIT(4))
#define DPORT_I2S0_RST   (BIT(4))
#define DPORT_I2S1_CLK_EN   (BIT(21))
#define DPORT_I2S1_RST   (BIT(21))

static void stupidDelayFn() {
	volatile int i;
	for (i=0; i<(1<<19); i++) ;
}

static void lcdIfaceInit() {
#if USE_I2S1
	SET_PERI_REG_MASK(PERIP_CLK_EN,DPORT_I2S1_CLK_EN);
	CLEAR_PERI_REG_MASK(PERIP_RST_EN,DPORT_I2S1_RST);
#else
	SET_PERI_REG_MASK(PERIP_CLK_EN,DPORT_I2S0_CLK_EN);
	CLEAR_PERI_REG_MASK(PERIP_RST_EN,DPORT_I2S0_RST);
#endif

	//Init pins to i2s functions
	SET_PERI_REG_MASK(GPIO_ENABLE_W1TS, (1<<11)|(1<<3)|(1<<0)|(1<<2)|(1<<5)|(1<<16)|(1<<17)|(1<<18)|(1<<19)|(1<<20));//ENABLE GPIO oe_enable
	
	PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO0_U, 0);
	PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO2_U, 0);
	PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO5_U, 0);
	PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO16_U, 0);
	PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO17_U, 0);
	PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO18_U, 0);
	PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO19_U, 0);
	PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO20_U, 0);
	PIN_FUNC_SELECT(PERIPHS_IO_MUX_SD_CMD_U, 2); //11
	PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO26_U, 0); //RS

#if USE_I2S1
	//Because of... reasons... the 16-bit values for i2s1 appear on d8...d23
	WRITE_PERI_REG(GPIO_FUNC0_OUT_SEL_CFG, (I2S1O_DATA_OUT8_IDX<<GPIO_GPIO_FUNC0_OUT_SEL_S));
	WRITE_PERI_REG(GPIO_FUNC2_OUT_SEL_CFG, (I2S1O_DATA_OUT9_IDX<<GPIO_GPIO_FUNC0_OUT_SEL_S));
	WRITE_PERI_REG(GPIO_FUNC5_OUT_SEL_CFG, (I2S1O_DATA_OUT10_IDX<<GPIO_GPIO_FUNC0_OUT_SEL_S));
	WRITE_PERI_REG(GPIO_FUNC16_OUT_SEL_CFG, (I2S1O_DATA_OUT11_IDX<<GPIO_GPIO_FUNC0_OUT_SEL_S));
	WRITE_PERI_REG(GPIO_FUNC17_OUT_SEL_CFG, (I2S1O_DATA_OUT12_IDX<<GPIO_GPIO_FUNC0_OUT_SEL_S));
	WRITE_PERI_REG(GPIO_FUNC18_OUT_SEL_CFG, (I2S1O_DATA_OUT13_IDX<<GPIO_GPIO_FUNC0_OUT_SEL_S));
	WRITE_PERI_REG(GPIO_FUNC19_OUT_SEL_CFG, (I2S1O_DATA_OUT14_IDX<<GPIO_GPIO_FUNC0_OUT_SEL_S));
	WRITE_PERI_REG(GPIO_FUNC20_OUT_SEL_CFG, (I2S1O_DATA_OUT15_IDX<<GPIO_GPIO_FUNC0_OUT_SEL_S));
	WRITE_PERI_REG(GPIO_FUNC26_OUT_SEL_CFG, (I2S1O_DATA_OUT16_IDX<<GPIO_GPIO_FUNC0_OUT_SEL_S)); //RS
	WRITE_PERI_REG(GPIO_FUNC11_OUT_SEL_CFG, (I2S1O_WS_OUT_IDX<<GPIO_GPIO_FUNC0_OUT_SEL_S));
#else
	WRITE_PERI_REG(GPIO_FUNC0_OUT_SEL_CFG, (148<<GPIO_GPIO_FUNC0_OUT_SEL_S));
	WRITE_PERI_REG(GPIO_FUNC2_OUT_SEL_CFG, (149<<GPIO_GPIO_FUNC0_OUT_SEL_S));
	WRITE_PERI_REG(GPIO_FUNC5_OUT_SEL_CFG, (150<<GPIO_GPIO_FUNC0_OUT_SEL_S));
	WRITE_PERI_REG(GPIO_FUNC16_OUT_SEL_CFG, (151<<GPIO_GPIO_FUNC0_OUT_SEL_S));
	WRITE_PERI_REG(GPIO_FUNC17_OUT_SEL_CFG, (152<<GPIO_GPIO_FUNC0_OUT_SEL_S));
	WRITE_PERI_REG(GPIO_FUNC18_OUT_SEL_CFG, (153<<GPIO_GPIO_FUNC0_OUT_SEL_S));
	WRITE_PERI_REG(GPIO_FUNC19_OUT_SEL_CFG, (154<<GPIO_GPIO_FUNC0_OUT_SEL_S));
	WRITE_PERI_REG(GPIO_FUNC20_OUT_SEL_CFG, (155<<GPIO_GPIO_FUNC0_OUT_SEL_S));
	WRITE_PERI_REG(GPIO_FUNC26_OUT_SEL_CFG, (156<<GPIO_GPIO_FUNC0_OUT_SEL_S)); //RS
	WRITE_PERI_REG(GPIO_FUNC11_OUT_SEL_CFG, (I2S0O_WS_OUT_IDX<<GPIO_GPIO_FUNC0_OUT_SEL_S));
#endif

	//GPIO_SET_GPIO_FUNC11_OUT_INV_SEL(1); //old
	WRITE_PERI_REG(GPIO_FUNC11_OUT_SEL_CFG, READ_PERI_REG(GPIO_FUNC11_OUT_SEL_CFG)|GPIO_GPIO_FUNC11_OUT_INV_SEL);

	//Reset I2S subsystem
	I2S0.CONF.fld.rx_reset=1; I2S0.CONF.fld.tx_reset=1;
	I2S0.CONF.fld.rx_reset=0; I2S0.CONF.fld.tx_reset=0;

	I2S0.CONF2.val=0;
	I2S0.CONF2.fld.lcd_en=1;

	I2S0.SAMPLE_RATE_CONF.fld.rx_bits_mod=16;
	I2S0.SAMPLE_RATE_CONF.fld.tx_bits_mod=16;
	I2S0.SAMPLE_RATE_CONF.fld.rx_bck_div_num=4;
	I2S0.SAMPLE_RATE_CONF.fld.tx_bck_div_num=4;

	I2S0.CLKM_CONF.val=0;
	I2S0.CLKM_CONF.fld.clka_ena=1;
	I2S0.CLKM_CONF.fld.clk_en=1;
	I2S0.CLKM_CONF.fld.clkm_div_a=0;
	I2S0.CLKM_CONF.fld.clkm_div_b=0;
	I2S0.CLKM_CONF.fld.clkm_div_num=8;

	I2S0.FIFO_CONF.val=0;
	I2S0.FIFO_CONF.fld.rx_fifo_mod=1;
	I2S0.FIFO_CONF.fld.tx_fifo_mod=1;
	I2S0.FIFO_CONF.fld.rx_data_num=32;
	I2S0.FIFO_CONF.fld.tx_data_num=32;

	I2S0.CONF1.val=0;
	I2S0.CONF1.fld.tx_stop_en=1;
	I2S0.CONF1.fld.tx_pcm_bypass=1;

	I2S0.CONF_CHAN.val=0;
	I2S0.CONF_CHAN.fld.tx_chan_mod=1;
	I2S0.CONF_CHAN.fld.rx_chan_mod=1;

	//Invert WS to active-low
	I2S0.CONF.fld.tx_right_first=1;
	I2S0.CONF.fld.rx_right_first=1;
	
	I2S0.TIMING.val=0;

}

//RS as GPIO:
//	PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO26_U, FUNC_GPIO26_GPIO26);
//	GPIO_SET_GPIO_FUNC26_OEN_SEL(1);


//I2S FIFO TX register
#define I2S_TX_FIFO (DR_REG_I2S_BASE + 0x0000)

#define PACK32(a, b) ((((a)&0xffff)<<16)|(((b)&0xffff)))


//Send data to the LCD controller
//high byte first, low second
static void lcdData(int val) {
	I2S0.TX_FIFO=PACK32((val>>8)|0x0100, (val&0xff)|0x0100);
}

//Set the index register in the LCD controller
static void lcdCmd(int reg) {
	I2S0.TX_FIFO=PACK32((reg>>8), reg&0xff);
}

//Set a register in the LCD controller
static void lcdSetReg(int reg, int val) {
	lcdCmd(reg);
	lcdData(val);
}


//Use this to send the queued up commands in the FIFO to the LCD.
//FIFO is 64 entries, use this to send them. One lcdSetReg eats up 4 fifo entries, so we have 16 reg settings
//before the fifo is full.
static void lcdFlush() {
	volatile int i;

	//Transmit FIFO
	I2S0.CONF.fld.tx_start=1;

	//Wait till fifo done
	I2S0.INT_CLR.val=0xFFFFFFFF;
	while(!(I2S0.INT_RAW.fld.tx_rempty_int_raw));
	//Wait for last bytes to leave i2s xmit thing
	//ToDo: poll bit in next hw
	for (i=0; i<(1<<8); i++);
	while(!(I2S0.STATE.fld.tx_idle));

	I2S0.CONF.fld.tx_start=0;
	I2S0.CONF.fld.tx_reset=1; I2S0.CONF.fld.tx_fifo_reset=1;
	I2S0.CONF.fld.tx_reset=0; I2S0.CONF.fld.tx_fifo_reset=0;

	//Workaround: make sure hw doesn't forget 1st byte after reset
	for (i=0; i<(1<<8); i++);
	while(I2S0.STATE.fld.tx_fifo_reset_back);
}


//Initialize the LCD using FIFO commands
static void lcdInit() {

	//Reset U/L cntrs by writing 4 times 0
	lcdCmd(0);
	lcdCmd(0);
	lcdCmd(0);
	lcdCmd(0);
	lcdFlush();

	lcdSetReg(0xA4, 1); //CALB
	lcdFlush();
	lcdSetReg(0x60, 0x1d00); //NL / start of 1st gate
	lcdSetReg(0x08, 0x0808); //BP/FP
	lcdSetReg(0x90, 0x0111); //RTNI, DIVI
	lcdFlush();
	lcdSetReg(0x10, 0x0530); //BT, AP
	lcdSetReg(0x11, 0x0243); //VC, DC0, DC1
	lcdFlush();
	lcdSetReg(0x12, 0x01BF); //PSON, PON
	lcdSetReg(0x07, 0x0100); //BASEE=1
	lcdFlush();

}



static void finishDma() {
	//No need to finish if no DMA transfer going on
	if (!I2S0.FIFO_CONF.fld.dscr_en) return;

	//Wait till fifo done
	while(!(I2S0.INT_RAW.fld.tx_rempty_int_raw)) ;
	//Wait for last bytes to leave i2s xmit thing
	//ToDo: poll bit in next hw
//	for (i=0; i<(1<<8); i++);
	while (!(I2S0.STATE.fld.tx_idle)) ;
	
	//Reset I2S for next transfer
	I2S0.CONF.fld.tx_start=0;
	I2S0.OUT_LINK.fld.outlink_start=0;

	
	I2S0.CONF.fld.rx_reset=1; I2S0.CONF.fld.tx_reset=1;
	I2S0.CONF.fld.rx_reset=0; I2S0.CONF.fld.tx_reset=0;

//	for (i=0; i<(1<<8); i++);
	while(I2S0.STATE.fld.tx_fifo_reset_back);
	I2S0.FIFO_CONF.fld.dscr_en=0; //Disable DMA mode
}


static volatile lldesc_t dmaDesc;

//Send a buffer to the LCD using DMA. Not very intelligent, sends only one buffer per time, doesn't
//chain descriptors.
//We should end up here after an lcdFlush, with the I2S peripheral reset and clean but configured for
//FIFO operation.
static void sendBufDma(uint16_t *buf, int len) {
	//Fill DMA descriptor
	dmaDesc.length=len*2;
	dmaDesc.size=len*2;
	dmaDesc.owner=1;
	dmaDesc.sosf=0;
	dmaDesc.buf=(uint8_t *)buf;
	dmaDesc.offset=0; //unused in hw
	dmaDesc.empty=0;
	dmaDesc.eof=1;

	//Reset DMA
	I2S0.LC_CONF.fld.in_rst=1; I2S0.LC_CONF.fld.out_rst=1; I2S0.LC_CONF.fld.ahbm_rst=1; I2S0.LC_CONF.fld.ahbm_fifo_rst=1;
	I2S0.LC_CONF.fld.in_rst=0; I2S0.LC_CONF.fld.out_rst=0; I2S0.LC_CONF.fld.ahbm_rst=0; I2S0.LC_CONF.fld.ahbm_fifo_rst=0;
	
	//Reset I2S FIFO
	I2S0.CONF.fld.tx_reset=1; I2S0.CONF.fld.tx_fifo_reset=1; I2S0.CONF.fld.rx_fifo_reset=1; 
	I2S0.CONF.fld.tx_reset=0; I2S0.CONF.fld.tx_fifo_reset=0; I2S0.CONF.fld.rx_fifo_reset=0; 

	//Set desc addr
	I2S0.OUT_LINK.fld.outlink_addr=((uint32_t)(&dmaDesc))&I2S_I2S_OUTLINK_ADDR;

	I2S0.FIFO_CONF.fld.dscr_en=1;

	//Enable and configure DMA
	I2S0.LC_CONF.val=I2S_I2S_OUT_DATA_BURST_EN | I2S_I2S_CHECK_OWNER | I2S_I2S_OUT_EOF_MODE | I2S_I2S_OUTDSCR_BURST_EN|I2S_I2S_OUT_DATA_BURST_EN;
	

	//Start transmission
	I2S0.OUT_LINK.fld.outlink_start=1;

	I2S0.CONF.fld.tx_start=1;
	//Clear int flags
	I2S0.INT_CLR.val=0xFFFFFFFF;
}

#define DMALEN 512 //Bug: >64 samples b0rks the i2s module for now.

static void tskone(void *pvParameters) {
	uint16_t buf1[DMALEN];
	uint16_t buf2[DMALEN];
	uint16_t *buf;
	int i=0, f=0, x, y, p=0;
	int rcos, rsin;
	int zoom; //4.4 fixed point
	int px=-110;
	int py=-110;
	int cx, cxx, dxx, dxy, cy, cxy, dyx, dyy;

	printf("Running LCD test\n");
	lcdIfaceInit();
	lcdInit();

	stupidDelayFn();
	stupidDelayFn();
	stupidDelayFn();
	stupidDelayFn();

#if 0
	while(1) {
		lcdInit();
		vTaskDelay(10000/portTICK_PERIOD_MS);
	}
#endif

#if 0
	while(1) {
		for (i=0; i<64; i+=2) {
			if (i<7) {
				x=(1<<i)&0xff;
				y=(1<<(i+1))&0xff;
			} else {
				x=0xFF;
				y=0xFF;
			}
			lcdData((x<<8)+y);
		}
		lcdFlush();
	}
#endif


	while(1) {
		lcdInit();		//re-init every frame, to be certain lcd is inited
		rcos=icos(f*8);
		rsin=isin(f*8);
		zoom=((isin(f*16)+0x8000)>>9)+32;
		px=(isin(f*3)>>7)-110;
		py=(icos(f*3)>>7)-110;

		printf("Writing bytes... %d\n", f);
		lcdSetReg(0x20, 0); //H addr
		lcdSetReg(0x21, 0); //V addr1
		lcdCmd(0x22);
		lcdFlush();
		i=0;
		buf=buf1;

		cxx=(zoom*((px)*rcos-(py)*rsin))>>(22-8);
		cxy=(zoom*((py)*rcos+(px)*rsin))>>(22-8);

		dxx=(zoom*((1)*rcos-(0)*rsin))>>(22-8);
		dxy=(zoom*((0)*rcos+(1)*rsin))>>(22-8);
		dyx=(zoom*((0)*rcos-(1)*rsin))>>(22-8);
		dyy=(zoom*((1)*rcos+(0)*rsin))>>(22-8);

		for (y=0; y<220; y++) {
			cx=cxx;
			cy=cxy;
			for (x=0; x<240; x++) {
				cx+=dyx;
				cy+=dyy;
				if ((cx^cy)&0x1000) p=0x1f; else p=0;
				if ((cx^cy)&0x2000) p|=0x3f<<5;
				if ((cx^cy)&0x4000) p|=0x1f<<11;
				buf[i++]=(p&0xff)|0x100;
				buf[i++]=(p>>8)|0x100;
				if (i==DMALEN) {
					finishDma();
					sendBufDma(buf, DMALEN);
					if (buf==buf1) buf=buf2; else buf=buf1;
					i=0;
				}
			}
			cxx+=dxx;
			cxy+=dxy;
		}
		finishDma();
		f++;
	}
}



TEST_CASE("LCD 16-bit mode test", "[i2s]") {
	xTaskCreatePinnedToCore(tskone  , "tskone"  , 2048, NULL, 3, NULL, 0);
	while(1) {
		vTaskDelay(20000/portTICK_PERIOD_MS);
	}
}


