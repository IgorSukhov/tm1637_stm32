/*
 * tm1637.h
 *
 *  Created on: Jul 6, 2021
 *      Author: SuhPC
 */

#ifndef INC_TM1637_H_
#define INC_TM1637_H_

#include <stdint.h>

typedef enum e_tm1637_address
{
	TM1637_ADDRESS_0,
	TM1637_ADDRESS_1,
	TM1637_ADDRESS_2,
	TM1637_ADDRESS_3,
	TM1637_ADDRESS_4,
	TM1637_ADDRESS_5,
}E_tm1637_address;

void display_control(uint8_t command);
void send_data(uint8_t *data, uint8_t len);
void test_cmd();
void test_cmd2();
void test_cmd3();

void set_data();
void set_address();
void control_display();

void tm1637_data_command_setting();
void tm1637_display_control();
void tm1637_address_command_setting(const uint8_t *data, const uint8_t length, const E_tm1637_address start_position);
void tm1637_display_time(uint8_t hours, uint8_t minutes, uint8_t colon);
void tm1637_display_time_from_sec(uint32_t seconds, uint8_t colon);

extern const uint8_t hex_num[];

#endif /* INC_TM1637_H_ */
