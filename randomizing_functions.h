#pragma once

// Initialize a structure that will hold the output of randomizeTime
struct time_outputs {
  uint32_t cumulativeTime;
  int count;
};

struct time_outputs randomizeTime(void);
static void randomizeSpeed(time_outputs randomTime, uint32_t TotalTime, uint32_t StartRestTime); 
static void randomizeAccel(time_outputs randomTime);
void Report(float targetSpeed, int activityTag, String message);
void HeaderReport(int count); 
int getMinSpeed(void);
