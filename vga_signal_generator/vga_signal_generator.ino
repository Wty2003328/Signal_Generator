/*
  VGA Line Swipe Generator
  Author: [Your Name]
  License: MIT
  Description: Generates dynamic color line swipes on VGA display using Arduino.
  Connections:
    - D3: HSYNC (68Ω series resistor) → DB15 Pin 13
    - D4: Red (470Ω) → DB15 Pin 1
    - D5: Green (470Ω) → DB15 Pin 2
    - D6: Blue (470Ω) → DB15 Pin 3
    - D10: VSYNC (68Ω) → DB15 Pin 14
    - GND → DB15 Pins 5,6,7,8,10
*/

#include "TimerHelpers.h"
#include <avr/pgmspace.h>
#include <avr/sleep.h>

// ------------------------- Display Configuration ---------------------------
constexpr int HORIZONTAL_RESOLUTION = 480;  // Pixels wide
constexpr int VERTICAL_RESOLUTION = 480;    // Pixels tall
constexpr byte HORIZONTAL_BYTES = HORIZONTAL_RESOLUTION / 8;
constexpr int VERTICAL_LINES = VERTICAL_RESOLUTION / 16;

// ------------------------ Hardware Definitions ------------------------------
constexpr byte HSYNC_PIN = 3;
constexpr byte RED_PIN = 4;     // PORTD bit 4
constexpr byte GREEN_PIN = 5;    // PORTD bit 5
constexpr byte BLUE_PIN = 6;     // PORTD bit 6
constexpr byte VSYNC_PIN = 10;

// ------------------------- Timing Constants ---------------------------------
constexpr byte VERTICAL_BACK_PORCH = 35;    // Includes sync pulse
constexpr int VERTICAL_FRONT_PORCH = 525 - VERTICAL_BACK_PORCH;

// ----------------------- Animation Configuration ----------------------------
enum SwipeDirection {
  RIGHT_TO_LEFT,
  LEFT_TO_RIGHT,
  TOP_TO_BOTTOM,
  BOTTOM_TO_TOP
};

constexpr SwipeDirection DIRECTION = RIGHT_TO_LEFT;
constexpr int FRAMES_PER_MOVE = 0;        // Animation speed (0 = fastest)
constexpr int LINE_THICKNESS = 5;         // Swipe line width in pixels
constexpr byte COLOR_WHITE = 0x70;        // BGR format (0b01110000)

// ------------------------- Global Variables ----------------------------------
volatile int vLine = 0;
volatile int messageLine = 0;
volatile int backPorchLinesToGo = 0;
volatile byte newFrame = false;

byte frameBuffer[VERTICAL_LINES][HORIZONTAL_BYTES] = {0};
int currentPosition = 0;
int frameCounter = 0;

// ----------------------- Interrupt Service Routines --------------------------
ISR(TIMER1_OVF_vect) {  // Vertical Sync
  vLine = 0;
  messageLine = 0;
  backPorchLinesToGo = VERTICAL_BACK_PORCH;
  newFrame = true;
}

ISR(TIMER2_OVF_vect) {  // Horizontal Sync
  backPorchLinesToGo--;
}

// ---------------------------- Core Functions --------------------------------
void initializeDisplay() {
  // Configure Timer1 for vertical sync
  Timer1::setMode(15, Timer1::PRESCALE_1024, Timer1::CLEAR_B_ON_COMPARE);
  OCR1A = 259;  // 16.666ms period (60Hz)
  OCR1B = 0;
  TIFR1 = bit(TOV1);
  TIMSK1 = bit(TOIE1);

  // Configure Timer2 for horizontal sync
  Timer2::setMode(7, Timer2::PRESCALE_8, Timer2::CLEAR_B_ON_COMPARE);
  OCR2A = 63;   // 32μs period (31.25kHz)
  OCR2B = 7;     // 4μs pulse width
  TIFR2 = bit(TOV2);
  TIMSK2 = bit(TOIE2);

  // Set initial position based on direction
  switch(DIRECTION) {
    case RIGHT_TO_LEFT: currentPosition = HORIZONTAL_BYTES - 1; break;
    case BOTTOM_TO_TOP: currentPosition = VERTICAL_LINES - 1; break;
    default: currentPosition = 0;
  }
}

void updateFrameBuffer() {
  if (frameCounter++ < FRAMES_PER_MOVE) return;
  frameCounter = 0;

  switch(DIRECTION) {
    case RIGHT_TO_LEFT:
      for (int i = 0; i < LINE_THICKNESS; i++) {
        frameBuffer[0][(currentPosition - i + HORIZONTAL_BYTES) % HORIZONTAL_BYTES] = COLOR_WHITE;
      }
      frameBuffer[0][(currentPosition + 1) % HORIZONTAL_BYTES] = 0;
      currentPosition = (currentPosition - 1 + HORIZONTAL_BYTES) % HORIZONTAL_BYTES;
      break;

    case LEFT_TO_RIGHT:
      for (int i = 0; i < LINE_THICKNESS; i++) {
        frameBuffer[0][(currentPosition + i) % HORIZONTAL_BYTES] = COLOR_WHITE;
      }
      frameBuffer[0][(currentPosition - 1 + HORIZONTAL_BYTES) % HORIZONTAL_BYTES] = 0;
      currentPosition = (currentPosition + 1) % HORIZONTAL_BYTES;
      break;

    case TOP_TO_BOTTOM:
      for (int i = 0; i < LINE_THICKNESS; i++) {
        memset(&frameBuffer[(currentPosition + i) % VERTICAL_LINES][0], 
              COLOR_WHITE, HORIZONTAL_BYTES);
      }
      memset(&frameBuffer[(currentPosition - 1 + VERTICAL_LINES) % VERTICAL_LINES][0],
            0, HORIZONTAL_BYTES);
      currentPosition = (currentPosition + 1) % VERTICAL_LINES;
      break;

    case BOTTOM_TO_TOP:
      for (int i = 0; i < LINE_THICKNESS; i++) {
        memset(&frameBuffer[(currentPosition - i + VERTICAL_LINES) % VERTICAL_LINES][0],
              COLOR_WHITE, HORIZONTAL_BYTES);
      }
      memset(&frameBuffer[(currentPosition + 1) % VERTICAL_LINES][0],
            0, HORIZONTAL_BYTES);
      currentPosition = (currentPosition - 1 + VERTICAL_LINES) % VERTICAL_LINES;
      break;
  }
  newFrame = false;
}

bool renderScanLine() {
  if (backPorchLinesToGo > 0) {
    backPorchLinesToGo--;
    return false;
  }

  if (vLine >= VERTICAL_RESOLUTION) return false;

  byte* bufferPtr = &frameBuffer[messageLine][0];
  byte pixelsRemaining = HORIZONTAL_BYTES;

  // Fast port writing for pixel data
  while (pixelsRemaining--) {
    PORTD = *bufferPtr++;  // Direct port manipulation for D4-D6
    delayMicroseconds(1);  // Maintain pixel timing
  }

  // End of line cleanup
  PORTD = 0;  // Reset port
  vLine++;
  
  if ((vLine & 0x0F) == 0) messageLine++;
  
  return (vLine == VERTICAL_RESOLUTION);
}

// --------------------------- Arduino Core -----------------------------------
void setup() {
  // Configure pins
  pinMode(HSYNC_PIN, OUTPUT);
  pinMode(VSYNC_PIN, OUTPUT);
  pinMode(RED_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);
  
  // Disable unnecessary peripherals
  TIMSK0 = 0;  // Disable Timer0 interrupts
  ADCSRA = 0;  // Disable ADC
  power_all_disable();
  power_timer1_enable();
  power_timer2_enable();

  // Initialize display system
  set_sleep_mode(SLEEP_MODE_IDLE);
  initializeDisplay();
}

void loop() {
  while(true) {
    sleep_mode();
    if (renderScanLine() && newFrame) {
      updateFrameBuffer();
    }
  }
}