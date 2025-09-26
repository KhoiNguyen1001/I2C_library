#include "DS3231.h"
#include "string.h"
#include "stm32f4xx_hal_i2c.h"

/* DS3231 register addresses */
#define DS3231_REG_TIME      0x00  /* seconds, minutes, hours, day, date, month, year */
#define DS3231_REG_ALARM1    0x07  /* 4 bytes */
#define DS3231_REG_ALARM2    0x0B  /* 3 bytes */
#define DS3231_REG_CONTROL   0x0E
#define DS3231_REG_STATUS    0x0F
#define DS3231_REG_AGING     0x10
#define DS3231_REG_TEMP_MSB  0x11  /* MSB */
#define DS3231_REG_TEMP_LSB  0x12  /* MSB + fractional */

/* Control register bit masks */
#define DS3231_CTRL_A1IE     (1 << 0)
#define DS3231_CTRL_A2IE     (1 << 1)
#define DS3231_CTRL_INTCN    (1 << 2)
#define DS3231_CTRL_RS1      (1 << 3)
#define DS3231_CTRL_RS2      (1 << 4)
#define DS3231_CTRL_CONV     (1 << 5)
#define DS3231_CTRL_BBSQW    (1 << 6)
#define DS3231_CTRL_EOSC     (1 << 7)

/* Status register bit masks */
#define DS3231_STAT_A1F      (1 << 0)
#define DS3231_STAT_A2F      (1 << 1)
#define DS3231_STAT_EN32KHZ  (1 << 3)
#define DS3231_STAT_OSF      (1 << 7)

/* Helpers: BCD <-> BIN */
static inline uint8_t ds3231_bcd2bin(uint8_t val);
static inline uint8_t ds3231_bin2bcd(uint8_t val);
static HAL_StatusTypeDef ds3231_read_regs(DS3231_HandleTypeDef *DS3231_Handle, uint8_t reg, uint8_t *buf, uint16_t len);
static HAL_StatusTypeDef ds3231_write_regs(DS3231_HandleTypeDef *DS3231_Handle, uint8_t reg, uint8_t *buf, uint16_t len);

/* Public API, User Configuration */
void DS3231_Init(DS3231_HandleTypeDef *DS3231_Handle, I2C_HandleTypeDef *hi2c, uint8_t DS3231_addr)
{
	DS3231_Handle->hi2c = hi2c;
	DS3231_Handle->DS3231_addr = DS3231_addr;
	DS3231_ClearOscillatorStopFlag(DS3231_Handle);
}

/* Read time/date from DS3231 */
HAL_StatusTypeDef DS3231_GetTimeDate(DS3231_HandleTypeDef *DS3231_Handle, DS3231_TimeDate *td)
{
    uint8_t buf[7];
    HAL_StatusTypeDef st = ds3231_read_regs(DS3231_Handle, DS3231_REG_TIME, buf, sizeof(buf));
    if (st != HAL_OK) return st;

    td->seconds = ds3231_bcd2bin(buf[0] & 0x7F);
    td->minutes = ds3231_bcd2bin(buf[1] & 0x7F);

    /* hours handling: 24h mode assumed */
    td->hours   = ds3231_bcd2bin(buf[2] & 0x3F);
    td->dayOfWeek = ds3231_bcd2bin(buf[3] & 0x07);
    td->day     = ds3231_bcd2bin(buf[4] & 0x3F);
    td->month   = ds3231_bcd2bin(buf[5] & 0x1F);
    td->year    = 2000 + ds3231_bcd2bin(buf[6]); /* DS3231 stores year offset from 2000 */

    return HAL_OK;
}

/* Write time/date to DS3231 */
HAL_StatusTypeDef DS3231_SetTimeDate(DS3231_HandleTypeDef *DS3231_Handle, const DS3231_TimeDate *td)
{
    uint8_t buf[7];
    if (!td) return HAL_ERROR;

    buf[0] = ds3231_bin2bcd((uint8_t)(td->seconds % 60));
    buf[1] = ds3231_bin2bcd((uint8_t)(td->minutes % 60));
    buf[2] = ds3231_bin2bcd((uint8_t)(td->hours % 24));    /* 24-hour mode */
    buf[3] = ds3231_bin2bcd((uint8_t)((td->dayOfWeek >= 1 && td->dayOfWeek <=7) ? td->dayOfWeek : 1));
    buf[4] = ds3231_bin2bcd((uint8_t)((td->day >= 1 && td->day <= 31) ? td->day : 1));
    buf[5] = ds3231_bin2bcd((uint8_t)((td->month >=1 && td->month <=12) ? td->month : 1));
    buf[6] = ds3231_bin2bcd((uint8_t)((td->year >= 2000) ? (td->year - 2000) : 0));

    return ds3231_write_regs(DS3231_Handle, DS3231_REG_TIME, buf, sizeof(buf));
}

/* Read temperature (returns Celsius in float) */
HAL_StatusTypeDef DS3231_GetTemperature(DS3231_HandleTypeDef *DS3231_Handle, float *temperature)
{
    uint8_t buf[2];
    HAL_StatusTypeDef st = ds3231_read_regs(DS3231_Handle, DS3231_REG_TEMP_MSB, buf, 2);
    if (st != HAL_OK) return st;

    int8_t msb = (int8_t)buf[0]; /* signed integer part */
    /* upper two bits of LSB are fractional part in 0.25°C steps */
    uint8_t fraction = (buf[1] >> 6) & 0x03;
    float frac_val = fraction * 0.25f;
    *temperature = msb + frac_val;
    return HAL_OK;
}

/* Set Alarm1 with mode. If enableIRQ != 0, set INTCN and A1IE in control reg */
HAL_StatusTypeDef DS3231_SetAlarm1(DS3231_HandleTypeDef *DS3231_Handle, const DS3231_TimeDate *td, DS3231_Alarm1Mode mode, uint8_t enableIRQ)
{
    /* Build alarm1 registers based on mode.
       Alarm1 registers: A1M1 (bit7 of seconds), A1M2 (bit7 of minutes), A1M3 (bit7 of hours), A1M4 (bit7 of day/date)
       For day/date bit (bit6 of reg4) choose date (0) or day (1) — we use date mode here.
    */
    uint8_t a1[4];
    memset(a1, 0, 4);

    /* prepare base values in BCD */
    a1[0] = ds3231_bin2bcd(td->seconds & 0x7F);
    a1[1] = ds3231_bin2bcd(td->minutes & 0x7F);
    a1[2] = ds3231_bin2bcd(td->hours & 0x3F);
    a1[3] = ds3231_bin2bcd(td->day & 0x3F); /* day-of-month */

    /* set mask bits depending on mode */
    switch (mode) {
        case DS3231_ALARM1_EVERY_SECOND:
            a1[0] |= 0x80; a1[1] |= 0x80; a1[2] |= 0x80; a1[3] |= 0x80;
            break;
        case DS3231_ALARM1_MATCH_SECONDS:
            a1[1] |= 0x80; a1[2] |= 0x80; a1[3] |= 0x80;
            break;
        case DS3231_ALARM1_MATCH_MIN_SEC:
            a1[2] |= 0x80; a1[3] |= 0x80;
            break;
        case DS3231_ALARM1_MATCH_HOUR_MIN_SEC:
            a1[3] |= 0x80;
            break;
        case DS3231_ALARM1_MATCH_DATE_HOUR_MIN_SEC:
        default:
            /* no mask bits -> full match */
            break;
    }

    HAL_StatusTypeDef st = ds3231_write_regs(DS3231_Handle, DS3231_REG_ALARM1, a1, 4);
    if (st != HAL_OK) return st;

    /* configure control register for interrupts if requested */
    uint8_t ctrl;
    st = ds3231_read_regs(DS3231_Handle, DS3231_REG_CONTROL, &ctrl, 1);
    if (st != HAL_OK) return st;

    if (enableIRQ) {
        ctrl |= DS3231_CTRL_INTCN;   /* switch INT/SQW to interrupt mode */
        ctrl |= DS3231_CTRL_A1IE;    /* enable alarm1 interrupt */
    } else {
        ctrl &= ~DS3231_CTRL_A1IE;
        /* leave INTCN as-is */
    }

    return ds3231_write_regs(DS3231_Handle, DS3231_REG_CONTROL, &ctrl, 1);
}

/* Set Alarm2 similarly (no seconds field) */
HAL_StatusTypeDef DS3231_SetAlarm2(DS3231_HandleTypeDef *DS3231_Handle, const DS3231_TimeDate *td, DS3231_Alarm2Mode mode, uint8_t enableIRQ)
{
    uint8_t a2[3];
    memset(a2, 0, 3);

    a2[0] = ds3231_bin2bcd(td->minutes & 0x7F);
    a2[1] = ds3231_bin2bcd(td->hours & 0x3F);
    a2[2] = ds3231_bin2bcd(td->day & 0x3F); /* date */

    switch (mode) {
        case DS3231_ALARM2_EVERY_MINUTE:
            a2[0] |= 0x80; a2[1] |= 0x80; a2[2] |= 0x80;
            break;
        case DS3231_ALARM2_MATCH_MIN:
            a2[1] |= 0x80; a2[2] |= 0x80;
            break;
        case DS3231_ALARM2_MATCH_HOUR_MIN:
            a2[2] |= 0x80;
            break;
        case DS3231_ALARM2_MATCH_DATE_HOUR_MIN:
        default:
            break;
    }

    HAL_StatusTypeDef st = ds3231_write_regs(DS3231_Handle, DS3231_REG_ALARM2, a2, 3);
    if (st != HAL_OK) return st;

    /* configure control register for interrupts if requested */
    uint8_t ctrl;
    st = ds3231_read_regs(DS3231_Handle, DS3231_REG_CONTROL, &ctrl, 1);
    if (st != HAL_OK) return st;

    if (enableIRQ) {
        ctrl |= DS3231_CTRL_INTCN;
        ctrl |= DS3231_CTRL_A2IE;
    } else {
        ctrl &= ~DS3231_CTRL_A2IE;
    }

    return ds3231_write_regs(DS3231_Handle, DS3231_REG_CONTROL, &ctrl, 1);
}

/* Clear alarm flags (A1F, A2F) */
HAL_StatusTypeDef DS3231_ClearAlarmFlags(DS3231_HandleTypeDef *DS3231_Handle)
{
    uint8_t status;
    HAL_StatusTypeDef st = ds3231_read_regs(DS3231_Handle, DS3231_REG_STATUS, &status, 1);
    if (st != HAL_OK) return st;

    status &= ~(DS3231_STAT_A1F | DS3231_STAT_A2F);
    return ds3231_write_regs(DS3231_Handle, DS3231_REG_STATUS, &status, 1);
}

uint8_t DS3231_IsAlarm1Triggered(DS3231_HandleTypeDef *DS3231_Handle)
{
    uint8_t status;
    if (ds3231_read_regs(DS3231_Handle, DS3231_REG_STATUS, &status, 1) != HAL_OK) return 0;
    return (status & DS3231_STAT_A1F) ? 1 : 0;
}

uint8_t DS3231_IsAlarm2Triggered(DS3231_HandleTypeDef *DS3231_Handle) {
    uint8_t status;
    if (ds3231_read_regs(DS3231_Handle, DS3231_REG_STATUS, &status, 1) != HAL_OK) return 0;
    return (status & DS3231_STAT_A2F) ? 1 : 0;
}

/* Square-wave control: set RS bits and INTCN accordingly */
HAL_StatusTypeDef DS3231_EnableSquareWave(DS3231_HandleTypeDef *DS3231_Handle, DS3231_SQWFreq freq)
{
    uint8_t ctrl;
    HAL_StatusTypeDef st = ds3231_read_regs(DS3231_Handle, DS3231_REG_CONTROL, &ctrl, 1);
    if (st != HAL_OK) return st;

    /* clear RS bits and A1/A2IE? don't change IE bits here */
    ctrl &= ~(DS3231_CTRL_RS1 | DS3231_CTRL_RS2);

    switch (freq) {
        case DS3231_SQW_1HZ:
            /* RS2=0 RS1=0, ensure INTCN=0 to output square wave */
            ctrl &= ~(DS3231_CTRL_INTCN);
            break;
        case DS3231_SQW_1KHZ:
            ctrl |= DS3231_CTRL_RS1;
            ctrl &= ~(DS3231_CTRL_RS2);
            ctrl &= ~(DS3231_CTRL_INTCN);
            break;
        case DS3231_SQW_4KHZ:
            ctrl &= ~(DS3231_CTRL_RS1);
            ctrl |= DS3231_CTRL_RS2;
            ctrl &= ~(DS3231_CTRL_INTCN);
            break;
        case DS3231_SQW_8KHZ:
            ctrl |= (DS3231_CTRL_RS1 | DS3231_CTRL_RS2);
            ctrl &= ~(DS3231_CTRL_INTCN);
            break;
        case DS3231_SQW_OFF:
        default:
            /* turn off square wave; to disable SQW set INTCN=1 (INT mode) if you want interrupts */
            ctrl |= DS3231_CTRL_INTCN;
            break;
    }

    return ds3231_write_regs(DS3231_Handle, DS3231_REG_CONTROL, &ctrl, 1);
}

HAL_StatusTypeDef DS3231_DisableSquareWave(DS3231_HandleTypeDef *DS3231_Handle)
{
    /* disable by setting INTCN=1 and clearing RS bits */
    uint8_t ctrl;
    HAL_StatusTypeDef st = ds3231_read_regs(DS3231_Handle, DS3231_REG_CONTROL, &ctrl, 1);
    if (st != HAL_OK) return st;
    ctrl |= DS3231_CTRL_INTCN;
    ctrl &= ~(DS3231_CTRL_RS1 | DS3231_CTRL_RS2);
    return ds3231_write_regs(DS3231_Handle, DS3231_REG_CONTROL, &ctrl, 1);
}

/* Enable/disable 32kHz output via status register EN32KHZ bit (bit3 of status register) */
HAL_StatusTypeDef DS3231_Enable32kHz(DS3231_HandleTypeDef *DS3231_Handle, uint8_t enable)
{
    uint8_t status;
    HAL_StatusTypeDef st = ds3231_read_regs(DS3231_Handle, DS3231_REG_STATUS, &status, 1);
    if (st != HAL_OK) return st;
    if (enable) status |= DS3231_STAT_EN32KHZ;
    else status &= ~DS3231_STAT_EN32KHZ;
    return ds3231_write_regs(DS3231_Handle, DS3231_REG_STATUS, &status, 1);
}

/* Oscillator stopped flag */
uint8_t DS3231_OscillatorStopped(DS3231_HandleTypeDef *DS3231_Handle)
{
    uint8_t status;
    if (ds3231_read_regs(DS3231_Handle, DS3231_REG_STATUS, &status, 1) != HAL_OK) return 1; /* assume stopped on error */
    return (status & DS3231_STAT_OSF) ? 1 : 0;
}

HAL_StatusTypeDef DS3231_ClearOscillatorStopFlag(DS3231_HandleTypeDef *DS3231_Handle)
{
    uint8_t status;
    HAL_StatusTypeDef st = ds3231_read_regs(DS3231_Handle, DS3231_REG_STATUS, &status, 1);
    if (st != HAL_OK) return st;
    status &= ~DS3231_STAT_OSF;
    return ds3231_write_regs(DS3231_Handle, DS3231_REG_STATUS, &status, 1);
}


/* Helpers: BCD <-> BIN */
static inline uint8_t ds3231_bcd2bin(uint8_t val) {
    return ((val >> 4) * 10) + (val & 0x0F);
}
static inline uint8_t ds3231_bin2bcd(uint8_t val) {
    return (uint8_t)(((val / 10) << 4) | (val % 10));
}


/* Low-level read/write wrappers */
static HAL_StatusTypeDef ds3231_read_regs(DS3231_HandleTypeDef *DS3231_Handle, uint8_t reg, uint8_t *buf, uint16_t len)
{
    if (!DS3231_Handle->hi2c) return HAL_ERROR;
    return HAL_I2C_Mem_Read(DS3231_Handle->hi2c, DS3231_Handle->DS3231_addr, reg, I2C_MEMADD_SIZE_8BIT, buf, len, HAL_MAX_DELAY);
}

static HAL_StatusTypeDef ds3231_write_regs(DS3231_HandleTypeDef *DS3231_Handle, uint8_t reg, uint8_t *buf, uint16_t len)
{
    if (!DS3231_Handle->hi2c) return HAL_ERROR;

    uint8_t buf_check[16], retry = 0;
    if (len > sizeof(buf_check)) return HAL_ERROR;  // safety
    while(retry < 10)
    {
    	HAL_I2C_Mem_Write(DS3231_Handle->hi2c, DS3231_Handle->DS3231_addr, reg, I2C_MEMADD_SIZE_8BIT, buf, len, HAL_MAX_DELAY);
    	HAL_Delay(2); // small delay
    	HAL_I2C_Mem_Read(DS3231_Handle->hi2c, DS3231_Handle->DS3231_addr, reg, I2C_MEMADD_SIZE_8BIT, buf_check, len, HAL_MAX_DELAY);
    	if(memcmp(buf_check,buf,len) == 0) {
    		return HAL_OK;
    	} else retry++;
    }
    return HAL_ERROR;
}
