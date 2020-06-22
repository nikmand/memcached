//
//  stats.c
//
//  Author: David Meisner (meisner@umich.edu)
//

#include "stats.h"
#include "loader.h"
#include <assert.h>
#include "worker.h"
#include "communication.h"

pthread_mutex_t stats_lock = PTHREAD_MUTEX_INITIALIZER;

void addSample(struct stat* stat, float value) {
  stat->s0 += 1.0;
  stat->s1 += value;
  stat->s2 += value*value;
  stat->min = fmin(stat->min,value);
  stat->max = fmax(stat->max,value);

  if(value < .001){
    int bin = (int)(value*10000000);
    stat->micros[bin] += 1;
  } else if( value < 5.0){
    int bin = value * 10000.0;
    assert(bin < 50001);
    stat->millis[bin] += 1;
  } else if (value < 999){
    int bin = (int)value;
    stat->fulls[bin] += 1;
  } else {
    int bin = (int)value/1000;
    if (bin > 999){
      bin = 999;
    }
    stat->fulls[bin] += 1;
  }


}//End addAvgSample()

double getAvg(struct stat* stat) {
  return (stat->s1/stat->s0);
}//End getAvg()

double getStdDev(struct stat* stat) {
  return sqrt((stat->s0*stat->s2 - stat->s1*stat->s1)/(stat->s0*(stat->s0 - 1)));
}//End getStdDev()

//Should we exit because time has expired?
void checkExit(struct config* config) {

  int runTime = config->run_time;
  struct timeval currentTime;
  gettimeofday(&currentTime, NULL);
  double totalTime = currentTime.tv_sec - start_time.tv_sec + 1e-6*(currentTime.tv_sec - start_time.tv_sec);
  if(totalTime >= runTime && runTime >0) {
    printf("Ran for %f, exiting\n", totalTime);
    exit(0);
  }

}//End checkExit()

double findQuantile(struct stat* stat, double quantile) { 

  //Find the 95th-percentile
  int nTillQuantile = global_stats.response_time.s0 * quantile;
  int  count = 0;
  int i;
  for( i = 0; i < 10000; i++) {
    count += stat->micros[i];
    if( count >= nTillQuantile ){
      double quantile = (i+1) * .0000001;
      return quantile;
    }
  }//End for i

  for( i = 0; i < 50000; i++) {
    count += stat->millis[i];
    if( count >= nTillQuantile ){
      double quantile = (i+1) * .0001;
      return quantile;
    }
  }//End for i
  printf("count  %d\n", count);

  for( i = 0; i < 1000; i++) {
    count += stat->fulls[i];
    if( count >= nTillQuantile ){
      double quantile = i+1;
      return quantile;
    }
  }//End for i
  return 1000;

}//End findQuantile()

void resetStats(){  // NOTE needs the lock
    memset(&global_stats, 0, sizeof(struct memcached_stats));
    global_stats.response_time.min = 1000000;
    struct timeval currentTime;
    gettimeofday(&currentTime, NULL);
    global_stats.last_time = currentTime;
}

double calcStats(){  // NOTE needs the lock
    double q95 = findQuantile(&global_stats.response_time, .95) * 1000;
    printf("Percentile: %10f \n", q95);
    return q95; // in milliseconds
}

void printGlobalStats(struct config* config) {

  pthread_mutex_lock(&stats_lock);
  struct timeval currentTime;
  gettimeofday(&currentTime, NULL);
  //double timeDiff =  currentTime.tv_sec - global_stats.last_time.tv_sec + 1e-6*(currentTime.tv_sec - global_stats.last_time.tv_sec); // probable usec
  double timeDiff =  1e+3*(currentTime.tv_sec - global_stats.last_time.tv_sec) + 1e-3*(currentTime.tv_usec - global_stats.last_time.tv_usec);
  double rps = global_stats.requests/(timeDiff / 1000.0);
  double std = getStdDev(&global_stats.response_time);
  double q90 = findQuantile(&global_stats.response_time, .90);
  double q95 = findQuantile(&global_stats.response_time, .95);
  double q99 = findQuantile(&global_stats.response_time, .99);

  printf("%10s,%8s,%16s, %8s,%11s,%10s,%13s,%10s,%10s,%10s,%12s,%10s,%10s,%11s,%14s\n", "timeDiff", "rps", "requests", "gets", "sets",  "hits", "misses", "avg_lat", "90th", "95th", "99th", "std", "min", "max", "avgGetSize");
  printf("%10f, %9.1f,  %10d, %10d, %10d, %10d, %10d, %10f, %10f, %10f, %10f, %10f, %10f, %10f, %10f\n", 
		timeDiff, rps, global_stats.requests, global_stats.gets, global_stats.sets, global_stats.hits, global_stats.misses,
		1000*getAvg(&global_stats.response_time), 1000*q90, 1000*q95, 1000*q99, 1000*std, 1000*global_stats.response_time.min, 1000*global_stats.response_time.max, getAvg(&global_stats.get_size));

  int i;
  printf("Outstanding requests per worker:\n");
  for(i=0; i<config->n_workers; i++){
    printf("%d ", config->workers[i]->n_requests);
  } 
  printf("\n");
  //Reset stats
  resetStats();

  checkExit(config);
  pthread_mutex_unlock(&stats_lock);

}//End printGlobalStats()

//Print out statistics every second
void statsLoop(struct config* config) {

  printf("Entering statsLoop\n");
  pthread_mutex_lock(&stats_lock);
  gettimeofday(&start_time, NULL);
  pthread_mutex_unlock(&stats_lock);

  sleep(2);
  printf("Stats:\n");
  printf("-------------------------\n");
  printf("stats_time = %d\n", config->stats_time);
  while(1) {
    printGlobalStats(config);
    usleep(config->stats_time * 1000);
  }//End while()


}//End statisticsLoop()


void ipcStatsLoop(struct config* config){

    printf("Entering ipcStatsLoop\n");
    //struct timeval currentTime;
    int sockfd, rlAgent_socket;
    double q95;

    sleep(2);
    sockfd = initCommunication();

    while(1){
        rlAgent_socket = rlAgentSync(sockfd); // blocking call
        pthread_mutex_lock(&stats_lock);
        //gettimeofday(&currentTime, NULL);
        resetStats();
        pthread_mutex_unlock(&stats_lock);
        //printf("Start Recording\n");
        usleep(config->stats_time * 1000);
        //printf("Stop Recording\n");
        pthread_mutex_lock(&stats_lock);
        q95 = calcStats();
        //checkExit(config);
        pthread_mutex_unlock(&stats_lock);
        sendStats(rlAgent_socket, q95);  // better out of mutex, to avoid queuing, q95 is just a number no danger to be changed
    }
} // End ipcStatsLopp

