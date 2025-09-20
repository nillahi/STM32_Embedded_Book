/*
 * Code Example 24
 * Language: C
 * Chapter: Chapter_06_Clock_Systems_and_Power_Management
 * Line: 7524
 * 
 * This code example is extracted from the STM32 Embedded Systems Programming book.
 * Use this code as a reference for your STM32 projects.
 * 
 * Hardware Requirements:
 * - STM32 Development Board (STM32F4 Discovery recommended)
 * - Basic components as specified in the book
 * 
 * Software Requirements:
 * - STM32CubeIDE
 * - STM32 HAL Library
 * - STM32CubeMX (for configuration)
 * 
 * Usage:
 * 1. Copy this file to your STM32 project
 * 2. Include necessary STM32 HAL headers
 * 3. Configure hardware in STM32CubeMX
 * 4. Build and flash to your development board
 */

#include "main.h"
#include <stdio.h>
#include <time.h>

RTC_HandleTypeDef hrtc;

/**
 * @brief  Initialize RTC with LSE clock source
 * @retval HAL_StatusTypeDef
 */
HAL_StatusTypeDef initialize_rtc(void)
{
    // Enable backup domain access
    HAL_PWR_EnableBkUpAccess();
    
    // Configure LSE
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSE;
    RCC_OscInitStruct.LSEState = RCC_LSE_ON;
    
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
        printf("ERROR: LSE configuration failed\n");
        return HAL_ERROR;
    }
    
    // Select LSE as RTC clock source
    RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};
    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_RTC;
    PeriphClkInitStruct.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
    
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK) {
        printf("ERROR: RTC clock configuration failed\n");
        return HAL_ERROR;
    }
    
    // Enable RTC clock
    __HAL_RCC_RTC_ENABLE();
    
    // Initialize RTC
    hrtc.Instance = RTC;
    hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
    hrtc.Init.AsynchPrediv = 127;  // For 32.768kHz: (127+1)*(255+1) = 32768
    hrtc.Init.SynchPrediv = 255;
    hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
    hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
    hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
    
    return HAL_RTC_Init(&hrtc);
}

/**
 * @brief  Set RTC time and date
 * @param  year, month, day, hour, minute, second
 * @retval HAL_StatusTypeDef
 */
HAL_StatusTypeDef set_rtc_time(uint16_t year, uint8_t month, uint8_t day,
                              uint8_t hour, uint8_t minute, uint8_t second)
{
    RTC_TimeTypeDef sTime = {0};
    RTC_DateTypeDef sDate = {0};
    
    // Set time
    sTime.Hours = hour;
    sTime.Minutes = minute;
    sTime.Seconds = second;
    sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
    sTime.StoreOperation = RTC_STOREOPERATION_RESET;
    
    if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN) != HAL_OK) {
        return HAL_ERROR;
    }
    
    // Set date
    sDate.WeekDay = RTC_WEEKDAY_MONDAY;  // Calculate actual weekday
    sDate.Month = month;
    sDate.Date = day;
    sDate.Year = year - 2000;  // RTC uses 2-digit year
    
    return HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BIN);
}

/**
 * @brief  Configure RTC alarm for periodic wake-up
 * @param  wake_interval_minutes: Wake-up interval in minutes
 * @retval HAL_StatusTypeDef
 */
HAL_StatusTypeDef configure_periodic_wakeup(uint16_t wake_interval_minutes)
{
    RTC_AlarmTypeDef sAlarm = {0};
    
    // Configure Alarm A for periodic wake-up
    sAlarm.AlarmTime.Hours = 0;
    sAlarm.AlarmTime.Minutes = wake_interval_minutes;
    sAlarm.AlarmTime.Seconds = 0;
    sAlarm.AlarmTime.SubSeconds = 0;
    sAlarm.AlarmTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
    sAlarm.AlarmTime.StoreOperation = RTC_STOREOPERATION_RESET;
    
    sAlarm.AlarmMask = RTC_ALARMMASK_DATEWEEKDAY | RTC_ALARMMASK_HOURS;
    sAlarm.AlarmSubSecondMask = RTC_ALARMSUBSECONDMASK_ALL;
    sAlarm.AlarmDateWeekDaySel = RTC_ALARMDATEWEEKDAYSEL_DATE;
    sAlarm.AlarmDateWeekDay = 1;
    sAlarm.Alarm = RTC_ALARM_A;
    
    return HAL_RTC_SetAlarm_IT(&hrtc, &sAlarm, RTC_FORMAT_BIN);
}

/**
 * @brief  RTC alarm callback - handles periodic wake-up
 * @param  hrtc: RTC handle
 * @retval None
 */
void HAL_RTC_AlarmAEventCallback(RTC_HandleTypeDef *hrtc)
{
    printf("RTC Alarm: Periodic wake-up triggered\n");
    
    // Perform scheduled tasks
    // - Read sensors
    // - Process data
    // - Transmit if needed
    
    // Update power statistics
    power_stats.wake_events++;
    
    // Reconfigure alarm for next wake-up
    configure_periodic_wakeup(10);  // Wake up every 10 minutes
}