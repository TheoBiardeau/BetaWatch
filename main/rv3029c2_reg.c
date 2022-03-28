/**************************************************************
   rv3029c2_reg, from BetaWatch library, is available for ESP32 platform to manage
   our connected watch (adapted from Micro Crystal RV-3029C2 rtc class driver).

   Licensed under University of Poitiers Connected Objects Master's degree by BetaWatch group.
   Author: Henintsoa Andrianarivony.

  NOTE: Currently this driver only supports the bare minimum for read
  and write the RTC and alarms. The extra features provided by this chip
  (trickle charger, eeprom, T° compensation) are unavailable.
 **************************************************************/

#include "rv3029c2_reg.h"

/**
  * @brief  Read generic device register
  *
  * @param  ctx   read / write interface definitions(ptr)
  * @param  reg   register to read
  * @param  data  pointer to buffer that store the data read(ptr)
  * @param  len   number of consecutive register to read
  * @retval       interface status (MANDATORY: return 0 -> no Error)
  *
  */
int32_t rv3029c2_read_reg(stmdev_ctx_t *ctx, uint8_t reg, uint8_t *data, uint16_t len)
{
	if ((reg > RV3029C2_USR1_RAM_PAGE + 7) ||
		(reg + len > RV3029C2_USR1_RAM_PAGE + 8))
		return EINVAL;
	
	int32_t ret;
  	ret = ctx->read_reg(ctx->handle, reg, data, len);

  	return ret;
}


/**
  * @brief  Write generic device register
  *
  * @param  ctx   read / write interface definitions(ptr)
  * @param  reg   register to write
  * @param  data  pointer to data to write in register reg(ptr)
  * @param  len   number of consecutive register to write
  * @retval       interface status (MANDATORY: return 0 -> no Error)
  *
  */
int32_t rv3029c2_write_reg(stmdev_ctx_t *ctx, uint8_t reg, uint8_t *data, uint16_t len)
{
	if ((reg > RV3029C2_USR1_RAM_PAGE + 7) ||
		(reg + len > RV3029C2_USR1_RAM_PAGE + 8))
		return EINVAL;

	int32_t ret;
  	ret = ctx->write_reg(ctx->handle, reg, data, len);

  	return ret;
}



// - Functions to get/set the RTC status reg
static int32_t rv3029c2_i2c_get_sr(stmdev_ctx_t *ctx, uint8_t *val)
{
	int ret = rv3029c2_read_reg(ctx, RV3029C2_STATUS, val, 1);

	if (ret < 0)
		return EIO;
	return 0;
}
static int32_t rv3029c2_i2c_set_sr(stmdev_ctx_t *ctx, uint8_t val)
{
	uint8_t buf[1];
	int sr;

	buf[0] = val;
	sr = rv3029c2_write_reg(ctx, RV3029C2_STATUS, buf, 1);

	if (sr < 0)
		return EIO;
	return 0;
}


// - Read time functions

static int rv3029c2_i2c_read_time(stmdev_ctx_t *ctx, struct tm *tm)
{
	uint8_t buf[1];
	int ret;
	uint8_t regs[RV3029C2_WATCH_SECTION_LEN] = { 0, };

	ret = rv3029c2_i2c_get_sr(ctx, buf);
	if (ret < 0) {
		return EIO;
	}

	ret = rv3029c2_read_reg(ctx, RV3029C2_W_SEC , regs,
					RV3029C2_WATCH_SECTION_LEN);
	if (ret < 0) {
		return ret;
	}

	tm->tm_sec = bcd2bin(regs[RV3029C2_W_SEC-RV3029C2_W_SEC]);
	tm->tm_min = bcd2bin(regs[RV3029C2_W_MINUTES-RV3029C2_W_SEC]);

	/* HR field has a more complex interpretation */
	{
		const uint8_t _hr = regs[RV3029C2_W_HOURS-RV3029C2_W_SEC];
		if (_hr & RV3029C2_REG_HR_12_24) {
			/* 12h format */
			tm->tm_hour = bcd2bin(_hr & 0x1f);
			if (_hr & RV3029C2_REG_HR_PM)	/* PM flag set */
				tm->tm_hour += 12;
		} else /* 24h format */
			tm->tm_hour = bcd2bin(_hr & 0x3f);
	}

	tm->tm_mday = bcd2bin(regs[RV3029C2_W_DATE-RV3029C2_W_SEC]);
	tm->tm_mon = bcd2bin(regs[RV3029C2_W_MONTHS-RV3029C2_W_SEC]) - 1;
	tm->tm_year = bcd2bin(regs[RV3029C2_W_YEARS-RV3029C2_W_SEC]) + 100;
	tm->tm_wday = bcd2bin(regs[RV3029C2_W_DAYS-RV3029C2_W_SEC]) - 1;

	return 0;
}

int rv3029c2_time_get(stmdev_ctx_t *ctx, struct tm *tm)
{
	return rv3029c2_i2c_read_time(ctx, tm);
}


// - Read alarm functions

static int rv3029c2_i2c_read_alarm(stmdev_ctx_t *ctx, struct rtc_wkalrm *alarm)
{
	struct tm *const tm = &alarm->time;
	int ret;
	uint8_t regs[8];

	ret = rv3029c2_i2c_get_sr(ctx, regs);
	if (ret < 0) {
		return EIO;
	}

	ret = rv3029c2_read_reg(ctx, RV3029C2_A_SC, regs,
					RV3029C2_ALARM_SECTION_LEN);

	if (ret < 0) {
		return ret;
	}

	tm->tm_sec = bcd2bin(regs[RV3029C2_A_SC-RV3029C2_A_SC] & 0x7f);
	tm->tm_min = bcd2bin(regs[RV3029C2_A_MN-RV3029C2_A_SC] & 0x7f);
	tm->tm_hour = bcd2bin(regs[RV3029C2_A_HR-RV3029C2_A_SC] & 0x3f);
	tm->tm_mday = bcd2bin(regs[RV3029C2_A_DT-RV3029C2_A_SC] & 0x3f);
	tm->tm_mon = bcd2bin(regs[RV3029C2_A_MO-RV3029C2_A_SC] & 0x1f) - 1;
	tm->tm_year = bcd2bin(regs[RV3029C2_A_YR-RV3029C2_A_SC] & 0x7f) + 100;
	tm->tm_wday = bcd2bin(regs[RV3029C2_A_DW-RV3029C2_A_SC] & 0x07) - 1;

	return 0;
}

int rv3029c2_read_alarm(stmdev_ctx_t *ctx, struct rtc_wkalrm *alarm)
{
	return rv3029c2_i2c_read_alarm(ctx, alarm);
}


// Set alarm functions

static int rv3029c2_rtc_i2c_set_alarm(stmdev_ctx_t *ctx, struct rtc_wkalrm *alarm)
{
	struct tm *const tm = &alarm->time;
	int ret;
	uint8_t regs[8];

	/*
	 * The clock has an 8 bit wide bcd-coded register (they never learn)
	 * for the year. tm_year is an offset from 1900 and we are interested
	 * in the 2000-2099 range, so any value less than 100 is invalid.
	*/
	if (tm->tm_year < 100)
		return EINVAL;

	ret = rv3029c2_i2c_get_sr(ctx, regs);
	if (ret < 0) {
		return EIO;
	}
	regs[RV3029C2_A_SC-RV3029C2_A_SC] = bin2bcd(tm->tm_sec & 0x7f);
	regs[RV3029C2_A_MN-RV3029C2_A_SC] = bin2bcd(tm->tm_min & 0x7f);
	regs[RV3029C2_A_HR-RV3029C2_A_SC] = bin2bcd(tm->tm_hour & 0x3f);
	regs[RV3029C2_A_DT-RV3029C2_A_SC] = bin2bcd(tm->tm_mday & 0x3f);
	regs[RV3029C2_A_MO-RV3029C2_A_SC] = bin2bcd((tm->tm_mon & 0x1f) - 1);
	regs[RV3029C2_A_DW-RV3029C2_A_SC] = bin2bcd((tm->tm_wday & 7) - 1);
	regs[RV3029C2_A_YR-RV3029C2_A_SC] = bin2bcd((tm->tm_year & 0x7f) - 100);

	ret = rv3029c2_write_reg(ctx, RV3029C2_A_SC, regs,
					RV3029C2_ALARM_SECTION_LEN);
	if (ret < 0)
		return ret;

	if (alarm->enabled) {
		uint8_t buf[1];

		/* clear AF flag */
		ret = rv3029c2_read_reg(ctx, RV3029C2_IRQ_FLAGS,
						buf, 1);
		if (ret < 0) {
			return ret;
		}
		buf[0] &= ~RV3029C2_IRQ_FLAGS_AF;
		ret = rv3029c2_write_reg(ctx, RV3029C2_IRQ_FLAGS,
						buf, 1);
		if (ret < 0) {
			return ret;
		}
		/* enable AIE irq */
		ret = rv3029c2_alarm_set_irq(ctx, 1);
		if (ret)
			return ret;
	} else {
		/* disable AIE irq */
		ret = rv3029c2_alarm_set_irq(ctx, 0);
		if (ret)
			return ret;
	}

	return 0;
}

int rv3029c2_set_alarm(stmdev_ctx_t *ctx, struct rtc_wkalrm *alarm)
{
	return rv3029c2_rtc_i2c_set_alarm(ctx, alarm);
}

// Set time functions

static int rv3029c2_i2c_set_time(stmdev_ctx_t *ctx, struct tm const *tm)
{
	uint8_t regs[8];
	int ret;

	/*
	 * The clock has an 8 bit wide bcd-coded register (they never learn)
	 * for the year. tm_year is an offset from 1900 and we are interested
	 * in the 2000-2099 range, so any value less than 100 is invalid.
	*/
	if (tm->tm_year < 100)
		return EINVAL;

	regs[RV3029C2_W_SEC-RV3029C2_W_SEC] = bin2bcd(tm->tm_sec);
	regs[RV3029C2_W_MINUTES-RV3029C2_W_SEC] = bin2bcd(tm->tm_min);
	regs[RV3029C2_W_HOURS-RV3029C2_W_SEC] = bin2bcd(tm->tm_hour);
	regs[RV3029C2_W_DATE-RV3029C2_W_SEC] = bin2bcd(tm->tm_mday);
	regs[RV3029C2_W_MONTHS-RV3029C2_W_SEC] = bin2bcd(tm->tm_mon+1);
	regs[RV3029C2_W_DAYS-RV3029C2_W_SEC] = bin2bcd((tm->tm_wday & 7)+1);
	regs[RV3029C2_W_YEARS-RV3029C2_W_SEC] = bin2bcd(tm->tm_year - 100);

	ret = rv3029c2_write_reg(ctx, RV3029C2_W_SEC, regs, RV3029C2_WATCH_SECTION_LEN);
	if (ret < 0)
		return ret;

	ret = rv3029c2_i2c_get_sr(ctx, regs);
	if (ret < 0) {
		return ret;
	}
	/* clear PON bit */
	ret = rv3029c2_i2c_set_sr(ctx, (regs[0] & ~RV3029C2_STATUS_PON));
	if (ret < 0) {
		return ret;
	}

	return 0;
}

int rv3029c2_time_set(stmdev_ctx_t *ctx, struct tm *tm)
{
	return rv3029c2_i2c_set_time(ctx, tm);
}

// - Set IRQ on alarm

int rv3029c2_alarm_set_irq(stmdev_ctx_t *ctx, int enable)
{
	int ret;
	uint8_t buf[1];

	/* enable AIE irq */
	ret = rv3029c2_read_reg(ctx, RV3029C2_IRQ_CTRL,	buf, 1);
	if (ret < 0) {
		return ret;
	}
	if (enable)
		buf[0] |= RV3029C2_IRQ_CTRL_AIE;
	else
		buf[0] &= ~RV3029C2_IRQ_CTRL_AIE;

	ret = rv3029c2_write_reg(ctx, RV3029C2_IRQ_CTRL, buf, 1);
	if (ret < 0) {
		return ret;
	}

	return 0;
}