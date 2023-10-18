#include "user_console.h"
#include "Arduino.h"
#include "config.h"
#include "led_board.h"

#include <EnableInterrupt.h>

#define BOUNCING_TIME 100

uint8_t inputPins[] = {BUT01_PIN, BUT02_PIN, BUT03_PIN, BUT04_PIN};

uint8_t inputPattern[NLEDS];
int count = 0;
bool input_started = false;
bool wasAlreadyPressed[] = {false, false, false, false };

long lastButtonPressedTimeStamps[NUM_INPUT_POS];

void button_handler(int i);
void button_handler_0(){ button_handler(0); }
void button_handler_1(){ button_handler(1); }
void button_handler_2(){ button_handler(2); }
void button_handler_3(){ button_handler(3); }

void (*button_handlers[4])() = { button_handler_0, button_handler_1, button_handler_2, button_handler_3 };

void button_handler(int i){
  long ts = millis();
  if (ts - lastButtonPressedTimeStamps[i] > BOUNCING_TIME){
    lastButtonPressedTimeStamps[i] = ts;
    int status = digitalRead(inputPins[i]);
    if (status == HIGH && !wasAlreadyPressed[i]) { 
      input_started = true;
      inputPattern[NUM_INPUT_POS - 1 - count] = i;
      turn_on_led(i);
      wasAlreadyPressed[i] = true;
      count++;
    }
  }
}

int read_difficulty_level(){
  int pot_value = analogRead(POT_PIN);
  int ratio = 1023/DIFFICULTY_LEVELS;
  int level = 1;
  for(int i = 1; i <= DIFFICULTY_LEVELS; i++){
    if (pot_value < i*ratio){
      level = i;
    }
  }
  return level;
}

uint8_t* get_current_pattern(){
  return inputPattern;
}

void init_player_console(){
  Serial.begin(9600);
  for (int i = 0; i < NUM_INPUT_POS; i++) {
    pinMode(inputPins[i], INPUT);  
    enableInterrupt(inputPins[i], button_handlers[i], CHANGE);       
  }
}

void print_on_console(const String& msg){
  Serial.println(msg);
}

void reset_player_input(){
  long ts = millis();
  for (int i = 0; i < NUM_INPUT_POS; i++) {
    inputPattern[i] = NUM_INPUT_POS;      
    lastButtonPressedTimeStamps[i] = ts;
    wasAlreadyPressed[i] = false;
  }
  delay(BOUNCING_TIME);
  input_started = false;
  count = 0;
  log("input reset");
}

bool player_input_started(){
  return input_started;
}

void log(const String& msg){
  #ifdef __DEBUG__
  Serial.println(msg);
  #endif
}


void test_player_input(){
  for (int i = 0; i < NUM_INPUT_POS; i++) {
    if (inputPattern[i]) {
      Serial.println(String("button ") + i + " pressed"); 
    }
  }
  int value = analogRead(POT_PIN);
  Serial.println(value);
}