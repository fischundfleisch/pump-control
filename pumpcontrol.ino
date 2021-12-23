// todo: Timeout = 5 Minuten, nach jedem Button pressen timeout = 0, if timeout = 5 minuten, DISPLAY_ON = LOW


#include <LiquidCrystal.h>
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

const int TRIGGER_PIN = 7;
const int ECHO_PIN = 6;
const int DISPLAY_PIN = 13;
const int BUTTON1_PIN = 8;
const int BUTTON2_PIN = 9;
const unsigned long SCAN_FREQ = 10UL * 1000UL;
const unsigned long DEBOUNCE = 1UL * 1000UL;

unsigned long scan_timer_ = 0;
unsigned long button1_timer_ = 0;
unsigned long button2_timer_ =0;

int pump1_state = 0; //0 = automatik, 1 = manuell ein, 2 = manuell aus
int pump2_state = 0;

void setup() {
  pinMode(TRIGGER_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  digitalWrite(ECHO_PIN, LOW);
  pinMode(DISPLAY_PIN, OUTPUT);
  digitalWrite(DISPLAY_PIN, HIGH);
  pinMode(BUTTON1_PIN, INPUT);
  pinMode(BUTTON2_PIN, INPUT);

  lcd.begin(16, 2);  //16 Zeichen, 2 Zeilen
  lcd.setCursor(0, 0); //Position Zeichen 0, Zeile 0
  lcd.print("Willkommen!");
  Serial.begin(9600);
  delay(1000);
}

void loop() {
  unsigned long act_time = millis();
  unsigned long time_span = act_time - scan_timer_;
  unsigned long timespan_button1 = act_time - button1_timer_;
  unsigned long timespan_button2 = act_time - button2_timer_;

  long duration = 0;
  long distance = 0;

  bool button1_state = digitalRead(BUTTON1_PIN);
  bool button2_state = digitalRead(BUTTON2_PIN);

  if (timespan_button1 > DEBOUNCE) {
    if (button1_state == LOW) {
      if (pump1_state == 0) {
        lcd.setCursor(0, 0);
        lcd.print("Pumpe 1 an      ");
        Serial.println("Pumpe manuell ein");
        pump1_state = 1;
        button1_timer_ = millis();
      }
      else if (pump1_state == 1) {
        lcd.setCursor(0, 0);
        lcd.print("Pumpe 1 aus     ");
        Serial.println("Pumpe manuell aus");
        pump1_state = 2;
        button1_timer_ = millis();
      }
      else if (pump1_state == 2) {
        lcd.setCursor(0, 0);
        lcd.print("Automatik 1 ein   ");
        Serial.println("Automatik 1 eingeschaltet");
        pump1_state = 0;
        button1_timer_ = millis();
      }
    }
  }

    if (timespan_button2 > DEBOUNCE) {
    if (button2_state == LOW) {
      if (pump2_state == 0) {
        lcd.setCursor(0, 0);
        lcd.print("Pumpe 2 an      ");
        Serial.println("Pumpe manuell ein");
        pump2_state = 1;
        button2_timer_ = millis();
      }
      else if (pump2_state == 1) {
        lcd.setCursor(0, 0);
        lcd.print("Pumpe 2 aus     ");
        Serial.println("Pumpe manuell aus");
        pump2_state = 2;
        button2_timer_ = millis();
      }
      else if (pump2_state == 2) {
        lcd.setCursor(0, 0);
        lcd.print("Automatik 2 ein ");
        Serial.println("Automatik 2 eingeschaltet");
        pump2_state = 0;
        button2_timer_ = millis();
      }
    }
  }


  if (time_span > SCAN_FREQ) {
    digitalWrite(TRIGGER_PIN, LOW);
    delay(5);
    digitalWrite(TRIGGER_PIN, HIGH);
    delay(10);
    digitalWrite(TRIGGER_PIN, LOW);
    duration = pulseIn(ECHO_PIN, HIGH); //returns micros
    distance = ((duration / 2.) * 0.03432) + 0.5; // +0.5 zum Runden
    lcd.setCursor(0, 0);
    lcd.print("Entfernung: ");
    lcd.setCursor(0, 1);
    lcd.print(distance);
    lcd.print(" cm");
    scan_timer_ = millis();
  }

}
