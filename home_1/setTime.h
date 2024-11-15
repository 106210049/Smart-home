// setTime.h
#ifndef SETTIME_H
#define SETTIME_H

struct TimeRequest {
  unsigned int timeOn;
  unsigned int timeOff;
};

// Hàm để cấu hình thời gian cho cấu trúc TimeRequest
TimeRequest createTimeRequest(int timeOn, int timeOff) {
  TimeRequest timing;
  timing.timeOn = timeOn;
  timing.timeOff = timeOff;
  return timing;
}

#endif
