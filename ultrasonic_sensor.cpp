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
 * @brief   This file contains definition of various functions developed for ultrasonic sensor functionality
 * @date    2nd May 2024
 *
 */

#include <stdio.h>
#include <syslog.h>
#include <wiringPi.h>
#include <semaphore.h>
#include <pthread.h>

#include "motor.h"
#include "time_stamp.h"

// Define GPIO pins for Trigger and Echo pins
#define TRIG 15
#define ECHO 16
#define DISTANCE_THRESHOLD 7

sem_t sem_ultrasonic;
extern bool is_forward;
extern bool is_reverse;
extern bool abortS3;

void setup_ultasonic_sensor() {
    wiringPiSetup();
    pinMode(TRIG, OUTPUT);
    pinMode(ECHO, INPUT);

    // Ensure the trigger pin is low
    digitalWrite(TRIG, LOW);
    delay(30);
}

void *ultrasonic_sensor_service(void *threadp) {
    struct timespec start = {0,0};
    struct timespec stop = {0,0};
    static struct timespec wcet = {0,0};
    struct timespec time_taken = {0,0};
    unsigned long ultrasonic_sensor_service_count = 0;
    printf("Distance Measurement In Progress\n");

    while (!abortS3) {
		sem_wait(&sem_ultrasonic);
		if(is_forward == true)
		{
			clock_gettime(CLOCK_REALTIME, &start);
			struct timeval detection_start, detection_end;
			long travel_time, distance;

			// Triggering the sensor for 10 microseconds
			digitalWrite(TRIG, HIGH);
			delayMicroseconds(10);
			digitalWrite(TRIG, LOW);

			// Wait for the echo start
			while (digitalRead(ECHO) == LOW);

			// Record time of signal return
			gettimeofday(&detection_start, NULL);
			while (digitalRead(ECHO) == HIGH);
			gettimeofday(&detection_end, NULL);

			// Calculate the distance
			travel_time = (detection_end.tv_sec - detection_start.tv_sec) * 1000000L + detection_end.tv_usec - detection_start.tv_usec;
			distance = travel_time / 58;
			if(distance < DISTANCE_THRESHOLD)
			{
				syslog(LOG_INFO, "Distance: %d cm\n", distance);
				is_obstacle_detected = true;
			}
			else
			{
				is_obstacle_detected = false;
			}
		        clock_gettime( CLOCK_REALTIME, &stop);
		        delta_t(&stop, &start, &time_taken);
			ultrasonic_sensor_service_count++;
		        if(check_wcet(&time_taken, &wcet))
		        {
			    syslog(LOG_INFO, "sensor wcet: %lu sec, %lu msec (%lu microsec), ((%lu nanosec))\n\n", wcet.tv_sec, (wcet.tv_nsec / NSEC_PER_MSEC), (wcet.tv_nsec / NSEC_PER_MICROSEC),wcet.tv_nsec);
		        }
			syslog(LOG_CRIT, "ultrasonic_sensor_service_count = %lu , timestamp: %lu sec, %lu msec (%lu microsec), ((%lu nanosec))\n\n", ultrasonic_sensor_service_count, wcet.tv_sec, (wcet.tv_nsec / NSEC_PER_MSEC), (wcet.tv_nsec / NSEC_PER_MICROSEC),wcet.tv_nsec);
		}
    }
    
    syslog(LOG_INFO, "Sensor stopped\n");

    pthread_exit(NULL);
}
