#include <Arduino.h>
#include "pins_arduino.h"
#define ch11 19
#define ch12 18
#define ch21 14
#define ch22 12
#define ch31 26
#define ch32 25
#define pi 3.1415926535
#define FREQUENCY 10000 // 10kHz
#define MotorF 30
#define PERIOD (1.0 / FREQUENCY) // 周期 (秒)
#define PERIOD_US (PERIOD * 1e6) // 周期 (マイクロ秒)
double counter_us(int t);
long int t = 0;

hw_timer_t *tmr0 = NULL;
void IRAM_ATTR onTimer1()
{
  int ch1, ch2, ch3;
  t = esp_timer_get_time();
  double t_sec = t / 1e6; // convert to seconds

  if ((sin(2 * pi * MotorF * t_sec) * 1000) > counter_us(t))
  {

    ch1 = 1;
  }
  else
  {
    ch1 = 0;
  }
  if ((sin(2 * pi * MotorF * t_sec + (2 * pi / 3)) * 1000) > counter_us(t))
  {
    ch2 = 1;
  }
  else
  {
    ch2 = 0;
  }
  if (sin(2 * pi * MotorF * t_sec + (4 * pi / 3)) * 1000 > counter_us(t))
  {
    ch3 = 1;
  }
  else
  {
    ch3 = 0;
  }

  GPIO.out_w1ts = (ch1 << ch11) + (ch2 << ch21) + (ch3 << ch31);
  GPIO.out_w1tc = (ch1 << ch12) + (ch2 << ch22) + (ch3 << ch32);
  GPIO.out_w1ts = ((1 - ch1) << ch12) + ((1 - ch2) << ch22) + ((1 - ch3) << ch32);
  GPIO.out_w1tc = ((1 - ch1) << ch11) + ((1 - ch2) << ch21) + ((1 - ch3) << ch31);
}

void setup()
{
  // put your setup code here, to run once:

  pinMode(ch11, OUTPUT);
  pinMode(ch12, OUTPUT);
  pinMode(ch21, OUTPUT);
  pinMode(ch22, OUTPUT);
  pinMode(ch31, OUTPUT);
  pinMode(ch32, OUTPUT);
  int ch1 = 0;
  int ch2 = 0;
  int ch3 = 0;

  tmr0 = timerBegin(0, 80, true);
  timerAttachInterrupt(tmr0, &onTimer1, true);
  timerAlarmWrite(tmr0, 10, true);
  timerAlarmEnable(tmr0);
}

void loop()
{
}
double counter_us(int t)
{
  double time_in_period = double(t % int(PERIOD_US));
  return time_in_period / PERIOD_US;
}
