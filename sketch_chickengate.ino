
// constants won't change. Used here to set a pin number:
#define LED_PIN_UP         5
#define LED_PIN_DOWN       6
#define STEPPER_DIRECTION  7
#define STEPPER_PIN        8

#define END_POS_TOP        9
#define END_POS_BOTTOM    10
#define MANUAL_SWITCH     11

#define posUnknown  0
#define posBottom   1
#define posToTop    2
#define posTop      3
#define posToBottom 4

#define DOWN -1
#define UP    1

// Variables will change:
bool ledState = LOW;             // ledState used to set the LED
bool stepperState = LOW;
volatile unsigned int upBtnCnt   = 0;
volatile unsigned int downBtnCnt = 0;
volatile byte curPos = posUnknown;

unsigned long previousMillis = 0;        // will store last time LED was updated

// constants won't change:
const long interval = 500;           // interval at which to blink (milliseconds)

void setup() {
  // set the digital pin as output:
  pinMode(LED_BUILTIN,       OUTPUT);
  pinMode(LED_PIN_UP,        OUTPUT);
  pinMode(LED_PIN_DOWN,      OUTPUT);
  pinMode(STEPPER_DIRECTION, OUTPUT);
  pinMode(STEPPER_PIN,       OUTPUT);

  pinMode(END_POS_TOP,       INPUT_PULLUP);
  pinMode(END_POS_BOTTOM,    INPUT_PULLUP);
  pinMode(MANUAL_SWITCH,     INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(END_POS_TOP),    ISR_posTop,       FALLING);
  attachInterrupt(digitalPinToInterrupt(END_POS_TOP),    ISR_unknownPos,   LOW);
  attachInterrupt(digitalPinToInterrupt(END_POS_TOP),    ISR_unknownPos,   RISING);
  attachInterrupt(digitalPinToInterrupt(END_POS_TOP),    ISR_unknownPos,   HIGH);
  attachInterrupt(digitalPinToInterrupt(END_POS_BOTTOM), ISR_posBottom,    FALLING);
  attachInterrupt(digitalPinToInterrupt(END_POS_BOTTOM), ISR_unknownPos,   LOW);
  attachInterrupt(digitalPinToInterrupt(END_POS_BOTTOM), ISR_unknownPos,   RISING);
  attachInterrupt(digitalPinToInterrupt(END_POS_BOTTOM), ISR_unknownPos,   HIGH);
  attachInterrupt(digitalPinToInterrupt(MANUAL_SWITCH),  ISR_manualSwitch, LOW);
  
}

void ISR_posTop()       { curPos = posTop;     }
void ISR_posBottom()    { curPos = posBottom;  }
void ISR_unknownPos()   { curPos = posUnknown; }
void ISR_manualSwitch() { 
  switch (curPos){
    case posUnknown:
    case posTop:
    case posToTop:
      curPos = posToBottom;
      break;
    case posBottom:
    case posToBottom:
      curPos = posToTop;
      break;
  }
}

void gate(short dir){
  if(posUnknown == curPos){
    pinMode(STEPPER_DIRECTION, dir);
    unsigned int currentMillis = millis();
    if ( (currentMillis - previousMillis) >= 50) {
      stepperState = !stepperState;
      pinMode(STEPPER_PIN, stepperState);
    }
  }
}

void loop() {
  unsigned long currentMillis = millis();

  unsigned int lastBtnCnt = upBtnCnt; 

  if (currentMillis - previousMillis >= interval) {
    // save the last time you blinked the LED
    previousMillis = currentMillis;

    // if the LED is off turn it on and vice-versa:
    ledState = !ledState;

    switch(curPos) {
      case posUnknown:
        digitalWrite(LED_PIN_UP,    ledState);
        digitalWrite(LED_PIN_DOWN, !ledState);
        digitalWrite(LED_BUILTIN,   ledState);
        gate(DOWN);
        break;
      case posBottom:
        digitalWrite(LED_PIN_UP,    LOW);
        digitalWrite(LED_PIN_DOWN,  HIGH);
        digitalWrite(LED_BUILTIN,   LOW);
        break;
      case posToTop:
        digitalWrite(LED_PIN_UP,    ledState);
        digitalWrite(LED_PIN_DOWN,  LOW);
        digitalWrite(LED_BUILTIN,   ledState);
        gate(UP);
        break;
      case posTop:
        digitalWrite(LED_PIN_UP,    HIGH);
        digitalWrite(LED_PIN_DOWN,  LOW);
        digitalWrite(LED_BUILTIN,   LOW);
        break;
      case posToBottom:
        digitalWrite(LED_PIN_UP,    LOW);
        digitalWrite(LED_PIN_DOWN,  ledState);
        digitalWrite(LED_BUILTIN,   ledState);
        gate(DOWN);
        break;
    }
  }
}
