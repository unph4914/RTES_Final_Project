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
 * @file    capture.h
 * @brief   This file contains definition of various functions developed for motor functionality
 * @date    2nd May 2024
 *
 */

#ifndef _CAPTURE_H
#define _CAPTURE_H


#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

extern sem_t sem_camera;

/**
 * @brief Camera service to display the black screen/camera feed on the display
 */
void *camera_service(void *threadp); 				

#endif
