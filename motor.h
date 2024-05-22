/*******************************************************************************
 * Copyright (C) 2024 by Krishna Suhagiya and Unmesh Phaterpekar
 *
 * Redistribution, modification or use of this software in source or binary
 * forms is permitted as long as the files maintain this copyright. Users are
 * permitted to modify this and use it to learn about the field of embedded
 * software. Krishna Suhagiya, Unmesh Phaterpekar and the University of Colorado are not liable for
 * any misuse of this material.
 * ****************************************************************************/

/**
 * @file    motor.h
 * @brief   This file contains declaration of various functions developed for motor functionality
 * @date    1st May 2024
 *
 */

#include <stdio.h>
#include <stdint.h>
#include <wiringPi.h>

extern bool is_obstacle_detected;
extern sem_t sem_motor;

/*
 * @brief Function to setup GPIOs for motor
 */
void setup_gpio();

/*
 * @brief Function to trigger the specified motor to move at the specified speed in the specific direction
 */
void control_motor(int motor, int speed, int direction);

/*
 * @brief Motor service to move the motor in the direction based on the gear status/sensor status
 */
void *motor_service(void *threadp);


