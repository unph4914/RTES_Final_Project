# Pi Parking System

## Overview

The Pi Parking System is a real-time embedded system developed as part of the ECEN 5623 course on Real-Time Embedded Systems. This project utilizes a Raspberry Pi 4+ as the central processing unit and integrates hardware components like a Logitech camera, motorized wheels, and an ultrasonic sensor to create a responsive, autonomous parking system for small robotic vehicles.

## Features

- **Dual Mode Operation**: The system operates in two main modes: forward and reverse.
- **Real-Time Obstacle Detection**: Utilizes an ultrasonic sensor to detect obstacles in real-time, stopping the vehicle to prevent collisions.
- **Camera Integration**: Provides real-time video feed in reverse mode to aid in navigation.
- **Motor Control**: Bidirectional control of the vehicle's wheels for smooth transitions between movements.
- **User Interface**: Displays real-time data and system status on a video display screen.

## Hardware Components

- **Raspberry Pi 4+**: Acts as the central controller.
- **Logitech C270 Webcam**: Provides the video feed.
- **Ultrasonic Sensor (HC-SR04)**: Used for obstacle detection.
- **TB6612FNG Motor Driver**: Controls the direction and speed of the motors.
- **5V DC Motors**: Powers the vehicle's movement.

## Software Design

The system software is designed around a sequencer that controls the execution frequency of the tasks, ensuring real-time performance and responsiveness. Tasks are implemented using POSIX threads with FIFO scheduling to meet real-time requirements.

### Main Components:

- **Sequencer Service**: Manages the timing and execution of all other services.
- **Camera Service**: Handles the camera operations, activating in reverse mode.
- **Motor Service**: Manages the vehicle's motor controls, including direction and speed.
- **Ultrasonic Sensor Service**: Monitors for obstacles and communicates with the motor service to prevent collisions.

## Documentation

For more detailed information on the system design and architecture, refer to the Project Report given in the repository.

## Contributors

- **Unmesh Phaterpekar**
- **Krishna Suhagiya**
