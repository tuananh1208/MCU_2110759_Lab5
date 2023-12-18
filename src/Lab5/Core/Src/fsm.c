/*
 * fsm.c
 *
 *  Created on: Dec 18, 2023
 *      Author: TuanAnh
 */


#include "fsm.h"

extern ADC_HandleTypeDef hadc1;

extern UART_HandleTypeDef huart2;

int state = INIT;
int ADC_value = 0;

uint8_t buffer_byte;
uint8_t buffer[MAX_BUFFER_SIZE];
uint8_t index_buffer = 0;
uint8_t buffer_flag = 0;

uint8_t cmd_flag = INIT;
uint8_t cmd_data[MAX_CMD_SIZE];
uint8_t cmd_data_index = 0;

int isCmdRST(uint8_t str[]) {
	if (str[0] == 'R' && str[1] == 'S' && str[2] == 'T')
		return 1;
	else
		return 0;
}

int isCmdOK(uint8_t str[]) {
	if (str[0] == 'O' && str[1] == 'K')
		return 1;
	else
		return 0;
}
void printTest() {
	uint8_t text[5] = {'R','S','T'};
	for (int i = 0; i < 3; i++) {
		HAL_UART_Transmit(&huart2, &text[i], 1, 300);
	}
}

void cmd_parser_fsm() {
	switch (state) {
	case INIT:
		if (buffer_byte == '!') {
			state = READING;
			cmd_data_index = 0;
		}
		break;
	case READING:
		if (buffer_byte != '!' && buffer_byte != '#') {
			cmd_data[cmd_data_index] = buffer_byte;
			//HAL_UART_Transmit(&huart2, &buffer_byte, 1, 300);
			cmd_data_index++;
		}
		if (buffer_byte == '#') {

			state = INIT;
			cmd_data_index = 0;
			if (isCmdRST(cmd_data) == 1) {
				cmd_flag = RST;
				setTimer1(3);
			} else if (isCmdOK(cmd_data) == 1) {
				cmd_flag = OK;
			}
		}
		break;
	default:
		break;
	}
}

void uart_comm_fsm() {
	char str[50];
	switch (cmd_flag) {
	case RST:
		if (timer1_flag == 1) {
			ADC_value = HAL_ADC_GetValue(&hadc1);
			HAL_UART_Transmit(&huart2, (void*) str, sprintf(str, "!ADC=%d#\r\n", ADC_value), 100);
			setTimer1(300);
		}
		break;
	case OK:
		ADC_value = -1;
		cmd_flag = INIT;
		break;
	default:
		break;
	}
}

