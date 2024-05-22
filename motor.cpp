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
 * @file    motor.cpp
 * @brief   This file contains definition of various functions developed for motor functionality
 * @date    1st May 2024
 *
 */

#include <stdio.h>
#include <syslog.h>
#include <wiringPi.h>
#include <semaphore.h>
#include <pthread.h>

#include "motor.h"
#include "time_stamp.h"

sem_t sem_motor;
bool is_forward = true;
bool is_reverse = false;
extern bool abortS2;

// GPIO pin definitions
#define MOTOR_PWM_A 1  // PWM for Motor A (GPIO 18)
#define MOTOR_IN1_A 4  // Direction IN1 for Motor A (GPIO 23)
#define MOTOR_IN2_A 5  // Direction IN2 for Motor A (GPIO 24)
#define MOTOR_PWM_B 23 // PWM for Motor B (GPIO 13)
#define MOTOR_IN1_B 3  // Direction IN1 for Motor B (GPIO 22, WiringPi pin 3)
#define MOTOR_IN2_B 2  // Direction IN2 for Motor B (GPIO 27, WiringPi pin 2)
#define STBY_PIN 6     // Standby pin (GPIO 25)
#define BUTTON_PIN 7   // Button pin (GPIO 4, WiringPi pin 7)

// Initialize GPIO pins
void setup_gpio() {
    wiringPiSetup();
    pinMode(BUTTON_PIN, INPUT);  // Set button pin as input
    pullUpDnControl(BUTTON_PIN, PUD_UP);  // Enable pull-up resistor
    pinMode(MOTOR_PWM_A, PWM_OUTPUT);
    pinMode(MOTOR_IN1_A, OUTPUT);
    pinMode(MOTOR_IN2_A, OUTPUT);
    pinMode(MOTOR_PWM_B, PWM_OUTPUT);
    pinMode(MOTOR_IN1_B, OUTPUT);
    pinMode(MOTOR_IN2_B, OUTPUT);
    pinMode(STBY_PIN, OUTPUT);
    digitalWrite(STBY_PIN, HIGH);  // Take motor driver out of standby mode
}

// Control motor speed and direction
void control_motor(int motor, int speed, int direction) {
    if (motor == 1) {  // Motor A
        pwmWrite(MOTOR_PWM_A, speed);
        digitalWrite(MOTOR_IN1_A, direction ? HIGH : LOW);
        digitalWrite(MOTOR_IN2_A, direction ? LOW : HIGH);
    } else if (motor == 2) {  // Motor B
        pwmWrite(MOTOR_PWM_B, speed);
        digitalWrite(MOTOR_IN1_B, direction ? HIGH : LOW);
        digitalWrite(MOTOR_IN2_B, direction ? LOW : HIGH);
    }
}

void *motor_service(void *threadp)
{
    struct timespec start = {0,0};
    struct timespec stop = {0,0};
    static struct timespec wcet = {0,0};
    struct timespec time_taken = {0,0};
    int button_state = 0;
    unsigned long motor_service_count = 0;
    printf("Motor started\r\n");
		
    while(!abortS2)
    {
        sem_wait(&sem_motor);
	clock_gettime(CLOCK_REALTIME, &start);
        button_state = digitalRead(BUTTON_PIN);  // Read button state
        if (button_state == 1) {  // Button is pressed
		is_forward = !is_forward;  // Toggle forward state
		is_reverse = !is_reverse;  // Toggle reverse state
	}
	if(is_forward | is_reverse)
	{
		if(is_forward)
		{
			// Test Motor A
			control_motor(1, 512, 1);  // Half speed forward
			// Test Motor B
			control_motor(2, 512, 1);  // Half speed forward
		}
		else if(is_reverse)
		{
			control_motor(1, 512, 0);  // Half speed backward
			control_motor(2, 512, 0);  // Half speed backward
		}
	}
	
	if(is_obstacle_detected)
	{
		control_motor(1, 0, 0);    // Stop Motor A
		control_motor(2, 0, 0);    // Stop Motor B
	}
	clock_gettime( CLOCK_REALTIME, &stop);
	delta_t(&stop, &start, &time_taken);
	motor_service_count++;
	if(check_wcet(&time_taken, &wcet))
	{
	    syslog(LOG_INFO, "motor wcet: %lu sec, %lu msec (%lu microsec), ((%lu nanosec))\n\n", wcet.tv_sec, (wcet.tv_nsec / NSEC_PER_MSEC), (wcet.tv_nsec / NSEC_PER_MICROSEC),wcet.tv_nsec);
	}
	syslog(LOG_CRIT, "motor_service_count = %lu , timestamp: %lu sec, %lu msec (%lu microsec), ((%lu nanosec))\n\n", motor_service_count, wcet.tv_sec, (wcet.tv_nsec / NSEC_PER_MSEC), (wcet.tv_nsec / NSEC_PER_MICROSEC),wcet.tv_nsec);
    }
	
        
    delay(500);

    control_motor(1, 0, 0);    // Stop Motor A

    control_motor(2, 0, 0);    // Stop Motor B

    syslog(LOG_INFO, "Motor stopped\r\n");
    
    pthread_exit(NULL);
}
