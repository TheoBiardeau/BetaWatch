/**************************************************************
   rv3029c2_reg, from BetaWatch library, is available for ESP32 platform to manage
   our connected watch (adapted from Micro Crystal RV-3029C2 rtc class driver).

   Licensed under University of Poitiers Connected Objects Master's degree by BetaWatch group.
   Author: Henintsoa Andrianarivony.

  NOTE: Currently this driver only supports the bare minimum for read
  and write the RTC and alarms. The extra features provided by this chip
  (trickle charger, eeprom, T° compensation) are unavailable.
 **************************************************************/

#ifndef RV3029C2_REGS_H
#define RV3029C2_REGS_H

/* -------------------- LIBRARIES -------------------- */

#include <time.h> 
#include <stdint.h>
#include <stddef.h>
#include <math.h>
#include <errno.h>

/* ------------------- DECLARATIONS ------------------ */

/* ===== REGISTERS ===== */

/* Register map */
/* control section */
#define RV3029C2_ONOFF_CTRL		        0x00
#define RV3029C2_IRQ_CTRL		          0x01
#define RV3029C2_IRQ_CTRL_AIE		      (1 << 0)
#define RV3029C2_IRQ_FLAGS		        0x02
#define RV3029C2_IRQ_FLAGS_AF		      (1 << 0)
#define RV3029C2_STATUS			          0x03
#define RV3029C2_STATUS_VLOW1		      (1 << 2)
#define RV3029C2_STATUS_VLOW2		      (1 << 3)
#define RV3029C2_STATUS_SR		        (1 << 4)
#define RV3029C2_STATUS_PON		        (1 << 5)
#define RV3029C2_STATUS_EEBUSY		    (1 << 7)
#define RV3029C2_RST_CTRL		          0x04
#define RV3029C2_CONTROL_SECTION_LEN  0x05

/* watch section */
#define RV3029C2_W_SEC			        0x08
#define RV3029C2_W_MINUTES		      0x09
#define RV3029C2_W_HOURS	        	0x0A
#define RV3029C2_REG_HR_12_24		    (1<<6)  /* 24h/12h mode */
#define RV3029C2_REG_HR_PM		      (1<<5)  /* PM/AM bit in 12h mode */
#define RV3029C2_W_DATE			        0x0B
#define RV3029C2_W_DAYS			        0x0C
#define RV3029C2_W_MONTHS		        0x0D
#define RV3029C2_W_YEARS		        0x0E
#define RV3029C2_WATCH_SECTION_LEN	0x07

/* alarm section */
#define RV3029C2_A_SC			          0x10
#define RV3029C2_A_MN			          0x11
#define RV3029C2_A_HR			          0x12
#define RV3029C2_A_DT			          0x13
#define RV3029C2_A_DW			          0x14
#define RV3029C2_A_MO			          0x15
#define RV3029C2_A_YR			          0x16
#define RV3029C2_ALARM_SECTION_LEN	0x07

/* timer section */
#define RV3029C2_TIMER_LOW		0x18
#define RV3029C2_TIMER_HIGH		0x19

/* temperature section */
#define RV3029C2_TEMP_PAGE		0x20

/* eeprom data section */
#define RV3029C2_E2P_EEDATA1		0x28
#define RV3029C2_E2P_EEDATA2		0x29

/* eeprom control section */
#define RV3029C2_CONTROL_E2P_EECTRL	    0x30
#define RV3029C2_TRICKLE_1K		          (1<<0)  /*  1K resistance */
#define RV3029C2_TRICKLE_5K		          (1<<1)  /*  5K resistance */
#define RV3029C2_TRICKLE_20K		        (1<<2)  /* 20K resistance */
#define RV3029C2_TRICKLE_80K		        (1<<3)  /* 80K resistance */
#define RV3029C2_CONTROL_E2P_XTALOFFSET	0x31
#define RV3029C2_CONTROL_E2P_QCOEF	    0x32
#define RV3029C2_CONTROL_E2P_TURNOVER	  0x33

/* user ram section */
#define RV3029C2_USR1_RAM_PAGE		0x38
#define RV3029C2_USR1_SECTION_LEN	0x04
#define RV3029C2_USR2_RAM_PAGE		0x3C
#define RV3029C2_USR2_SECTION_LEN	0x04


/* ===== STRUCTURES ===== */

struct rv3029_data {
	struct device		*dev;
	struct rtc_device	*rtc;
	struct regmap		*regmap;
	int irq;
};

#ifndef MEMS_SHARED_TYPES
#define MEMS_SHARED_TYPES

/** @addtogroup  Interfaces_Functions
  * @brief       This section provide a set of functions used to read and
  *              write a generic register of the device.
  *              MANDATORY: return 0 -> no Error.
  * @{
  *
  */
typedef int32_t (*stmdev_write_ptr)(void *, uint8_t, const uint8_t *, uint16_t);
typedef int32_t (*stmdev_read_ptr)(void *, uint8_t, uint8_t *, uint16_t);

typedef struct {
  //Component mandatory fields
  stmdev_write_ptr  write_reg;
  stmdev_read_ptr   read_reg;
  //Customizable optional pointer
  void *handle;
} stmdev_ctx_t;

#endif // MEMS SHARED TYPES


struct rtc_wkalrm {
        unsigned char enabled;        /* 0 = alarm disabled, 1 = alarm enabled */
        unsigned char pending;  /* 0 = alarm not pending, 1 = alarm pending */
        struct tm time;        /* time the alarm is set to */
};

/* ===== FUNCTIONS ===== */

// - Conversion BCD(Binary Coded Decimal)/BIN(Binary)
#define bcd2bin(x)	(((x) & 0x0f) + ((x) >> 4) * 10)
#define bin2bcd(x)	((((x) / 10) << 4) + (x) % 10)

// - I2C functions to read/write regs of the RTC  
int32_t rv3029c2_read_reg(stmdev_ctx_t *ctx, uint8_t reg, uint8_t *data, uint16_t len);
int32_t rv3029c2_write_reg(stmdev_ctx_t *ctx, uint8_t reg, uint8_t *data, uint16_t len);

// - Functions to get/set the RTC status reg
static int32_t rv3029c2_i2c_get_sr(stmdev_ctx_t *ctx, uint8_t *val);
static int32_t rv3029c2_i2c_set_sr(stmdev_ctx_t *ctx, uint8_t val);

// - Functions to get/set time regs of the RTC
static int rv3029c2_i2c_read_time(stmdev_ctx_t *ctx, struct tm *tm);
int rv3029c2_time_get(stmdev_ctx_t *ctx, struct tm *tm);
static int rv3029c2_i2c_set_time(stmdev_ctx_t *ctx, struct tm const *tm);
int rv3029c2_time_set(stmdev_ctx_t *ctx, struct tm *tm);

// - Functions to get/set alarm regs of the RTC (+ set alarm for IRQ)
static int rv3029c2_i2c_read_alarm(stmdev_ctx_t *ctx, struct rtc_wkalrm *alarm);
int rv3029c2_read_alarm(stmdev_ctx_t *ctx, struct rtc_wkalrm *alarm);
static int rv3029c2_rtc_i2c_set_alarm(stmdev_ctx_t *ctx, struct rtc_wkalrm *alarm);
int rv3029c2_set_alarm(stmdev_ctx_t *ctx, struct rtc_wkalrm *alarm);
int rv3029c2_alarm_set_irq(stmdev_ctx_t *ctx, int enable);

#endif