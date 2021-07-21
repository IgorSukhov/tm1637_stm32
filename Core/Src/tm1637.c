#include "tm1637.h"
#include <string.h>
#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_gpio.h"
#include "main.h"
#include "stm32f1xx.h"

#define CLK_LOW HAL_GPIO_WritePin(CLK_GPIO_Port, CLK_Pin, GPIO_PIN_RESET)
#define CLK_HIGH HAL_GPIO_WritePin(CLK_GPIO_Port, CLK_Pin, GPIO_PIN_SET)

#define DIO_LOW HAL_GPIO_WritePin(DIO_GPIO_Port, DIO_Pin, GPIO_PIN_RESET)
#define DIO_HIGH HAL_GPIO_WritePin(DIO_GPIO_Port, DIO_Pin, GPIO_PIN_SET)
#define DIO_VAL(val) HAL_GPIO_WritePin(DIO_GPIO_Port, DIO_Pin, val)

#define DIO_READ HAL_GPIO_ReadPin(DIO_GPIO_Port, DIO_Pin)

#define DELAY delay(10u)

#define MIN(a, b)  (((a) < (b)) ? (a) : (b))

const uint8_t hex_num[] = { 0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F, 0x77, 0x7C, 0x39, 0x5E, 0x79, 0x71 };

void delay(uint32_t t)
{
	uint32_t i;

	for (i = 0u; i < t; i++) asm("NOP");
}

void tm1637_dio_in()
{
	CLEAR_BIT(DIO_GPIO_Port->CRL, GPIO_CRL_MODE7_0);
	CLEAR_BIT(DIO_GPIO_Port->CRL, GPIO_CRL_MODE7_1);
	CLEAR_BIT(DIO_GPIO_Port->CRL, GPIO_CRL_CNF7_0);
	SET_BIT(DIO_GPIO_Port->CRL, GPIO_CRL_CNF7_1);
}

void tm1637_dio_out()
{
	SET_BIT(DIO_GPIO_Port->CRL, GPIO_CRL_MODE7_0);
	SET_BIT(DIO_GPIO_Port->CRL, GPIO_CRL_MODE7_1);
	SET_BIT(DIO_GPIO_Port->CRL, GPIO_CRL_CNF7_0);
	CLEAR_BIT(DIO_GPIO_Port->CRL, GPIO_CRL_CNF7_1);
}

void tm1637_start()
{
	tm1637_dio_out();
	CLK_HIGH;
	DIO_HIGH;
	DELAY;
	DIO_LOW;
	DELAY;
	CLK_LOW;
	DELAY;
}

void tm1637_ack()
{
	CLK_LOW;
	DIO_LOW;
	tm1637_dio_in();
	DELAY;
	while (DIO_READ);
	DELAY;
	CLK_HIGH;
	while (DIO_READ);
	DELAY;
	DIO_LOW;
	DELAY;
	CLK_LOW;
	tm1637_dio_out();
}

void tm1637_stop()
{
	CLK_HIGH;
	DIO_LOW;
	DELAY;
	DIO_HIGH;
}

void tm1637_write_bytes(uint8_t *data, uint8_t length)
{
	uint32_t t_bit;
	uint32_t t_byte;

	tm1637_start();

	for (t_byte = 0u; t_byte < length; t_byte++)
	{
		for (t_bit = 0u; t_bit < 8u; t_bit++)
		{
			CLK_LOW;
			DIO_VAL((data[t_byte] >> t_bit) & 1u);
			DELAY;
			CLK_HIGH;
			DELAY;
		}

		tm1637_ack();
	}

	tm1637_stop();
}

void tm1637_data_command_setting()
{
	uint8_t data = 0x40u;

	tm1637_write_bytes(&data, 1u);
}

void tm1637_address_command_setting(const uint8_t *data, const uint8_t length, const E_tm1637_address start_position)
{
	uint8_t cmd[6u];
	uint8_t len;

	if (data != NULL)
	{
		cmd[0u] = (0xc0u | start_position); //start_position
		len = MIN(length, sizeof(data));
		memcpy(&cmd[1u], data, len);
		tm1637_write_bytes(cmd, len + 1u);
	}
}

void tm1637_display_time(uint8_t hours, uint8_t minutes, uint8_t colon)
{
	uint8_t data[4];

	data[0] = hex_num[hours / 10u];
	data[1] = hex_num[hours % 10u];
	data[2] = hex_num[minutes / 10u];
	data[3] = hex_num[minutes % 10u];

	if (colon)
	{
		data[1] |= 0x80u;
	}
	tm1637_address_command_setting(data, 4u, TM1637_ADDRESS_0);
}

void tm1637_display_time_from_sec(uint32_t seconds, uint8_t colon)
{
	tm1637_display_time((seconds / 3600u), ((seconds % 3600) / 60), colon);
}

void tm1637_display_control()
{
	uint8_t data = 0x88u;

	tm1637_write_bytes(&data, 1u);
}
