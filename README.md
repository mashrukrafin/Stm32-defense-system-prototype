# STM32-Based Defense System Prototype

## Project Overview

A mobile embedded defense-system prototype developed using the STM32F103C8T6 microcontroller. The system was designed to monitor a protected area, detect unauthorized objects, track their movement, and respond when they enter a predefined perimeter.

This project explores real-time embedded control, sensor integration, task synchronization, and automated object tracking.

## Key Features

- Ultrasonic-based distance measurement
- Object detection using IR sensors
- Two-axis servo motor control
- Automated object tracking
- PWM-based actuator control
- Concurrent task execution using FreeRTOS
- Semaphore-based task synchronization

## Hardware Components

- STM32F103C8T6 (Blue Pill)
- Four custom-designed IR sensors
- Ultrasonic sensor
- Two servo motors
- ST-LINK V2

## Software and Technologies

- STM32CubeIDE
- Embedded C
- STM32 HAL
- FreeRTOS
- PWM
- Timer Input Capture
- Binary Semaphore

## Project Development

The project involved coordinating independent sensor-monitoring and servo-control tasks using FreeRTOS. Timer input capture was used for ultrasonic distance measurement, while PWM controlled the servo motors.

## Project Documentation

The project report and selected implementation materials are included in this repository.

## Project Type

Academic group project developed as part of the EEE program at United International University.

## Contributors

See the project report for the complete group member list.
