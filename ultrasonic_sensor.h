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
 * @file    ultrasonic_sensor.cpp
 * @brief   This file contains declaration of various functions developed for ultrasonic sensor functionality
 * @date    2nd May 2024
 *
 */

#include <stdio.h>
#include <wiringPi.h>

extern sem_t sem_ultrasonic;

/*
 * @brief Function to setup the ultrasonic sensor
 */
void setup_ultasonic_sensor();

/*
 * @brief The ultrasonic sensor service to detect the obstacles and set the is_obstacle_detected flag
 */
void *ultrasonic_sensor_service(void *threadp);
