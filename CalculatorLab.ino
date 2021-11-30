#include "cowpi.h"

struct gpio_registers *gpio;
struct spi_registers *spi;
struct timer_registers_16bit *timer1;
volatile uint8_t *timer;
unsigned long last_keypad_press = 0;

const uint8_t keys[4][4] = {
  {0x1, 0x2, 0x3, 0xA},
  {0x4, 0x5, 0x6, 0xB},
  {0x7, 0x8, 0x9, 0xC},
  {0xF, 0x0, 0xE, 0xD}
};

const uint8_t seven_segments[23] = {
  0b01111110, 0b00110000, 0b01101101, 0b01111001,
  0b00110011, 0b01011011, 0b01011111, 0b01110000,
  0b01111111, 0b01110011, 0b01110111, 0b00011111,
  0b00001101, 0b00111101, 0b01001111, 0b01000111
};


void setup() {
  Serial.begin(9600);
  gpio = 0x23;
  spi = 0x4C;
  timer1 = 0x80;
  timer = 0x6E;
  setup_simple_io();
  setup_keypad();
  setup_display_module();
  setup_timer();
  pinMode(2, INPUT);
  pinMode(3, INPUT);
  attachInterrupt(digitalPinToInterrupt(2), handle_buttonpress, CHANGE);
  attachInterrupt(digitalPinToInterrupt(3), handle_keypress, CHANGE);
}

void setup_timer(){
  //every second
  timer1->control |= 0b0000110100000000;
  timer1->compareA = 15625;
}

void setup_simple_io() {
  gpio[A0_A5].direction &= 0b11001111;
  gpio[A0_A5].output &= 0b11001111;
  gpio[D8_D13].direction &= 0b11111100;
  gpio[D8_D13].output |= 0b00000011;
  gpio[D8_D13].direction |= 0b00010000;
}

void setup_keypad() {
  gpio[D0_D7].direction |= 0b11110000;
  gpio[D0_D7].output &= 0b00001111;
  gpio[A0_A5].direction &= 0b11110000;
  gpio[A0_A5].output |= 0b00001111;
}

void setup_display_module() {
  gpio[D8_D13].direction |= 0b00101100;
  spi->control |= 0b01010001;
  for (char i = 1; i <= 8; i++) {
    display_data(i, 0);     // clear all digit registers
  }
  display_data(0xA, 8);     // intensity at 17/32
  display_data(0xB, 7);     // scan all eight digits
  display_data(0xC, 1);     // take display out of shutdown mode
  display_data(0xF, 0);     // take display out of test mode, just in case
}

uint8_t get_key_pressed() {
  uint8_t key_pressed = 0xFF;
  unsigned long now = millis();
  if (now - last_keypad_press > 500) {
    last_keypad_press = now;
    for(int i = 0; i < 4; i++){
      gpio[D0_D7].output |= 0b11110000;
      gpio[D0_D7].output ^= (1<<(i+4));
      for(int j = 0; j < 4; j++){
        if(!(gpio[A0_A5].input & (1<<j))){
          key_pressed = keys[i][j];
        }
      }
    }
    gpio[D0_D7].output &= 0b00001111;
  }
  return key_pressed;
}

void display_data(uint8_t address, uint8_t value) {
  // address is MAX7219's register address (1-8 for digits; otherwise see MAX7219 datasheet Table 2)
  // value is the bit pattern to place in the register
  gpio[D8_D13].output &= 0b11111011;
  spi->data = address;
  while(!(spi->status & 0b10000000));
  spi->data = value;
  while(!(spi->status & 0b10000000));
  gpio[D8_D13].output |= 0b00000100;
}

ISR(TIMER1_COMPA_vect){
  //code for when timer interrupt occurs
}

void handle_buttonpress(){
  
}

void handle_keypress(){
  
}

void loop() {
  // put your main code here, to run repeatedly:

}
