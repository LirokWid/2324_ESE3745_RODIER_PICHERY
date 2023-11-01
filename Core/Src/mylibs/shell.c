/**
 * @file shell.c
 *
 * @brief This file contains the implementation of a simple shell for Nucleo-STM32G474.
 *
 * @date October 1, 2023
 * @author Nicolas
 */

#include "usart.h"
#include "mylibs/shell.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Global variables and constants

/**
 * @var prompt
 * @brief Prompt displayed in the shell.
 */
uint8_t prompt[] = "Rodier_Pichery@Nucleo-STM32G474RET6>>";

/**
 * @var started
 * @brief Welcome message displayed at the start of the shell.
 */
uint8_t started[] =
    "\r\n*-----------------------------*"
    "\r\n| Welcome on Nucleo-STM32G474 |"
    "\r\n|   M.Pichery and B.Rodier    |"
    "\r\n*-----------------------------*"
    "\r\n";

/**
 * @var help_prompt
 * @brief Help text with available commands.
 */
uint8_t help_prompt[] =
    "\r\n			Help all command			"
    "\r\n										"
    "\r\n- start								"
    "\r\n	Turns ON the engine power stage		"
    "\r\n										"
    "\r\n- stop									"
    "\r\n	Turns OFF the engine power stage	"
    "\r\n										"
    "\r\n- speed XXXX							"
    "\r\n	Set the motor speed					"
    "\r\n	 100 to full speed Clockwise		"
    "\r\n	-100 to full speed CounterClockwise "
    "\r\n										";

/**
 * @var newline
 * @brief Newline character.
 */
uint8_t newline[] = "\r\n";

/**
 * @var backspace
 * @brief Backspace character used for clearing characters.
 */
uint8_t backspace[] = "\b \b";

/**
 * @var cmdNotFound
 * @brief Message for an unknown command.
 */
uint8_t cmdNotFound[] = "/!\\ Command not found /!\\ \r\n";

/**
 * @var uartRxReceived
 * @brief Flag to indicate UART data reception.
 */
uint8_t uartRxReceived;

/**
 * @var uartRxBuffer
 * @brief Buffer for receiving UART data.
 */
uint8_t uartRxBuffer[UART_RX_BUFFER_SIZE];

/**
 * @var uartTxBuffer
 * @brief Buffer for transmitting UART data.
 */
uint8_t uartTxBuffer[UART_TX_BUFFER_SIZE];

/**
 * @var motor_set_speed
 * @brief External variable for motor speed.
 */
extern int motor_set_speed;

/**
 * @var cmdBuffer
 * @brief Buffer for storing the received command.
 */
char cmdBuffer[CMD_BUFFER_SIZE];

/**
 * @var idx_cmd
 * @brief Index for the current position in the command buffer.
 */
int idx_cmd;

/**
 * @var argv
 * @brief Array of command arguments.
 */
char* argv[MAX_ARGS];

/**
 * @var argc
 * @brief Number of command arguments.
 */
int argc = 0;

/**
 * @var token
 * @brief Tokenized command argument.
 */
char* token;

/**
 * @var newCmdReady
 * @brief Flag to indicate a new command is ready to be processed.
 */
int newCmdReady = 0;

// Code written by Mr. PAPAZOGLOU

/**
 * @brief Initializes the shell.
 */
void Shell_Init(void) {
    memset(argv, 0, MAX_ARGS * sizeof(char*));
    memset(cmdBuffer, 0, CMD_BUFFER_SIZE * sizeof(char));
    memset(uartRxBuffer, 0, UART_RX_BUFFER_SIZE * sizeof(char));
    memset(uartTxBuffer, 0, UART_TX_BUFFER_SIZE * sizeof(char));

    HAL_UART_Receive_IT(&huart2, uartRxBuffer, UART_RX_BUFFER_SIZE);
    HAL_UART_Transmit(&huart2, started, strlen((char*)started), HAL_MAX_DELAY);
    HAL_UART_Transmit(&huart2, prompt, strlen((char*)prompt), HAL_MAX_DELAY);
}

/**
 * @brief Main loop of the shell.
 */
void Shell_Loop(void) {
    if (uartRxReceived) {
        switch (uartRxBuffer[0]) {
        case ASCII_CR: // Newline, process the instruction
            HAL_UART_Transmit(&huart2, newline, sizeof(newline), HAL_MAX_DELAY);
            cmdBuffer[idx_cmd] = '\0';
            argc = 0;
            token = strtok(cmdBuffer, " ");
            while (token != NULL) {
                argv[argc++] = token;
                token = strtok(NULL, " ");
            }
            idx_cmd = 0;
            newCmdReady = 1;
            break;
        case ASCII_BACK: // Delete the last character
            cmdBuffer[idx_cmd--] = '\0';
            HAL_UART_Transmit(&huart2, backspace, sizeof(backspace), HAL_MAX_DELAY);
            break;
        default: // New character, append it to the command buffer
            cmdBuffer[idx_cmd++] = uartRxBuffer[0];
            HAL_UART_Transmit(&huart2, uartRxBuffer, UART_RX_BUFFER_SIZE, HAL_MAX_DELAY);
        }
        uartRxReceived = 0;
    }

    // Code written by Baptiste RODIER and Maxime PICHERY
    if (newCmdReady) {
        // CMD "help" -> prompt the help text in the terminal of the user
        if (strcmp(argv[0], "help") == 0) {
            HAL_UART_Transmit(&huart2, help_prompt, strlen((char*)help_prompt), HAL_MAX_DELAY);
        }
        // CMD "start" -> Start the engine power stage
        else if (strcmp(argv[0], "start") == 0) {
            int result = start_PWM();
            if (result == SUCCESS) {
                int uartTxStringLength = snprintf((char*)uartTxBuffer, UART_TX_BUFFER_SIZE, "\r\npower ON \r\nSpeed set to 0");
                HAL_UART_Transmit(&huart2, uartTxBuffer, uartTxStringLength, HAL_MAX_DELAY);
            }
            if (result == ERROR) {
                int uartTxStringLength = snprintf((char*)uartTxBuffer, UART_TX_BUFFER_SIZE, "\r\n/!\\ Failed to power ON /!\\");
                HAL_UART_Transmit(&huart2, uartTxBuffer, uartTxStringLength, HAL_MAX_DELAY);
            }
        }
        // CMD "stop" -> Stop the engine power stage
        else if (strcmp(argv[0], "stop") == 0) {
            int result = stop_PWM();
            if (result == SUCCESS) {
                int uartTxStringLength = snprintf((char*)uartTxBuffer, UART_TX_BUFFER_SIZE, "\r\npower OFF");
                HAL_UART_Transmit(&huart2, uartTxBuffer, uartTxStringLength, HAL_MAX_DELAY);
            }
            if (result == ERROR) {
                int uartTxStringLength = snprintf((char*)uartTxBuffer, UART_TX_BUFFER_SIZE, "\r\n/!\\ Failed to power OFF /!\\");
                HAL_UART_Transmit(&huart2, uartTxBuffer, uartTxStringLength, HAL_MAX_DELAY);
            }
        }
        // CMD "speed" -> Set the motor speed (motor speed is between -100 and 100)
        else if (strcmp(argv[0], "speed") == 0) {
            int val = atoi(argv[1]);
            if ((-100 <= val) && (val <= 100)) {
                motor_set_speed = val;
                int uartTxStringLength = snprintf((char*)uartTxBuffer, UART_TX_BUFFER_SIZE, "\r\nPWM set to %03d", val);
                HAL_UART_Transmit(&huart2, uartTxBuffer, uartTxStringLength, HAL_MAX_DELAY);
            } else {
                int uartTxStringLength = snprintf((char*)uartTxBuffer, UART_TX_BUFFER_SIZE, "\r\nFailed to set PWM");
                HAL_UART_Transmit(&huart2, uartTxBuffer, uartTxStringLength, HAL_MAX_DELAY);
            }
        }
        // NO CMD FOUND
        else {
            HAL_UART_Transmit(&huart2, cmdNotFound, sizeof(cmdNotFound), HAL_MAX_DELAY);
        }

        // Prompt text is transmitted, and the "newCmdReady" flag is reset to 0
        HAL_UART_Transmit(&huart2, prompt, sizeof(prompt), HAL_MAX_DELAY);
        newCmdReady = 0;
    }
}

/**
 * @brief Callback function for UART data reception.
 *
 * @param huart Pointer to the UART handle.
 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef* huart) {
    uartRxReceived = 1;
    HAL_UART_Receive_IT(&huart2, uartRxBuffer, UART_RX_BUFFER_SIZE);
}
