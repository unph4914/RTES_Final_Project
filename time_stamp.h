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
 * @file    time_stamp.h
 * @brief   This file contains declaration of various functions developed for time profiling
 * @date    3rd May 2024
 *
 */

#include <stdio.h>
#include <stdint.h>
#include <sys/time.h>

#define MSEC_PER_SEC (1000)
#define NSEC_PER_SEC (1000000000)
#define NSEC_PER_MSEC (1000000)
#define NSEC_PER_MICROSEC (1000)

/*
 * @brief Function to measure the difference between the two timestamps
 */
int delta_t(struct timespec *stop, struct timespec *start, struct timespec *delta_t);

/*
 * @brief Function to check if the current time observed is more than WCET
 */
bool check_wcet(struct timespec *time_taken, struct timespec *wcet);
