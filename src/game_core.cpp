#include "game_core.h"
#include "Arduino.h"
#include "config.h"
#include "led_board.h"
#include "user_console.h"
#include <avr/sleep.h>

#define MAX_WAITING_TIME 10000
#define TIME_IN_GAME_OVER 10000
#define T1_TIME 2000
#define T2_TIME 2000
#define T3_TIME 6000
#define MIN_TIME_DISPLAY 500
#define MIN_TIME_INPUT 1500

/* current pattern to be generated */
uint8_t current_pattern[NLEDS];

/* current max time to generate the pattern */
long max_time_to_generate;

/* current max time that can be used to input the pattern */
long max_time_to_form_pattern;

/* current max time to turn off the led on a random pattern */
long max_time_to_turn_off;

/* difficulty level */
int difficulty_level;

/* difficulty scaling factor */
double difficulty_scaling_factor = 1;

/* the score */
long score = 0;

/* define if the pattern has been generated or not */
bool generated = false;

/* current game state */
int game_state;

/* time in which the game entered in the game state */
long entered_state_time;

/* how long it the game is in current state */
long current_time_in_state;


void change_game_state(int new_state){
  game_state = new_state;
  entered_state_time = millis();
}

void update_game_state_time(){
  current_time_in_state = millis() - entered_state_time;
}

void check_difficulty_level(){
  int new_difficulty_level = read_difficulty_level();
  if (new_difficulty_level != difficulty_level){
    difficulty_level = new_difficulty_level;
    Serial.println(String("New difficulty Level: ") + difficulty_level);  
  } 
}

void game_intro(){
  reset_led_board();
  reset_player_input();
  Serial.println(String("Welcome to the Catch the Led Pattern Game. Press Key B1 to Start"));
  reset_pulsing();
  change_game_state(GAME_WAIT_TO_START);
}

void game_wait_to_start(){
  if (current_time_in_state < MAX_WAITING_TIME){
    go_on_pulsing();
    check_difficulty_level();
    if (player_input_started()){
      change_game_state(GAME_INIT);
    }
  } else {
    change_game_state(GAME_SLEEP);
  }
}

void game_sleep(){
  Serial.println(String("Entering sleep mode..."));
  reset_pulsing();
  delay(500);
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);  
  sleep_enable();
  sleep_mode();  
  sleep_disable();   
  change_game_state(GAME_INTRO);
}

void generatePattern(){
  max_time_to_generate = T1_TIME;
  generated = false;
  light_led_board();
  change_game_state(GAME_LOOP_GENERATE_PATTERN);
}

void game_init(){
  reset_pulsing();

  difficulty_level = read_difficulty_level();
  difficulty_scaling_factor = 1.0 + difficulty_level*0.1;  
  score = 0;

  Serial.println(String("Go!"));  
  max_time_to_turn_off = T2_TIME - difficulty_level*100; 
  max_time_to_form_pattern = T3_TIME;
  generatePattern();
}


void game_loop_generate_pattern(){
  if (current_time_in_state >= max_time_to_generate){
    change_game_state(GAME_LOOP_DISPLAY_PATTERN);
  }

  if (!generated) {
    boolean used[NLEDS];
    for (int i = 0; i < NLEDS; i++) {
      used[i] = false;
    }

    for (int i = 0; i < NLEDS; i++) {
      boolean done = false;
      uint8_t n, j;
      do {
        n = random(NLEDS);
        j = 0;
        for (; j < i; j++) {
          if (current_pattern[j] == n) {
            break;
          }
        }
        if (j == i) {
          current_pattern[i] = n;
          used[n] = true;
          done = true;
        }
      } while (!done);
    }
    generated = true;
  }
}


void game_loop_display_pattern(){
  turn_off_pattern(current_pattern, max_time_to_turn_off);
  Serial.println(String("Inserire pattern"));
  reset_player_input();
  change_game_state(GAME_LOOP_WAITING_PLAYER_PATTERN);
}

void change_to_game_over(){
  reset_led_board();
  go_on_lose();
  Serial.println(String("Game Over - Final Score: ") + score);
  change_game_state(GAME_OVER);
}


void game_loop_wait_player_pattern(){
  uint8_t* current_input_pat = get_current_pattern();
  if (current_time_in_state >= max_time_to_form_pattern){
    bool correct_input = true;
    for(int i = 0; i < NLEDS && correct_input; i++){
      if (current_input_pat[i] != current_pattern[i]){
        correct_input = false;
      }
    }

    if (!correct_input){
      change_to_game_over();
    } else {
      score++;
      if(max_time_to_turn_off > MIN_TIME_DISPLAY)
        max_time_to_turn_off /= difficulty_scaling_factor;
      if(max_time_to_form_pattern > MIN_TIME_INPUT) 
        max_time_to_form_pattern /= difficulty_scaling_factor;
      Serial.println(String("New Point! Score: ") + score);
      reset_led_board();
      generatePattern();
    }
  }
}

void game_over(){
  if (current_time_in_state > TIME_IN_GAME_OVER){
     change_game_state(GAME_INTRO);
  }
}

int get_game_state(){
  return game_state;
}