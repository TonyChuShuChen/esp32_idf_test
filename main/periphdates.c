#include <stdio.h>

#include "esp_types.h"
#include "rom/ets_sys.h"

#include "soc/rtc_cntl_reg.h"
#include "soc/dport_reg.h"
#include "soc/gpio_sd_reg.h"
#include "soc/efuse_reg.h"
#include "soc/rtc_io_reg.h"

void checkPeriphDates() {
	if (READ_PERI_REG(DPORT_REG_DATE)!=DPORT_DPORT_DATE_VERSION) printf("DPORT_REG_DATE mismatch! hwver %x != incver %x\n", READ_PERI_REG(DPORT_REG_DATE), DPORT_DPORT_DATE_VERSION);
//	if (READ_PERI_REG(EFUSE_DATE)!=EFUSE_EFUSE_DATE_VERSION) printf("EFUSE_DATE mismatch! hwver %x != incver %x\n", READ_PERI_REG(EFUSE_DATE), EFUSE_EFUSE_DATE_VERSION);
	if (READ_PERI_REG(RTC_IO_DATE)!=RTC_IO_RTC_IO_DATE_VERSION) printf("RTC_IO_DATE mismatch! hwver %x != incver %x\n", READ_PERI_REG(RTC_IO_DATE), RTC_IO_RTC_IO_DATE_VERSION);
	if (READ_PERI_REG(GPIO_SIGMADELTA_VERSION_REG)!=SIGMADELTA_GPIO_SD_DATE_VERSION) printf("SIGMADELTA_VERSION mismatch! hwver %x != incver %x\n", READ_PERI_REG(GPIO_SIGMADELTA_VERSION_REG), SIGMADELTA_GPIO_SD_DATE_VERSION);
	if (READ_PERI_REG(RTC_CNTL_DATE)!=RTC_CNTL_RTC_CNTL_DATE_VERSION) printf("RTC_CNTL_DATE mismatch! hwver %x != incver %x\n", READ_PERI_REG(RTC_CNTL_DATE), RTC_CNTL_RTC_CNTL_DATE_VERSION);
//	if (READ_PERI_REG(SLCHOSTDATE)!=SLCHOST_SLCHOST_DATE_VERSION) printf("SLCHOSTDATE mismatch! hwver %x != incver %x\n", READ_PERI_REG(SLCHOSTDATE), SLCHOST_SLCHOST_DATE_VERSION);
//	if (READ_PERI_REG(SLCDATE)!=SLC_SLC_DATE_VERSION) printf("SLCDATE mismatch! hwver %x != incver %x\n", READ_PERI_REG(SLCDATE), SLC_SLC_DATE_VERSION);
}
