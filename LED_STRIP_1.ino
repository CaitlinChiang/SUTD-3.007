#include <FastLED.h>
#define USE_ARDUINO_INTERRUPTS true 
#include <PulseSensorPlayground.h>
#include <Wire.h>


// DEFINING VARIABLES FOR LEDs
#define LED_TYPE             WS2811 // LED CHIPS WITH A DATA LINE, GROUND, AND POWER
#define NUM_LEDS_PER_STRIP   30
#define LED_BRIGHTNESS       255
#define COLOR_ORDER          GRB

#define LED_DIGITAL_PIN_1    9
#define LED_DIGITAL_PIN_2    11

CRGB LED_STRIP_1[NUM_LEDS_PER_STRIP];
CRGB LED_STRIP_2[NUM_LEDS_PER_STRIP];

CRGBPalette16 LED_COLOR_1;
CRGBPalette16 LED_COLOR_2;

TBlendType    COLOR_BLENDING;


// DEFINING VARIABLES FOR PULSE SENSOR
PulseSensorPlayground PULSE_SENSOR;

#define PULSE_ANALOG_PIN     1

int PULSE_SIGNAL;
int BPM_SIGNAL;
int THRESHOLD = 550;

byte BPM_1 = 0;
byte BPM_2;
int PULSES_BEFORE_AVERAGING = 0;
int BPM_BLINK = 0;
int TOUCHED_ONCE = 0;


// INITIAL SETUPS
void setup() {
    Wire.begin();
    Serial.begin(9600);

    // PULSE SENSOR
    PULSE_SENSOR.analogInput(PULSE_ANALOG_PIN);
    PULSE_SENSOR.setThreshold(THRESHOLD);

    if (PULSE_SENSOR.begin()) {
      Serial.println("Pulse Sensor is activated.");
    }

    // LEDs
    delay(3000);
    FastLED.addLeds<LED_TYPE, LED_DIGITAL_PIN_1, COLOR_ORDER>(LED_STRIP_1, NUM_LEDS_PER_STRIP).setCorrection(TypicalLEDStrip);
    FastLED.addLeds<LED_TYPE, LED_DIGITAL_PIN_2, COLOR_ORDER>(LED_STRIP_2, NUM_LEDS_PER_STRIP).setCorrection(TypicalLEDStrip);
    FastLED.setBrightness(LED_BRIGHTNESS);
    
    SetupLEDColors();
    COLOR_BLENDING = NOBLEND;
}


// MAIN LOOP
void loop()
{
    PULSE_SIGNAL = analogRead(PULSE_ANALOG_PIN);
    BPM_SIGNAL = ConvertBPMToMiliseconds(PULSE_SENSOR.getBeatsPerMinute());

    if (PULSES_BEFORE_AVERAGING > 600) {
      BPM_1 = 500;
    }
    if (PULSES_BEFORE_AVERAGING < 600) {
      BPM_1 = BPM_SIGNAL;
    }

    Wire.beginTransmission(5);
    Wire.write(BPM_1);
    Wire.endTransmission();

    Wire.requestFrom(5, 1);
    while (Wire.available()) {
      BPM_2 = Wire.read();
    }

    if (PULSE_SIGNAL > THRESHOLD) {
      TOUCHED_ONCE = 1;
    }

    if ((BPM_1 != 255 && BPM_1 > 1) && (BPM_2 != 255 && BPM_2 > 1)) {
      PULSES_BEFORE_AVERAGING++;
      Serial.println(PULSES_BEFORE_AVERAGING);
    }

    if (PULSES_BEFORE_AVERAGING > 600) {
      BPM_BLINK = 15;

      static uint8_t COLOR_INDEX = 0;
      COLOR_INDEX = COLOR_INDEX + 1;
      
      ShowPulseEffect(COLOR_INDEX);
      
      FastLED.show();
      FastLED.delay(BPM_BLINK);
    }

    if (PULSES_BEFORE_AVERAGING < 600 && TOUCHED_ONCE > 0) {
      if (BPM_1 > 18) {
        BPM_BLINK = 18;
      }
      if (BPM_1 < 18 || BPM_1 == 18) {
        BPM_BLINK = BPM_1;
      }
      
      static uint8_t COLOR_INDEX = 0;
      COLOR_INDEX = COLOR_INDEX + 1;
      
      ShowPulseEffect(COLOR_INDEX);
      
      FastLED.show();
      FastLED.delay(BPM_BLINK);
    }
}


// PALETTE FOR WAVE EFFECTS FROM PULSE
void SetupLEDColors()
{
    fill_solid(LED_COLOR_1, 16, CRGB::Black);
    LED_COLOR_1[0] = CRGB::DeepSkyBlue;
    LED_COLOR_1[4] = CRGB::DeepSkyBlue;
    LED_COLOR_1[8] = CRGB::DeepSkyBlue;
    LED_COLOR_1[12] = CRGB::DeepSkyBlue;  

    fill_solid(LED_COLOR_2, 16, CRGB::Black);
    LED_COLOR_2[0] = CRGB::Gold;
    LED_COLOR_2[4] = CRGB::Gold;
    LED_COLOR_2[8] = CRGB::Gold;
    LED_COLOR_2[12] = CRGB::Gold;
}


// FUNCTION THAT CONVERTS BPM TO MILISECONDS FOR ARDUINO DELAY
int ConvertBPMToMiliseconds(int BPM)
{
  return (60000 / BPM) / 30;
}


// LIGHT UP THE LED STRIPS WITH THEIR RESPECTIVE COLORS
void ShowPulseEffect(uint8_t COLOR_INDEX)
{
    for (int i = 0; i < NUM_LEDS_PER_STRIP; i++) {
      LED_STRIP_1[i] = ColorFromPalette(LED_COLOR_1, COLOR_INDEX, LED_BRIGHTNESS, COLOR_BLENDING);
      LED_STRIP_2[i] = ColorFromPalette(LED_COLOR_2, COLOR_INDEX, LED_BRIGHTNESS, COLOR_BLENDING);
      COLOR_INDEX += 1;
    }
}
