#include "pins_arduino.h"
#include <Arduino.h>
#define ch11 19
#define ch12 18
#define ch21 14
#define ch22 12
#define ch31 26
#define ch32 25
#define pi 3.1415926535
#define FREQUENCY 1000           // 1kHz
#define PERIOD (1.0 / FREQUENCY) // 周期 (秒)
#define PERIOD_US (PERIOD * 1e6) // 周期 (マイクロ秒)
#define numdata 10000
double counter_us(int t);
long int t = 0;
int MotorF = 1;
double amp = 0.13;
TaskHandle_t thp[1];
typedef struct {
  byte channels;
  uint16_t pin15;
  uint16_t pin13;
  uint16_t pin33;
} data_t;
hw_timer_t *tmr0 = NULL;
volatile data_t data[numdata];

volatile int ch1 = 0;
volatile int ch2 = 0;
volatile int ch3 = 0;
void SaveState(void *args);
void Core0a(void *args);
void Core1a(void *args);
void setup() {
  // put your setup code here, to run once:

  pinMode(ch11, OUTPUT);
  pinMode(ch12, OUTPUT);
  pinMode(ch21, OUTPUT);
  pinMode(ch22, OUTPUT);
  pinMode(ch31, OUTPUT);
  pinMode(ch32, OUTPUT);

  pinMode(15, ANALOG);
  pinMode(13, ANALOG);
  pinMode(33, ANALOG);

  pinMode(32, ANALOG);
  pinMode(35, ANALOG);
  pinMode(34, ANALOG);

  Serial.begin(115200);
  xTaskCreatePinnedToCore(Core0a, "Core0a", 4096, NULL, 3, &thp[0], 0);
  xTaskCreatePinnedToCore(Core1a, "Core1a", 4096, NULL, 3, &thp[1], 1);
  for (int i = 0; i < numdata; i++) {
    data[i].channels = 0;
    data[i].pin15 = 0;
    data[i].pin13 = 0;
    data[i].pin33 = 0;
  }
}

void loop() {
  // Serial.println("Hello World");
}
double counter_us(int t) {
  double time_in_period = double(t % int(PERIOD_US));
  return time_in_period / PERIOD_US;
}
uint16_t adc1, adc2, adc3;
bool c1, c2, c3;

void SaveState(void *args) {
  static int i = 0;
  if (i == numdata) {
    i = 0;
  }
  adc1 = analogRead(15);
  adc2 = analogRead(13);
  adc3 = analogRead(33);
  c1 = ch1;
  c2 = ch2;
  c3 = ch3;
  data[i].channels = (c1 << 2) + (c2 << 1) + c3;
  data[i].pin15 = adc1;
  data[i].pin13 = adc2;
  data[i].pin33 = adc3;
  i++;
}
void Core0a(void *args) {
  static int i = 0;
  while (1) {
    i++;
    if (i == numdata) {
      i = 0;
      for (int j = 0; j < numdata; j++) {
        Serial.printf("%d,%d,%d,%d\n", data[j].channels, data[j].pin15,
                      data[j].pin13, data[j].pin33);
      }
    }
    Serial.flush();
    delay(1);
  }
}
void Core1a(void *args) {
  while (1) {

    t = esp_timer_get_time();
    double t_sec = t / 1e6; // convert to seconds

    if ((sin(2 * pi * MotorF * t_sec) * 1000) > counter_us(t)) {

      ch1 = 1;
    } else {
      ch1 = 0;
    }
    if ((sin(2 * pi * MotorF * t_sec + (2 * pi / 3)) * 1000) > counter_us(t)) {
      ch2 = 1;
    } else {
      ch2 = 0;
    }

    if (sin(2 * pi * MotorF * t_sec + (4 * pi / 3)) * 1000 > counter_us(t)) {
      ch3 = 1;
    } else {
      ch3 = 0;
    } //*/

    SaveState(NULL);
    if (counter_us(t) < amp) {
      GPIO.out_w1ts = (ch1 << ch11) + (ch2 << ch21) + (ch3 << ch31);
      GPIO.out_w1tc = (ch1 << ch12) + (ch2 << ch22) + (ch3 << ch32);
      GPIO.out_w1ts =
          ((1 - ch1) << ch12) + ((1 - ch2) << ch22) + ((1 - ch3) << ch32);
      GPIO.out_w1tc =
          ((1 - ch1) << ch11) + ((1 - ch2) << ch21) + ((1 - ch3) << ch31);
    } else {

      GPIO.out_w1tc = (1 << ch11) + (1 << ch21) + (1 << ch31);
      GPIO.out_w1ts = ((1) << ch12) + ((1) << ch22) + ((1) << ch32);
    }
  }
}