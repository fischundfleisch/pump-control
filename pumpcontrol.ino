// HILFE! Fehler 418 in Zeile 71 überschreibt auch den Button-Druck. Temporary Workaround: Etwas im richtigen Abstand unter den Ultraschall halten um zu pumpen
// README: 
// Fehler 418: Teekessel hat zu wenig/zu viel Wasser, falscher Wert (passiert zb bei Wassertropfen am Ultraschall


#include <LiquidCrystal.h>
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

const int TRIGGER_PIN = 7;
const int ECHO_PIN = 6;
const int DISPLAY_PIN = 13;
const int BUTTON1_PIN = 8;
const int BUTTON2_PIN = 9;
const int PUMP1_PIN = 10;
const unsigned long SCAN_FREQ = 10UL * 1000UL;
const unsigned long DEBOUNCE = 1UL * 1000UL;
const unsigned long PUMP1_ON = 1UL * 60UL * 1000UL;

unsigned long scan_timer_ = 0;
unsigned long button1_timer_ = 0;
unsigned long button2_timer_ = 0;
unsigned long pump1_timer_ = 0;

int pump1_state_ = 0; //0 = automatik, 1 = manuell ein, 2 = manuell aus, 3 = automatik ein, 4 = automatik aus
int pump2_state_ = 0;


void setup() {
  pinMode(TRIGGER_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  digitalWrite(ECHO_PIN, LOW);
  pinMode(DISPLAY_PIN, OUTPUT);
  digitalWrite(DISPLAY_PIN, HIGH);
  pinMode(BUTTON1_PIN, INPUT);
  pinMode(BUTTON2_PIN, INPUT);
  pinMode(PUMP1_PIN, OUTPUT);
  digitalWrite(PUMP1_PIN, HIGH);    // fürs Relais muss HIGH für aus, LOW für an sein

  lcd.begin(16, 2);  //16 Zeichen, 2 Zeilen
  lcd.setCursor(0, 0); //Position Zeichen 0, Zeile 0
  lcd.print("Willkommen!");
  Serial.begin(9600);
  delay(1000);
}

void automatik(long distance_cm) {                            // derzeit nur für PUMPE 1!!
  unsigned long timespan_pump1 = millis() - pump1_timer_;

  if (dist_cm >= 2 && dist_cm < 50) {       // wenn Distanz passt...
    if (timespan_pump1 > PUMP1_ON) {        // ... und Zeit passt...
      if (pump1_state == 4 || pump1_state == 0) {   // und Pumpe vorher nicht an war oder per Hand gerade auf Automatik gestellt wurde...
        digitalWrite(PUMP1_PIN, LOW);             // wird Pumpe eingeschaltet
        lcd.setCursor(0,0);
        lcd.print("Pumpe 1 Aut ON  ");
        pump1_timer_ = millis();
        pump1_state_ = 3;
      }
      else {
        digitalWrite(PUMP1_PIN, HIGH);      // Wenn Zeit und Distanz passen, aber PUMP1_ON Zeit vergangen ist, schalten wir aus.
        lcd.setCursor(0,0);
        lcd.print("Pumpe 1 AUT OFF  ");
        pump1_timer_ = millis();
        pump1_state_ = 4;
      }
    }
  }
  else {
    digitalWrite(PUMP1_PIN, HIGH);        // Wenn Distanz nicht passt, schalten wir aus. #
    lcd.setCursor(0,0);
    lcd.print("Fehler 418      ");
  }
}

void get_Distance() {
  unsigned long time_span = millis() - scan_timer_;
  long duration = 0;
  long distance = 0;

  if (time_span > SCAN_FREQ) {
    digitalWrite(TRIGGER_PIN, LOW);
    delay(5);
    digitalWrite(TRIGGER_PIN, HIGH);
    delay(10);
    digitalWrite(TRIGGER_PIN, LOW);
    duration = pulseIn(ECHO_PIN, HIGH); //returns micros
    distance = ((duration / 2.) * 0.03432) + 0.5; // +0.5 zum Runden

    lcd.setCursor(0, 1);
    lcd.print("Abstand: ");
    lcd.print(distance);
    lcd.print(" cm  ");
    scan_timer_ = millis();
  }
  
  return distance;      // distance? oder distance_cm?
}

void loop() {
  unsigned long act_time = millis();
  unsigned long timespan_button1 = act_time - button1_timer_;
  unsigned long timespan_button2 = act_time - button2_timer_;

  bool button1_state = digitalRead(BUTTON1_PIN);
  bool button2_state = digitalRead(BUTTON2_PIN);

  if (timespan_button1 > DEBOUNCE) {
    if (button1_state == LOW) {
      if (pump1_state == 0) {
        lcd.setCursor(0, 0);
        lcd.print("Pumpe 1 an      ");
        Serial.println("Pumpe manuell ein");
        pump1_state_ = 1;
        digitalWrite(PUMP1_PIN, LOW);
        button1_timer_ = millis();
      }
      else if (pump1_state == 1) {
        lcd.setCursor(0, 0);
        lcd.print("Pumpe 1 aus     ");
        Serial.println("Pumpe manuell aus");
        pump1_state_ = 2;
        digitalWrite(PUMP1_PIN, HIGH);
        button1_timer_ = millis();
      }
      else if (pump1_state == 2) {
        lcd.setCursor(0, 0);
        lcd.print("Automatik 1 ein   ");
        Serial.println("Automatik 1 eingeschaltet");
        pump1_state_ = 0;
        button1_timer_ = millis();
        automatik();
      }
    }
  }

  if (timespan_button2 > DEBOUNCE) {
    if (button2_state == LOW) {
      if (pump2_state_ == 0) {
        lcd.setCursor(0, 0);
        lcd.print("Pumpe 2 an      ");
        Serial.println("Pumpe manuell ein");
        pump2_state_ = 1;
        button2_timer_ = millis();
      }
      else if (pump2_state_ == 1) {
        lcd.setCursor(0, 0);
        lcd.print("Pumpe 2 aus     ");
        Serial.println("Pumpe manuell aus");
        pump2_state_ = 2;
        button2_timer_ = millis();
      }
      else if (pump2_state_ == 2) {
        lcd.setCursor(0, 0);
        lcd.print("Automatik 2 ein ");
        Serial.println("Automatik 2 eingeschaltet");
        pump2_state_ = 0;
        button2_timer_ = millis();
      }
    }
  }
  
 automatik(get_Distance());

}
