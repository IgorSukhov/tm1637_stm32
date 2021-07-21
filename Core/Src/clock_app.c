#include "main.h"
#include "tm1637.h"
#include "usbd_cdc_if.h"
#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_rtc.h"

uint32_t last_cnt = 0u;
uint32_t cnt;

int8_t message_hanler(uint8_t* buf, uint32_t *len)
{
	RTC_TimeTypeDef sTime;

	sTime.Hours = buf[0u];
	sTime.Minutes = buf[1u];
	sTime.Seconds = buf[2u];
	HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN);

	return 0u;
}

void clkapp_init()
{
	RTC_TimeTypeDef sTime;

	CDC_Receive_msg = message_hanler;
	HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
	tm1637_data_command_setting();
	tm1637_display_control();
}

void clkapp_update()
{
	RTC_TimeTypeDef sTime;

	cnt = (RTC->CNTH << 16u) | RTC->CNTL;

	if (cnt != last_cnt)
	{
		last_cnt = cnt;

		HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
		tm1637_display_time(sTime.Hours, sTime.Minutes, (sTime.Seconds & 1u));
	}
}
