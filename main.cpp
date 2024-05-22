/**
 * Sam Siewert, December 2017
 * Modified by Krishna Suhagiya and Unmesh Phaterpekar
 *
 * @file    main.cpp
 * @brief   This file contains the main function and scheduler.
 * @date    30th April 2024
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include <pthread.h>
#include <sched.h>
#include <semaphore.h>
#include <signal.h>

#include <syslog.h>

#include <errno.h>

#include "capture.h"
#include "motor.h"
#include "ultrasonic_sensor.h"

#define USEC_PER_MSEC (1000)
#define NANOSEC_PER_SEC (1000000000)
#define NUM_CPU_CORES (1)

#define NUM_THREADS (3+1)

bool abortS=FALSE, abortS1=FALSE, abortS2=FALSE, abortS3=FALSE;
struct timeval start_time_val;

typedef struct
{
    int threadIdx;
    unsigned long long sequencePeriods;
} threadParams_t;

void print_scheduler(void)
{
   int schedType;

   schedType = sched_getscheduler(getpid());

   switch(schedType)
   {
       case SCHED_FIFO:
           printf("Pthread Policy is SCHED_FIFO\n");
           break;
       case SCHED_OTHER:
           printf("Pthread Policy is SCHED_OTHER\n"); exit(-1);
         break;
       case SCHED_RR:
           printf("Pthread Policy is SCHED_RR\n"); exit(-1);
           break;
       default:
           printf("Pthread Policy is UNKNOWN\n"); exit(-1);
   }
}

void intHandler(int arg)
{
    // Abort the sequencer itself
    abortS=TRUE; abortS1=TRUE; abortS2=TRUE; abortS3=TRUE;
}

void *sequencer(void *threadp)
{
    struct timeval current_time_val;
    struct timespec delay_time = {0,8333333}; // delay for 8.33 msec, 120 Hz
    struct timespec remaining_time;
    double current_time;
    double residual;
    int rc, delay_cnt=0;
    unsigned long long seqCnt=0;
    threadParams_t *threadParams = (threadParams_t *)threadp;

    gettimeofday(&current_time_val, (struct timezone *)0);

    do
    {
        delay_cnt=0; residual=0.0;

        do
        {
            rc=nanosleep(&delay_time, &remaining_time);

            if(rc == EINTR)
            { 
                residual = remaining_time.tv_sec + ((double)remaining_time.tv_nsec / (double)NANOSEC_PER_SEC);

                if(residual > 0.0) printf("residual=%lf, sec=%d, nsec=%d\n", residual, (int)remaining_time.tv_sec, (int)remaining_time.tv_nsec);
 
                delay_cnt++;
            }
            else if(rc < 0)
            {
                perror("Sequencer nanosleep");
                exit(-1);
            }
           
        } while((residual > 0.0) && (delay_cnt < 100));

        seqCnt++;
        gettimeofday(&current_time_val, (struct timezone *)0);
        //syslog(LOG_INFO, "Sequencer cycle %llu @ sec=%d, msec=%d\n", seqCnt, (int)(current_time_val.tv_sec-start_time_val.tv_sec), (int)current_time_val.tv_usec/USEC_PER_MSEC);


        if(delay_cnt > 1) printf("Sequencer looping delay %d\n", delay_cnt);

        // Release each service at a sub-rate of the generic sequencer rate

        // Camera service = RT_MAX-1	@ 15 Hz
        if((seqCnt % 8) == 0) sem_post(&sem_camera);

        // Motor service = RT_MAX-2	@ 8 Hz
        if((seqCnt % 15) == 0) sem_post(&sem_motor);

        // Ultrasonic service = RT_MAX-3	@ 6 Hz
        if((seqCnt % 20) == 0) sem_post(&sem_ultrasonic);

    } while(!abortS);

    sem_post(&sem_camera); sem_post(&sem_motor); sem_post(&sem_ultrasonic);
    abortS1=TRUE; abortS2=TRUE; abortS3=TRUE;

    pthread_exit((void *)0);
}

int i, rc, scope;
cpu_set_t threadcpu;
pthread_t threads[NUM_THREADS];
threadParams_t threadParams[NUM_THREADS];
pthread_attr_t rt_sched_attr[NUM_THREADS];
int rt_max_prio, rt_min_prio;
struct sched_param rt_param[NUM_THREADS];
struct sched_param main_param;
pthread_attr_t main_attr;
pid_t mainpid;
bool is_obstacle_detected;

int main( int argc, char *argv[] ) 
{
    cpu_set_t allcpuset;

    printf("Welcome to Pi Parking System\r\n");
    
    setup_gpio();
    setup_ultasonic_sensor();
    openlog("pi-parking", 0, LOG_USER);
    syslog(LOG_INFO, "starting");
    
    /* Stop program with Ctrl+C */
    struct sigaction act;
    act.sa_handler = intHandler;
    sigaction(SIGINT, &act, NULL);

    CPU_ZERO(&allcpuset);

    for(i=0; i < NUM_CPU_CORES; i++)
        CPU_SET(i, &allcpuset);

    printf("Using CPUS=%d from total available.\r\n", CPU_COUNT(&allcpuset));


    // initialize the sequencer semaphores
    //
    if (sem_init (&sem_camera, 0, 0)) { printf ("Failed to initialize sem_camera\r\n"); exit (-1); }
    if (sem_init (&sem_motor, 0, 0)) { printf ("Failed to initialize sem_motor\r\n"); exit (-1); }
    if (sem_init (&sem_ultrasonic, 0, 0)) { printf ("Failed to initialize sem_ultrasonic\r\n"); exit (-1); }

    mainpid=getpid();

    rt_max_prio = sched_get_priority_max(SCHED_FIFO);
    rt_min_prio = sched_get_priority_min(SCHED_FIFO);

    rc=sched_getparam(mainpid, &main_param);
    main_param.sched_priority=rt_max_prio;
    rc=sched_setscheduler(getpid(), SCHED_FIFO, &main_param);
    if(rc < 0) perror("main_param");
    print_scheduler();

    pthread_attr_getscope(&main_attr, &scope);

    if(scope == PTHREAD_SCOPE_SYSTEM)
      printf("PTHREAD SCOPE SYSTEM\r\n");
    else if (scope == PTHREAD_SCOPE_PROCESS)
      printf("PTHREAD SCOPE PROCESS\r\n");
    else
      printf("PTHREAD SCOPE UNKNOWN\r\n");

    printf("rt_max_prio=%d\r\n", rt_max_prio);
    printf("rt_min_prio=%d\r\n", rt_min_prio);

    for(i=0; i < NUM_THREADS; i++)
    {

      CPU_ZERO(&threadcpu);
      CPU_SET(0, &threadcpu);

      rc=pthread_attr_init(&rt_sched_attr[i]);
      rc=pthread_attr_setinheritsched(&rt_sched_attr[i], PTHREAD_EXPLICIT_SCHED);
      rc=pthread_attr_setschedpolicy(&rt_sched_attr[i], SCHED_FIFO);
      rc=pthread_attr_setaffinity_np(&rt_sched_attr[i], sizeof(cpu_set_t), &threadcpu);

      rt_param[i].sched_priority=rt_max_prio-i;
      pthread_attr_setschedparam(&rt_sched_attr[i], &rt_param[i]);

      threadParams[i].threadIdx=i;
    }
   
    printf("Service threads will run on %d CPU cores\r\n", CPU_COUNT(&threadcpu));

    // camera_service = RT_MAX-1 @ 15 Hz
    //
    rt_param[1].sched_priority=rt_max_prio-1;
    pthread_attr_setschedparam(&rt_sched_attr[1], &rt_param[1]);
    rc=pthread_create(&threads[1],               // pointer to thread descriptor
                      &rt_sched_attr[1],         // use specific attributes
                      //(void *)0,               // default attributes
                      camera_service,                 // thread function entry point
                      (void *)&(threadParams[1]) // parameters to pass in
                     );
    if(rc < 0)
        perror("pthread_create for camera\r\n");
    else
        printf("pthread_create successful for camera\r\n");

    // motor_service = RT_MAX-2	@ 8 Hz
    //
    rt_param[2].sched_priority=rt_max_prio-2;
    pthread_attr_setschedparam(&rt_sched_attr[2], &rt_param[2]);
    rc=pthread_create(&threads[2], &rt_sched_attr[2], motor_service, (void *)&(threadParams[2]));
    if(rc < 0)
        perror("pthread_create for motor\r\n");
    else
        printf("pthread_create successful for motor\r\n");

    // ultrasonic_sensor_service = RT_MAX-3	@ 6 Hz
    //
    rt_param[3].sched_priority=rt_max_prio-3;
    pthread_attr_setschedparam(&rt_sched_attr[3], &rt_param[3]);
    rc=pthread_create(&threads[3], &rt_sched_attr[3], ultrasonic_sensor_service, (void *)&(threadParams[3]));
    if(rc < 0)
        perror("pthread_create for sensor failed\r\n");
    else
        printf("pthread_create successful for sensor\r\n");

    // Wait for service threads to initialize and await release by sequencer.
    usleep(1000000);
 
    // Create Sequencer thread
    printf("Start sequencer\n");

    // Sequencer = RT_MAX	@ 120 Hz
    //
    rt_param[0].sched_priority=rt_max_prio;
    pthread_attr_setschedparam(&rt_sched_attr[0], &rt_param[0]);
    rc=pthread_create(&threads[0], &rt_sched_attr[0], sequencer, (void *)&(threadParams[0]));
    if(rc < 0)
        perror("pthread_create for scheduler service 0");
    else
        printf("pthread_create successful for scheduler service 0\n");
        
   printf("Joining threads \r\n");


   for(i=0;i<NUM_THREADS;i++)
       pthread_join(threads[i], NULL);

   printf("TEST COMPLETE\n");
   return 0;
}
