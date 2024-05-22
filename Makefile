INCLUDE_DIRS = -I/usr/include/opencv4
LIB_DIRS = 
CC=g++

CDEFS=
CFLAGS= -O0 -g $(INCLUDE_DIRS) $(CDEFS)
LIBS= -L/usr/lib -lopencv_core -lopencv_flann -lopencv_video -lrt -lwiringPi

HFILES= 
CFILES= main.cpp capture.cpp motor.cpp ultrasonic_sensor.cpp time_stamp.cpp

SRCS= ${HFILES} ${CFILES}
OBJS= ${CFILES:.cpp=.o}

all:	main

clean:
	-rm -f *.o *.d
	-rm -f main

main: main.o capture.o motor.o ultrasonic_sensor.o time_stamp.o
	$(CC) $(LDFLAGS) $(CFLAGS) -o $@ main.o capture.o motor.o ultrasonic_sensor.o time_stamp.o `pkg-config --libs opencv4` $(LIBS)

depend:

.cpp.o: $(SRCS)
	$(CC) $(CFLAGS) -c $<
