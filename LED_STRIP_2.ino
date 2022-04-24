#include <FastLED.h>
#define USE_ARDUINO_INTERRUPTS true 
#include <PulseSensorPlayground.h>
#include <Wire.h>


// DEFINING VARIABLES FOR LEDs
#define LED_TYPE             WS2811 // LED CHIPS WITH A DATA LINE, GROUND, AND POWER
#define NUM_LEDS_PER_STRIP   30
#define LED_BRIGHTNESS       255
#define COLOR_ORDER          GRB

#define LED_DIGITAL_PIN_1    10
#define LED_DIGITAL_PIN_2    12

CRGB LED_STRIP_1[NUM_LEDS_PER_STRIP];
CRGB LED_STRIP_2[NUM_LEDS_PER_STRIP];

CRGBPalette16 LED_COLOR_1;
CRGBPalette16 LED_COLOR_2;

TBlendType    COLOR_BLENDING;


// DEFINING VARIABLES FOR PULSE SENSOR
PulseSensorPlayground PULSE_SENSOR;

#define PULSE_ANALOG_PIN     2

int PULSE_SIGNAL;
int BPM_SIGNAL;
int THRESHOLD = 650;

byte BPM_1;
byte BPM_2 = 0;
int PULSES_BEFORE_AVERAGING = 0;
int BPM_BLINK = 0;
int TOUCHED_ONCE = 0;


// INITIAL SETUPS
void setup() {
    Wire.begin(5);
    Wire.onReceive(receiveEvent);
    Wire.onRequest(requestEvent);
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
    BPM_2 = BPM_SIGNAL;

    if (PULSE_SIGNAL > THRESHOLD) {
      TOUCHED_ONCE = 1;
    }
    
    if (BPM_1 == 244) {
      BPM_BLINK = 15;

      static uint8_t COLOR_INDEX = 0;
      COLOR_INDEX = COLOR_INDEX + 1;
      
      ShowPulseEffect(COLOR_INDEX);

      FastLED.show();
      FastLED.delay(BPM_BLINK);
    }

    if (BPM_1 != 244 && TOUCHED_ONCE > 0) {
      if (BPM_2 > 18) {
        BPM_BLINK = 18;
      }
      if (BPM_2 < 18 || BPM_2 == 18) {
        BPM_BLINK = BPM_2;
      }

      static uint8_t COLOR_INDEX = 0;
      COLOR_INDEX = COLOR_INDEX + 1;
      
      ShowPulseEffect(COLOR_INDEX);

      FastLED.show();
      FastLED.delay(BPM_BLINK);
    }
}


// RECEIVE BPM_1 DATA
void receiveEvent(int howMany) {
  while (Wire.available()) {
    BPM_1 = Wire.read();
  }
}


// SEND BPM_2 DATA
void requestEvent() {
  Wire.write(BPM_2);
}


// PALETTE FOR WAVE EFFECTS FROM PULSE
void SetupLEDColors()
{
    fill_solid(LED_COLOR_1, 16, CRGB::Black);
    LED_COLOR_1[0] = CRGB::HotPink;
    LED_COLOR_1[4] = CRGB::HotPink;
    LED_COLOR_1[8] = CRGB::HotPink;
    LED_COLOR_1[12] = CRGB::HotPink;

    fill_solid(LED_COLOR_2, 16, CRGB::Black);
    LED_COLOR_2[0] = CRGB::Red;
    LED_COLOR_2[4] = CRGB::Red;
    LED_COLOR_2[8] = CRGB::Red;
    LED_COLOR_2[12] = CRGB::Red;
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
