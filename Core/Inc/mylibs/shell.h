/**
 * @file shell.h
 *
 * @brief Header file for the implementation of a simple shell for Nucleo-STM32G474.
 *
 * @date October 1, 2023
 * @author Nicolas
 */

#ifndef INC_MYLIBS_SHELL_H_
#define INC_MYLIBS_SHELL_H_

/**
 * @def UART_RX_BUFFER_SIZE
 * @brief Size of the UART receive buffer.
 */
#define UART_RX_BUFFER_SIZE 1

/**
 * @def UART_TX_BUFFER_SIZE
 * @brief Size of the UART transmit buffer.
 */
#define UART_TX_BUFFER_SIZE 64

/**
 * @def CMD_BUFFER_SIZE
 * @brief Size of the command buffer for storing user input.
 */
#define CMD_BUFFER_SIZE 64

/**
 * @def MAX_ARGS
 * @brief Maximum number of command arguments.
 */
#define MAX_ARGS 9

/**
 * @def ASCII_LF
 * @brief ASCII value for Line Feed (LF).
 */
#define ASCII_LF 0x0A

/**
 * @def ASCII_CR
 * @brief ASCII value for Carriage Return (CR).
 */
#define ASCII_CR 0x0D

/**
 * @def ASCII_BACK
 * @brief ASCII value for Backspace (BACK).
 */
#define ASCII_BACK 0x08


/**
 * @brief Initializes the shell.
 */
void Shell_Init(void);

/**
 * @brief Main loop of the shell for processing user commands.
 */
void Shell_Loop(void);

#endif /* INC_MYLIBS_SHELL_H_ */
