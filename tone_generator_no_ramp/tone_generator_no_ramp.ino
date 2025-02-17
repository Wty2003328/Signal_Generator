#include <Arduino.h>
#include "AD9833.h"
#include <pt.h>
#include <Wire.h>
#include "PT2258.h"

// --------------------- Pin Definitions ----------------------
#define FNC_PIN     2
#define LED_PIN     8
#define BUTTON_PIN  9

// --------------------- Device Objects ------------------------
PT2258 pt2258(0x8C);
AD9833 waveGenerator(FNC_PIN);

// -------------------- Global Variables -----------------------
int  frequencyList[] = {3000, 10000, 30000};
int  volumeList[]    = {20, 12, 10};  // Default/Starting volumes
int  blinkCount      = 0;
int  frequencyIndex  = 0;
bool isRunning       = false;

// Protothread control blocks
static struct pt ptBlinkLED, ptGenerateTone;

// ------------------ Timing/Behavior Constants ---------------
static const unsigned long BLINK_ON_DURATION   = 10;   // LED on (ms)
static const unsigned long BLINK_OFF_DURATION  = 40;   // LED off (ms)
static const unsigned long BLINK_CYCLE_PAUSE   = 5000; // Pause (ms) after 80 blinks
static const int           BLINK_MAX_COUNT     = 80;   // After 80 blinks, pause

static const unsigned long RAMP_DELAY_US       = 500;  // 0.5 ms delay between attenuation steps
static const int           FREQUENCY_CYCLES    = 30;   // Stop after 30 frequency cycles
static const unsigned long LONG_PRESS_DELAY    = 3000; // ms: press button for 3s to toggle isRunning

// These thresholds match the original usage in the generateTone thread
static const int  BLINK_THRESHOLD_START        = 29;   // wave enable after blinkCount > 29
static const int  BLINK_THRESHOLD_MID          = 49;   // ramp up after blinkCount > 49
static const int  BLINK_THRESHOLD_END          = 79;   // reset after blinkCount > 79
static const unsigned long PAUSE_BETWEEN_CYCLES= 5000; // ms: wait 5s after a full cycle


// -----------------------------------------------------------------------------
// Protothread: Blink LED
// Blinks the LED for 80 cycles, then pauses 5 seconds, repeats.
//  - On  for BLINK_ON_DURATION
//  - Off for BLINK_OFF_DURATION
// After 80 blinks, wait 5000 ms, then reset blinkCount.
// -----------------------------------------------------------------------------
static int blinkLED(struct pt *pt)
{
  static unsigned long lastTime = 0;

  PT_BEGIN(pt);
  while (true)
  {
    // LED ON
    lastTime = millis();
    PT_WAIT_UNTIL(pt, (millis() - lastTime) >= BLINK_OFF_DURATION);
    digitalWrite(LED_PIN, HIGH);

    // LED OFF
    lastTime = millis();
    PT_WAIT_UNTIL(pt, (millis() - lastTime) >= BLINK_ON_DURATION);
    digitalWrite(LED_PIN, LOW);

    // Count each blink cycle
    blinkCount++;

    if (blinkCount > (BLINK_MAX_COUNT - 1))
    {
      // Pause after completing BLINK_MAX_COUNT cycles
      lastTime = millis();
      PT_WAIT_UNTIL(pt, (millis() - lastTime) >= BLINK_CYCLE_PAUSE);
      blinkCount = 0;
    }
  }
  PT_END(pt);
}

// -----------------------------------------------------------------------------
// Protothread: Generate Tones
// For each cycle:
//   1. Set volume to (baseVolume + 50), apply frequency
//   2. Wait until blinkCount > 29, then enable output
//   3. Ramp volume down to baseVolume in steps
//   4. Wait until blinkCount > 49, then ramp up volume to near max
//   5. Disable output, wait until blinkCount > 79, then wait 5s
//   6. Move to next frequency
// -----------------------------------------------------------------------------
static int generateTone(struct pt *pt)
{
  static unsigned long lastTime = 0;

  PT_BEGIN(pt);
  while (true)
  {
    // Set initial high volume offset and pick frequency
    int baseVolume  = volumeList[frequencyIndex % 3];
    int baseFreq    = frequencyList[frequencyIndex % 3];

    pt2258.attenuation(1, 79);
    waveGenerator.ApplySignal(SINE_WAVE, REG0, baseFreq);

    // Wait until blinkCount > 29, then enable output
    PT_WAIT_UNTIL(pt, blinkCount > BLINK_THRESHOLD_START);
    waveGenerator.EnableOutput(true);

    pt2258.attenuation(1, baseVolume);

    // Wait until blinkCount > 49
    PT_WAIT_UNTIL(pt, blinkCount > BLINK_THRESHOLD_MID);
    waveGenerator.EnableOutput(false);

    // Wait until blinkCount > 79, then pause 5s
    PT_WAIT_UNTIL(pt, blinkCount > BLINK_THRESHOLD_END);
    lastTime = millis();
    PT_WAIT_UNTIL(pt, (millis() - lastTime) >= PAUSE_BETWEEN_CYCLES);

    // Reset blinkCount if needed
    if (blinkCount == BLINK_MAX_COUNT)
    {
      blinkCount = 0;
    }

    // Move to next frequency
    frequencyIndex++;
  }
  PT_END(pt);
}

// -----------------------------------------------------------------------------
// Standard Arduino Setup
// -----------------------------------------------------------------------------
void setup()
{
  Serial.begin(9600);

  waveGenerator.Begin();
  waveGenerator.EnableOutput(false);

  pinMode(LED_PIN, OUTPUT);
  pinMode(FNC_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  digitalWrite(LED_PIN, LOW);

  // Initialize Protothreads
  PT_INIT(&ptBlinkLED);
  PT_INIT(&ptGenerateTone);

  // Set I2C speed
  Wire.setClock(400000);

  // Attempt to initialize PT2258
  if (!pt2258.begin()) {
    Serial.println("PT2258 Successfully Initiated");
  } else {
    Serial.println("Failed to Initiate PT2258");
  }

  // Unmute and set default attenuation
  pt2258.mute(false);
  pt2258.attenuation(1, 79);
}

// -----------------------------------------------------------------------------
// Standard Arduino Loop
// -----------------------------------------------------------------------------
void loop()
{
  static int previousButtonState = HIGH;

  // Read push button
  int currentButtonState = digitalRead(BUTTON_PIN);

  // For debugging/logging
  Serial.println(blinkCount);

  // If we've done 30 cycles, stop and reset everything
  if (frequencyIndex == FREQUENCY_CYCLES)
  {
    waveGenerator.EnableOutput(false);
    digitalWrite(LED_PIN, LOW);
    previousButtonState = HIGH;
    isRunning = false;     // Toggle off
    blinkCount = 0;
    frequencyIndex = 0;
    return;
  }

  // Toggle isRunning on long-press (3s)
  if (currentButtonState == LOW && previousButtonState == HIGH)
  {
    delay(50); // Debounce
    if (digitalRead(BUTTON_PIN) == LOW)
    {
      delay(LONG_PRESS_DELAY);
      isRunning = !isRunning;
    }
  }

  // If running, execute both protothreads
  if (isRunning)
  {
    blinkLED(&ptBlinkLED);
    generateTone(&ptGenerateTone);
  }

  previousButtonState = currentButtonState;
}
