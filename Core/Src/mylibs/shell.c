/*
 * shell.c
 *
 *  Created on: Oct 1, 2023
 *      Author: nicolas
 */
#include "usart.h"
#include "mylibs/shell.h"
#include <stdio.h>
#include <string.h>

uint8_t prompt[]="pasfredo@Nucleo-STM32G474RET6>>";
uint8_t started[]=
		"\r\n*-----------------------------*"
		"\r\n| Welcome on Nucleo-STM32G474 |"
		"\r\n|   M.Pichery and B.Rodier    |"
		"\r\n*-----------------------------*"
		"\r\n";
uint8_t help_prompt[]=	"\r\n			Help all command			"
						"\r\n										"
						"\r\n- start								"
						"\r\n	Turns ON the engine power stage		"
						"\r\n										"
						"\r\n- stop									"
						"\r\n	Turns OFF the engine power stage	"
						"\r\n										"
						"\r\n- speed XXXX							"
						"\r\n	Set the motor speed					"
						"\r\n	100 to full speed clockwise			"
						"\r\n	-100 to full speed counterclockwise "
						"\r\n										";
uint8_t newline[]="\r\n";
uint8_t backspace[]="\b \b";
uint8_t cmdNotFound[]="Command not found\r\n";
uint8_t brian[]="Brian is in the kitchen\r\n";
uint8_t uartRxReceived;
uint8_t uartRxBuffer[UART_RX_BUFFER_SIZE];
uint8_t uartTxBuffer[UART_TX_BUFFER_SIZE];

char	 	cmdBuffer[CMD_BUFFER_SIZE];
int 		idx_cmd;
char* 		argv[MAX_ARGS];
int		 	argc = 0;
char*		token;
int 		newCmdReady = 0;

void Shell_Init(void){
	memset(argv, NULL, MAX_ARGS*sizeof(char*));
	memset(cmdBuffer, NULL, CMD_BUFFER_SIZE*sizeof(char));
	memset(uartRxBuffer, NULL, UART_RX_BUFFER_SIZE*sizeof(char));
	memset(uartTxBuffer, NULL, UART_TX_BUFFER_SIZE*sizeof(char));

	HAL_UART_Receive_IT(&huart2, uartRxBuffer, UART_RX_BUFFER_SIZE);
	HAL_UART_Transmit(&huart2, started, strlen((char *)started), HAL_MAX_DELAY);
	HAL_UART_Transmit(&huart2, prompt, strlen((char *)prompt), HAL_MAX_DELAY);
}

void Shell_Loop(void){
	if(uartRxReceived){
		switch(uartRxBuffer[0]){
		case ASCII_CR: // Nouvelle ligne, instruction à traiter
			HAL_UART_Transmit(&huart2, newline, sizeof(newline), HAL_MAX_DELAY);
			cmdBuffer[idx_cmd] = '\0';
			argc = 0;
			token = strtok(cmdBuffer, " ");
			while(token!=NULL){
				argv[argc++] = token;
				token = strtok(NULL, " ");
			}
			idx_cmd = 0;
			newCmdReady = 1;
			break;
		case ASCII_BACK: // Suppression du dernier caractère
			cmdBuffer[idx_cmd--] = '\0';
			HAL_UART_Transmit(&huart2, backspace, sizeof(backspace), HAL_MAX_DELAY);
			break;

		default: // Nouveau caractère
			cmdBuffer[idx_cmd++] = uartRxBuffer[0];
			HAL_UART_Transmit(&huart2, uartRxBuffer, UART_RX_BUFFER_SIZE, HAL_MAX_DELAY);
		}
		uartRxReceived = 0;
	}

	if(newCmdReady){
		if(strcmp(argv[0],"WhereisBrian?")==0){
			HAL_UART_Transmit(&huart2, brian, sizeof(brian), HAL_MAX_DELAY);
		}

		else if(strcmp(argv[0],"help")==0){
			HAL_UART_Transmit(&huart2, help_prompt, strlen((char *)help_prompt), HAL_MAX_DELAY);
		}

		else if(strcmp(argv[0],"start")==0){
			int result = start_PWM();
			if(result == SUCCESS)
			{
				int uartTxStringLength = snprintf((char *)uartTxBuffer, UART_TX_BUFFER_SIZE, "\r\npower ON \r\nSpeed set to 0");
				HAL_UART_Transmit(&huart2, uartTxBuffer, uartTxStringLength, HAL_MAX_DELAY);
			}
			if(result == ERROR)
			{
				int uartTxStringLength = snprintf((char *)uartTxBuffer, UART_TX_BUFFER_SIZE, "\r\nFailed to power ON");
				HAL_UART_Transmit(&huart2, uartTxBuffer, uartTxStringLength, HAL_MAX_DELAY);
			}
		}

		else if(strcmp(argv[0],"stop")==0){
			int result = start_PWM();
			if(result == SUCCESS)
			{
				int uartTxStringLength = snprintf((char *)uartTxBuffer, UART_TX_BUFFER_SIZE, "\r\npower OFF");
				HAL_UART_Transmit(&huart2, uartTxBuffer, uartTxStringLength, HAL_MAX_DELAY);
			}
			if(result == ERROR)
			{
				int uartTxStringLength = snprintf((char *)uartTxBuffer, UART_TX_BUFFER_SIZE, "\r\nFailed to power OFF");
				HAL_UART_Transmit(&huart2, uartTxBuffer, uartTxStringLength, HAL_MAX_DELAY);
			}
		}

		else if(strcmp(argv[0],"speed")==0){
			int val = atoi(argv[1]);
			if((-100 <= val) && (val<= 100))
			{
				set_PWM(val);
				int uartTxStringLength = snprintf((char *)uartTxBuffer,UART_TX_BUFFER_SIZE, "\r\nPWM set to %03d", val);
				HAL_UART_Transmit(&huart2, uartTxBuffer, uartTxStringLength, HAL_MAX_DELAY);
			}
			else
			{
				int uartTxStringLength = snprintf((char *)uartTxBuffer, UART_TX_BUFFER_SIZE, "\r\nFailed to set PWM");
				HAL_UART_Transmit(&huart2, uartTxBuffer, uartTxStringLength, HAL_MAX_DELAY);
			}
		}

		else{
			HAL_UART_Transmit(&huart2, cmdNotFound, sizeof(cmdNotFound), HAL_MAX_DELAY);
		}
		HAL_UART_Transmit(&huart2, prompt, sizeof(prompt), HAL_MAX_DELAY);
		newCmdReady = 0;
	}
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef * huart){
	uartRxReceived = 1;
	HAL_UART_Receive_IT(&huart2, uartRxBuffer, UART_RX_BUFFER_SIZE);
}
