Projet realised by @MaximePich et @LirokWid

# Nucleo-STM32G474 Motor Control Project

This project is designed for controlling a motor using an STM32G474 microcontroller on a Nucleo development board. It includes a simple shell interface for user interaction and motor control. The software provides functionalities for starting/stopping the motor, setting the motor speed, and monitoring the motor's current.

Example of the motor being controlled : https://youtube.com/shorts/cWRz2HWbShM

## Table of Contents

- [Introduction](#nucleo-stm32g474-motor-control-project)
- [Features](#features)
- [Prerequisites](#prerequisites)
- [Getting Started](#getting-started)
- [Usage](#usage)

## Features

- Control the motor speed and direction.
- Monitor the motor's current.
- User-friendly shell interface for interacting with the system.

## Prerequisites

Before getting started, make sure you have the following:

- An STM32G474 Nucleo development board.
- Development environment setup for STM32 (e.g., STM32CubeIDE).
- Necessary libraries and dependencies (provided in the code).
- A motor or motor controller connected to the Nucleo board.

## Getting Started

1. Clone this repository to your local development environment:

   ```bash
   git clone <repository-url>
   
   Open the project in your preferred development environment (e.g., STM32CubeIDE).

2. Build and flash the project to your STM32G474 Nucleo board.

3. Connect your motor or motor controller to the appropriate pins on the Nucleo board.

4. Power up the system and start the shell interface.

## Usage

When you power up the system, it will initialize and display a shell prompt.

You can enter various commands to control the motor:

- help: Display a list of available commands.
- start: Start the motor.
- stop: Stop the motor.
- speed <value>: Set the motor speed, where <value> is between -100 and 100.

Use the shell interface to monitor the motor's current and control its behavior.

