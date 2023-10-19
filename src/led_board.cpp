#include "Arduino.h"
#include "config.h"
#include "led_board.h"

uint8_t ledPin[] = {LED01_PIN, LED02_PIN, LED03_PIN, LED04_PIN};
uint8_t pulseIntensity = 0;
uint8_t pulseDelta = 5;

extern void log(const String& msg);

void init_led_board(){   
  for (int i = 0; i < NLEDS; i++){
    pinMode(ledPin[i], OUTPUT);     
  }
  pinMode(LED_START, OUTPUT);     
}

void reset_led_board(){   
  for (int i = 0; i < NLEDS; i++){
    digitalWrite(ledPin[i], LOW);
  }
}

void light_led_board() {
  for (int i = 0; i < NLEDS; i++){
    digitalWrite(ledPin[i], HIGH);
  }
}

void turn_on_led(int nled) {
  digitalWrite(ledPin[nled], HIGH);
}

void reset_pulsing() {
  pulseIntensity = 0;
  pulseDelta = 5;
  analogWrite(LED_START, pulseIntensity);   
}

void go_on_pulsing(){
  analogWrite(LED_START, pulseIntensity);   
  pulseIntensity = pulseIntensity + pulseDelta;
  if (pulseIntensity == 0 || pulseIntensity == 255) {
    pulseDelta = -pulseDelta ; 
  }     
  delay(15);                               
}

void go_on_lose(){
  digitalWrite(LED_START, HIGH);
  delay(1000);
  digitalWrite(LED_START, LOW);
}

void turn_off_pattern(uint8_t pattern[NLEDS], long delaytime){
  for (int i = 0; i < NLEDS; i++){
    digitalWrite(ledPin[pattern[i]], LOW);
  }
  delay(delaytime);
}
