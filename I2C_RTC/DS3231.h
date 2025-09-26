#ifndef DS3231_H
#define DS3231_H

#include <stdint.h>
#include "stm32f4xx_hal.h"   // or your MCU HAL

/* Time/date structure */
typedef struct {
    uint8_t seconds;    /* 0-59 */
    uint8_t minutes;    /* 0-59 */
    uint8_t hours;      /* 0-23 (24-hour mode) */
    uint8_t dayOfWeek;  /* 1-7 */
    uint8_t day;        /* 1-31 */
    uint8_t month;      /* 1-12 */
    uint16_t year;      /* full year e.g., 2025 */
} DS3231_TimeDate;

typedef struct {
	I2C_HandleTypeDef *hi2c;
	uint8_t DS3231_addr;
} DS3231_HandleTypeDef;

/* Alarm1 modes (A1M4,A1M3,A1M2,A1M1 mask bits) */
typedef enum {
    DS3231_ALARM1_EVERY_SECOND = 0,         /* every second */
    DS3231_ALARM1_MATCH_SECONDS,            /* match seconds */
    DS3231_ALARM1_MATCH_MIN_SEC,            /* match minutes + seconds */
    DS3231_ALARM1_MATCH_HOUR_MIN_SEC,       /* match hour + min + sec */
    DS3231_ALARM1_MATCH_DATE_HOUR_MIN_SEC   /* match date + hour + min + sec */
} DS3231_Alarm1Mode;

/* Alarm2 modes (A2M4,A2M3,A2M2 mask bits) */
typedef enum {
    DS3231_ALARM2_EVERY_MINUTE = 0,         /* every minute (when seconds = 0) */
    DS3231_ALARM2_MATCH_MIN,                /* match minutes */
    DS3231_ALARM2_MATCH_HOUR_MIN,           /* match hour + min */
    DS3231_ALARM2_MATCH_DATE_HOUR_MIN       /* match date + hour + min */
} DS3231_Alarm2Mode;

/* Square-wave frequency selectors */
typedef enum {
    DS3231_SQW_OFF = 0,     /* INTCN = 1, SQW disabled */
    DS3231_SQW_1HZ,         /* RS2=0 RS1=0 */
    DS3231_SQW_1KHZ,        /* RS2=0 RS1=1 */
    DS3231_SQW_4KHZ,        /* RS2=1 RS1=0 */
    DS3231_SQW_8KHZ         /* RS2=1 RS1=1 */
} DS3231_SQWFreq;

/* Public API */
void DS3231_Init(DS3231_HandleTypeDef *DS3231_Handle, I2C_HandleTypeDef *hi2c, uint8_t DS3231_addr); /* initialize library with I2C handle */

/* Time/Date */
HAL_StatusTypeDef DS3231_GetTimeDate(DS3231_HandleTypeDef *DS3231_Handle, DS3231_TimeDate *td);
HAL_StatusTypeDef DS3231_SetTimeDate(DS3231_HandleTypeDef *DS3231_Handle, const DS3231_TimeDate *td);

/* Temperature */
HAL_StatusTypeDef DS3231_GetTemperature(DS3231_HandleTypeDef *DS3231_Handle, float *temperature); /* returns Celsius */

/* Alarm1 & Alarm2 */
HAL_StatusTypeDef DS3231_SetAlarm1(DS3231_HandleTypeDef *DS3231_Handle, const DS3231_TimeDate *td, DS3231_Alarm1Mode mode, uint8_t enableIRQ);
/* For Alarm2 only minutes/hour/date fields are used. seconds ignored. */
HAL_StatusTypeDef DS3231_SetAlarm2(DS3231_HandleTypeDef *DS3231_Handle, const DS3231_TimeDate *td, DS3231_Alarm2Mode mode, uint8_t enableIRQ);

HAL_StatusTypeDef DS3231_ClearAlarmFlags(DS3231_HandleTypeDef *DS3231_Handle);            /* clear A1F and A2F flags */
uint8_t DS3231_IsAlarm1Triggered(DS3231_HandleTypeDef *DS3231_Handle);
uint8_t DS3231_IsAlarm2Triggered(DS3231_HandleTypeDef *DS3231_Handle);

/* Control / square wave / oscillator */
HAL_StatusTypeDef DS3231_EnableSquareWave(DS3231_HandleTypeDef *DS3231_Handle, DS3231_SQWFreq freq);
HAL_StatusTypeDef DS3231_DisableSquareWave(DS3231_HandleTypeDef *DS3231_Handle);
HAL_StatusTypeDef DS3231_Enable32kHz(DS3231_HandleTypeDef *DS3231_Handle, uint8_t enable); /* enable/disable 32kHz pin (status register EN32k) */

uint8_t DS3231_OscillatorStopped(DS3231_HandleTypeDef *DS3231_Handle);
HAL_StatusTypeDef DS3231_ClearOscillatorStopFlag(DS3231_HandleTypeDef *DS3231_Handle);

#endif
