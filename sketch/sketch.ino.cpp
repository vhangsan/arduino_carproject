#include "SR04.h"
#include "pitches.h"
#include "IRremote.h"
#define TRIG_PIN 12
#define LED_PIN 7
#define LATCH_PIN 3
#define CLOCK_PIN 4
#define DATA_PIN 2
#define ECHO_PIN 13
#define LEFT_TURN_SIGNAL 7
#define RIGHT_TURN_SIGNAL 8
int receiver = 11;
SR04 sr04 = SR04(ECHO_PIN,TRIG_PIN);
float distance;
float threshold = 25;
float maxDelay = 700;
float minDelay = 100;

int melody = NOTE_C5;
int duration = 2000;  // 500 miliseconds

IRrecv irrecv(receiver);  
decode_results results;  

byte Sequence[8] = {
  B00100000,
  B01100000,
  B11100000,
  B00000000,
  B00000100,
  B00000110,
  B00000111,
  B00000000,
};


void setup() {
  pinMode(LEFT_TURN_SIGNAL, INPUT_PULLUP);
  pinMode(RIGHT_TURN_SIGNAL, INPUT_PULLUP);
  pinMode(DATA_PIN, OUTPUT);
  pinMode(CLOCK_PIN, OUTPUT);
  pinMode(LATCH_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  irrecv.enableIRIn(); 
  Serial.begin(9600);
  delay(1000);
}


void leftBlinker_Sequence(int counter, bool left, bool right) {
  digitalWrite(LATCH_PIN, LOW);

  shiftOut(DATA_PIN, CLOCK_PIN, LSBFIRST, (Sequence[counter]*left)+(Sequence[counter+4]*right));  
 
  digitalWrite(LATCH_PIN, HIGH);
}


void loop() {
  distance=sr04.Distance();
  if (irrecv.decode(&results)) {
    Serial.print("Signal: ");
    Serial.println(results.value);
    irrecv.resume();
  }
  
  if (distance < threshold) {
    Serial.println(distance);
    digitalWrite(LATCH_PIN, LOW);
    shiftOut(DATA_PIN,  CLOCK_PIN, LSBFIRST, Sequence[3]);
    digitalWrite(LATCH_PIN, HIGH);
    delay(minDelay + (maxDelay * distance/threshold));
    digitalWrite(LATCH_PIN, LOW);
    shiftOut(DATA_PIN,  CLOCK_PIN, LSBFIRST, Sequence[2]+Sequence[6]);
    digitalWrite(LATCH_PIN, HIGH);
    delay(minDelay + (maxDelay * distance/threshold));
  }
  else {
    bool left = false;
    bool right = false;
    switch(results.value) {
      case 0xFFA857:
        left = true;
        right = true;
        break;
      case 0xFFE01F:
        left = true;
        break;
      case 0xFF906F:
        right = true;
        break;
      default:
        left = false;
        right = false;
    }
    for (int i = 0; i < 4; i++) {
      delay(500);
      leftBlinker_Sequence(i, left, right);
    }
    shiftOut(DATA_PIN, CLOCK_PIN, LSBFIRST, Sequence[0]);
  }
}