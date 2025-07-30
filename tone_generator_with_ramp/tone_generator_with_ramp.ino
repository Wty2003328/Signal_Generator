#include <Arduino.h>
#include "AD9833.h"
#include <pt.h>
#include <Wire.h>
#include "PT2258.h"

// --------------------- Pin Definitions ----------------------
#define FNC_PIN 2
#define CLOCK_PIN 8 // Was LED_PIN
#define TRIGGER_PIN 9

// --------------------- Device Objects ------------------------
PT2258 pt2258(0x8C);
AD9833 waveGenerator(FNC_PIN);

// -------------------- Global Variables -----------------------
int frequencyList[] = {3000, 10000, 30000};
int volumeList[] = {20, 12, 10}; // Default/Starting volumes
int clockCount = 0;
int frequencyIndex = 0;
bool isRunning = false;

// Protothread control blocks
static struct pt ptClockPulse, ptGenerateTone;

// ------------------ Timing Constants -------------------------
static const unsigned long CLOCK_HIGH_DURATION = 10; // HIGH time (ms)
static const unsigned long CLOCK_LOW_DURATION = 40;  // LOW time (ms)
static const unsigned long CLOCK_CYCLE_PAUSE = 5000; // Pause after N pulses
static const int CLOCK_MAX_COUNT = 80;

static const unsigned long RAMP_DELAY_US = 500; // 0.5 ms delay between attenuation steps
static const int FREQUENCY_CYCLES = 30;
static const int TONE_START_THRESHOLD = 29;
static const int TONE_RAMP_DOWN_THRESHOLD = 49;

// ------------------ Interrupt Service Routine ----------------
void triggerISR()
{
    isRunning = !isRunning;
}

// -----------------------------------------------------------------------------
// Protothread: Clock Pulse Generator (used as time base)
// -----------------------------------------------------------------------------
static int clockPulse(struct pt *pt)
{
    static unsigned long lastTime = 0;
    PT_BEGIN(pt);
    while (true)
    {
        lastTime = millis();
        PT_WAIT_UNTIL(pt, (millis() - lastTime) >= CLOCK_LOW_DURATION);
        digitalWrite(CLOCK_PIN, HIGH);

        lastTime = millis();
        PT_WAIT_UNTIL(pt, (millis() - lastTime) >= CLOCK_HIGH_DURATION);
        digitalWrite(CLOCK_PIN, LOW);

        clockCount++;
        if (clockCount >= CLOCK_MAX_COUNT)
        {
            lastTime = millis();
            PT_WAIT_UNTIL(pt, (millis() - lastTime) >= CLOCK_CYCLE_PAUSE);
            clockCount = 0;
        }
    }
    PT_END(pt);
}

// -----------------------------------------------------------------------------
// Protothread: Tone Generator
// -----------------------------------------------------------------------------
static int generateTone(struct pt *pt)
{
    static unsigned long lastTime = 0;
    PT_BEGIN(pt);
    while (true)
    {
        int baseVolume = volumeList[frequencyIndex % 3];
        int baseFreq = frequencyList[frequencyIndex % 3];

        pt2258.attenuation(1, min(baseVolume + 50, 79));
        waveGenerator.ApplySignal(SINE_WAVE, REG0, baseFreq);

        PT_WAIT_UNTIL(pt, clockCount > TONE_START_THRESHOLD);
        waveGenerator.EnableOutput(true);

        lastTime = micros();
        PT_WAIT_UNTIL(pt, micros() - lastTime > RAMP_DELAY_US);
        pt2258.attenuation(1, min(volumeList[frequencyIndex % 3] + 40, 79));
        lastTime = micros();
        PT_WAIT_UNTIL(pt, micros() - lastTime > RAMP_DELAY_US);
        pt2258.attenuation(1, min(volumeList[frequencyIndex % 3] + 30, 79));
        lastTime = micros();
        PT_WAIT_UNTIL(pt, micros() - lastTime > RAMP_DELAY_US);
        pt2258.attenuation(1, min(volumeList[frequencyIndex % 3] + 20, 79));
        lastTime = micros();
        PT_WAIT_UNTIL(pt, micros() - lastTime > RAMP_DELAY_US);
        pt2258.attenuation(1, min(volumeList[frequencyIndex % 3] + 16, 79));
        lastTime = micros();
        PT_WAIT_UNTIL(pt, micros() - lastTime > RAMP_DELAY_US);
        pt2258.attenuation(1, min(volumeList[frequencyIndex % 3] + 15, 79));
        lastTime = micros();
        PT_WAIT_UNTIL(pt, micros() - lastTime > RAMP_DELAY_US);
        pt2258.attenuation(1, min(volumeList[frequencyIndex % 3] + 14, 79));
        lastTime = micros();
        PT_WAIT_UNTIL(pt, micros() - lastTime > RAMP_DELAY_US);
        pt2258.attenuation(1, min(volumeList[frequencyIndex % 3] + 13, 79));
        lastTime = micros();
        PT_WAIT_UNTIL(pt, micros() - lastTime > RAMP_DELAY_US);
        pt2258.attenuation(1, min(volumeList[frequencyIndex % 3] + 12, 79));
        lastTime = micros();
        PT_WAIT_UNTIL(pt, micros() - lastTime > RAMP_DELAY_US);
        pt2258.attenuation(1, min(volumeList[frequencyIndex % 3] + 11, 79));
        lastTime = micros();
        PT_WAIT_UNTIL(pt, micros() - lastTime > RAMP_DELAY_US);
        pt2258.attenuation(1, min(volumeList[frequencyIndex % 3] + 10, 79));
        lastTime = micros();
        PT_WAIT_UNTIL(pt, micros() - lastTime > RAMP_DELAY_US);
        pt2258.attenuation(1, min(volumeList[frequencyIndex % 3] + 9, 79));
        lastTime = micros();
        PT_WAIT_UNTIL(pt, micros() - lastTime > RAMP_DELAY_US);
        pt2258.attenuation(1, min(volumeList[frequencyIndex % 3] + 8, 79));
        lastTime = micros();
        PT_WAIT_UNTIL(pt, micros() - lastTime > RAMP_DELAY_US);
        pt2258.attenuation(1, min(volumeList[frequencyIndex % 3] + 7, 79));
        lastTime = micros();
        PT_WAIT_UNTIL(pt, micros() - lastTime > RAMP_DELAY_US);
        pt2258.attenuation(1, min(volumeList[frequencyIndex % 3] + 6, 79));
        lastTime = micros();
        PT_WAIT_UNTIL(pt, micros() - lastTime > RAMP_DELAY_US);
        pt2258.attenuation(1, min(volumeList[frequencyIndex % 3] + 5, 79));
        lastTime = micros();
        PT_WAIT_UNTIL(pt, micros() - lastTime > RAMP_DELAY_US);
        pt2258.attenuation(1, min(volumeList[frequencyIndex % 3] + 4, 79));
        lastTime = micros();
        PT_WAIT_UNTIL(pt, micros() - lastTime > RAMP_DELAY_US);
        pt2258.attenuation(1, min(volumeList[frequencyIndex % 3] + 3, 79));
        lastTime = micros();
        PT_WAIT_UNTIL(pt, micros() - lastTime > RAMP_DELAY_US);
        pt2258.attenuation(1, min(volumeList[frequencyIndex % 3] + 2, 79));
        lastTime = micros();
        PT_WAIT_UNTIL(pt, micros() - lastTime > RAMP_DELAY_US);
        pt2258.attenuation(1, min(volumeList[frequencyIndex % 3] + 1, 79));

        lastTime = micros();
        PT_WAIT_UNTIL(pt, micros() - lastTime > RAMP_DELAY_US);
        pt2258.attenuation(1, volumeList[frequencyIndex % 3]);

        PT_WAIT_UNTIL(pt, clockCount > TONE_RAMP_DOWN_THRESHOLD);
        pt2258.attenuation(1, min(volumeList[frequencyIndex % 3] + 1, 79));
        lastTime = micros();
        PT_WAIT_UNTIL(pt, micros() - lastTime > RAMP_DELAY_US);
        pt2258.attenuation(1, min(volumeList[frequencyIndex % 3] + 2, 79));
        lastTime = micros();
        PT_WAIT_UNTIL(pt, micros() - lastTime > RAMP_DELAY_US);
        pt2258.attenuation(1, min(volumeList[frequencyIndex % 3] + 3, 79));
        lastTime = micros();
        PT_WAIT_UNTIL(pt, micros() - lastTime > RAMP_DELAY_US);
        pt2258.attenuation(1, min(volumeList[frequencyIndex % 3] + 4, 79));
        lastTime = micros();
        PT_WAIT_UNTIL(pt, micros() - lastTime > RAMP_DELAY_US);
        pt2258.attenuation(1, min(volumeList[frequencyIndex % 3] + 5, 79));
        lastTime = micros();
        PT_WAIT_UNTIL(pt, micros() - lastTime > RAMP_DELAY_US);
        pt2258.attenuation(1, min(volumeList[frequencyIndex % 3] + 6, 79));
        lastTime = micros();
        PT_WAIT_UNTIL(pt, micros() - lastTime > RAMP_DELAY_US);
        pt2258.attenuation(1, min(volumeList[frequencyIndex % 3] + 7, 79));
        lastTime = micros();
        PT_WAIT_UNTIL(pt, micros() - lastTime > RAMP_DELAY_US);
        pt2258.attenuation(1, min(volumeList[frequencyIndex % 3] + 8, 79));
        lastTime = micros();
        PT_WAIT_UNTIL(pt, micros() - lastTime > RAMP_DELAY_US);
        pt2258.attenuation(1, min(volumeList[frequencyIndex % 3] + 9, 79));
        lastTime = micros();
        PT_WAIT_UNTIL(pt, micros() - lastTime > RAMP_DELAY_US);
        pt2258.attenuation(1, min(volumeList[frequencyIndex % 3] + 10, 79));
        lastTime = micros();
        PT_WAIT_UNTIL(pt, micros() - lastTime > RAMP_DELAY_US);
        pt2258.attenuation(1, min(volumeList[frequencyIndex % 3] + 11, 79));
        lastTime = micros();
        PT_WAIT_UNTIL(pt, micros() - lastTime > RAMP_DELAY_US);
        pt2258.attenuation(1, min(volumeList[frequencyIndex % 3] + 12, 79));
        lastTime = micros();
        PT_WAIT_UNTIL(pt, micros() - lastTime > RAMP_DELAY_US);
        pt2258.attenuation(1, min(volumeList[frequencyIndex % 3] + 13, 79));
        lastTime = micros();
        PT_WAIT_UNTIL(pt, micros() - lastTime > RAMP_DELAY_US);
        pt2258.attenuation(1, min(volumeList[frequencyIndex % 3] + 14, 79));
        lastTime = micros();
        PT_WAIT_UNTIL(pt, micros() - lastTime > RAMP_DELAY_US);
        pt2258.attenuation(1, min(volumeList[frequencyIndex % 3] + 15, 79));
        lastTime = micros();
        PT_WAIT_UNTIL(pt, micros() - lastTime > RAMP_DELAY_US);
        pt2258.attenuation(1, min(volumeList[frequencyIndex % 3] + 16, 79));
        lastTime = micros();
        PT_WAIT_UNTIL(pt, micros() - lastTime > RAMP_DELAY_US);
        pt2258.attenuation(1, min(volumeList[frequencyIndex % 3] + 20, 79));
        lastTime = micros();
        PT_WAIT_UNTIL(pt, micros() - lastTime > RAMP_DELAY_US);
        pt2258.attenuation(1, min(volumeList[frequencyIndex % 3] + 30, 79));
        lastTime = micros();
        PT_WAIT_UNTIL(pt, micros() - lastTime > RAMP_DELAY_US);
        pt2258.attenuation(1, min(volumeList[frequencyIndex % 3] + 40, 79));
        lastTime = micros();
        PT_WAIT_UNTIL(pt, micros() - lastTime > RAMP_DELAY_US);
        pt2258.attenuation(1, min(volumeList[frequencyIndex % 3] + 50, 79));
        lastTime = micros();
        PT_WAIT_UNTIL(pt, micros() - lastTime > RAMP_DELAY_US);

        pt2258.attenuation(1, 79);
        waveGenerator.EnableOutput(false);

        PT_WAIT_UNTIL(pt, clockCount >= CLOCK_MAX_COUNT);
        lastTime = millis();
        PT_WAIT_UNTIL(pt, (millis() - lastTime) >= CLOCK_CYCLE_PAUSE);
        clockCount = 0;

        frequencyIndex++;
    }
    PT_END(pt);
}

// -----------------------------------------------------------------------------
// Setup
// -----------------------------------------------------------------------------
void setup()
{
    Serial.begin(9600);
    waveGenerator.Begin();
    waveGenerator.EnableOutput(false);

    pinMode(CLOCK_PIN, OUTPUT);
    pinMode(FNC_PIN, OUTPUT);
    pinMode(TRIGGER_PIN, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(TRIGGER_PIN), triggerISR, RISING);

    digitalWrite(CLOCK_PIN, LOW);
    PT_INIT(&ptClockPulse);
    PT_INIT(&ptGenerateTone);

    Wire.setClock(400000);
    if (!pt2258.begin())
    {
        Serial.println("PT2258 Successfully Initiated");
    }
    else
    {
        Serial.println("Failed to Initiate PT2258");
    }

    pt2258.mute(false);
    pt2258.attenuation(1, 79);
}

// -----------------------------------------------------------------------------
// Main Loop
// -----------------------------------------------------------------------------
void loop()
{
    int currentButtonState = digitalRead(BUTTON_PIN);
    static int previousButtonState = HIGH;

    Serial.println(clockCount);
    if (frequencyIndex == FREQUENCY_CYCLES)
    {
        waveGenerator.EnableOutput(false);
        digitalWrite(CLOCK_PIN, LOW);
        previousButtonState = HIGH;
        isRunning = false;
        clockCount = 0;
        frequencyIndex = 0;
        return;
    }

    if (currentButtonState == LOW && previousButtonState == HIGH)
    {
        delay(50); // Debounce delay
        if (digitalRead(BUTTON_PIN) == LOW)
        {
            delay(3000);
            isRunning = !isRunning;
        }
    }

    if (isRunning)
    {
        clockPulse(&ptClockPulse);
        generateTone(&ptGenerateTone);
    }
    previousButtonState = currentButtonState;
}
