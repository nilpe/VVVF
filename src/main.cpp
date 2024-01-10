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
double counter_us(int t);
long int t = 0;
int MotorF = 8;
TaskHandle_t thp[1];
typedef struct {
  byte channels;
  uint16_t pin15;
  uint16_t pin13;
  uint16_t pin33;
} data_t;
hw_timer_t *tmr0 = NULL;
volatile data_t data[1000];
void IRAM_ATTR onTimer1() {
  /*
    Serial.printf("pin15: %u\n", analogReadMilliVolts(15));
    Serial.printf("pin13: %u\n", analogReadMilliVolts(13));
    Serial.printf("pin33: %u\n\n", analogReadMilliVolts(33));
  //*/
}

volatile int ch1 = 0;
volatile int ch2 = 0;
volatile int ch3 = 0;
void SaveState(void *args);
void Core0a(void *args);
void setup() {
  // put your setup code here, to run once:

  pinMode(ch11, OUTPUT);
  pinMode(ch12, OUTPUT);
  pinMode(ch21, OUTPUT);
  pinMode(ch22, OUTPUT);
  pinMode(ch31, OUTPUT);
  pinMode(ch32, OUTPUT);
  // analogSetAttenuation(ADC_6db);
  pinMode(15, ANALOG);
  pinMode(13, ANALOG);
  pinMode(33, ANALOG);

  pinMode(32, ANALOG);
  pinMode(35, ANALOG);
  pinMode(34, ANALOG);

  // tmr0 = timerBegin(0, 80, true);
  // timerAttachInterrupt(tmr0, &onTimer1, true);
  // timerAlarmWrite(tmr0, 1000000, true);
  // timerAlarmEnable(tmr0);
  Serial.begin(115200);
  xTaskCreatePinnedToCore(Core0a, "Core0a", 4096, NULL, 3, &thp[0], 0);
}

void loop() {
  // Serial.println("Hello World");

  t = esp_timer_get_time();
  double t_sec = t / 1e6; // convert to seconds
  // increasing the frequency of the motor by 1Hz every 0.1 seconds
  /*if (MotorF > 50) {
      MotorF = 50;
    }else  if (t%1000000 == 100000) {
      MotorF += 1;
      t = 0;
    }//*/
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
  /*if (counter_us(t) < 0.4 ) {
    ch1 = 0; // pin15,32
    ch2 = 1; // pin13,34
    ch3 = 0; // pin33,35
  } else {
    ch1 = 0;
    ch2 = 0;
    ch3 = 0;
  }//*/
  SaveState(NULL);
  if (counter_us(t) > 0.15) {
    GPIO.out_w1ts = (ch1 << ch11) + (ch2 << ch21) + (ch3 << ch31);
    GPIO.out_w1tc = (ch1 << ch12) + (ch2 << ch22) + (ch3 << ch32);
    GPIO.out_w1ts =
        ((1 - ch1) << ch12) + ((1 - ch2) << ch22) + ((1 - ch3) << ch32);
    GPIO.out_w1tc =
        ((1 - ch1) << ch11) + ((1 - ch2) << ch21) + ((1 - ch3) << ch31);
  } else {
    // GPIO.out_w1ts = (ch1 << ch11) + (ch2 << ch21) + (ch3 << ch31);
    GPIO.out_w1tc = (1 << ch12) + (1 << ch22) + (1 << ch32);
    GPIO.out_w1ts = ((1) << ch12) + ((1) << ch22) + ((1) << ch32);
  }
}
double counter_us(int t) {
  double time_in_period = double(t % int(PERIOD_US));
  return time_in_period / PERIOD_US;
}
uint16_t adc1, adc2, adc3;
bool c1, c2, c3;

void SaveState(void *args) {
  static int i = 0;
  if (i == 1000) {
    i = 0;
  }
  adc1 = analogRead(15);
  adc2 = analogRead(13);
  adc3 = analogRead(33);
  c1 = ch1;
  c2 = ch2;
  c3 = ch3;
  /* Serial.printf("pin15: %u\n", adc1);
  Serial.printf("pin13: %u\n", adc2);
  Serial.printf("pin33: %u\n", adc3);
  Serial.printf("ch1: %d\n", c1);
  Serial.printf("ch2: %d\n", c2);
  Serial.printf("ch3: %d\n", c3);
  Serial.printf("hoge\n\n");*/
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
    if (i == 1000) {
      i = 0;
      for (int j = 0; j < 1000; j++) {
        Serial.printf("%d,%d,%d,%d\n", data[j].channels, data[j].pin15,
                      data[j].pin13, data[j].pin33);
      }
    }
    Serial.flush();
    delay(1);
  }
}