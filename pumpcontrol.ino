// todo: Timeout = 5 Minuten, nach jedem Button pressen timeout = 0, if timeout = 5 minuten, DISPLAY_ON = LOW
// README:
// Fehler 418: Teekessel hat zu wenig/zu viel Wasser, falscher Wert (passiert zb bei Wassertropfen am Ultraschall


#include <LiquidCrystal.h>
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

const int TRIG_TANK_PIN = 0;
const int ECHO_TANK_PIN = 1;
const int TRIGGER_PIN = 7;
const int ECHO_PIN = 6;
const int DISPLAY_PIN = 13;
const int PUMP1_PIN = 10;
const unsigned long SCAN_FREQ = 10UL * 1000UL;
const unsigned long DEBOUNCE = 1UL * 1000UL;
const unsigned long PUMP1_ON = 1UL * 60UL * 1000UL;

unsigned long scan_timer_ = 0;
unsigned long pump1_timer_ = 0;

int pump1_state = 0; //0 = automatik, 1 = manuell ein, 2 = manuell aus, 3 = automatik ein, 4 = automatik aus
int pump2_state = 0;

long dist_cm = 0;
long tank_level = 0;

void setup() {
  pinMode(TRIG_TANK_PIN, OUTPUT);
  pinMode(ECHO_TANK_PIN, INPUT);
  digitalWrite(ECHO_TANK_PIN, LOW);
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
  // Serial.begin(9600);
  // delay(1000);
}

void automatik() {                            // derzeit nur für PUMPE 1!!
  unsigned long timespan_pump1 = millis() - pump1_timer_;

  if (dist_cm >= 0 && dist_cm < 40) {       // wenn Distanz passt...
    get_Tank_level();
    if (tank_level > 8) {
      if (timespan_pump1 > PUMP1_ON) {        // ... und Zeit passt...
        if (pump1_state == 4 || pump1_state == 0) {   // und Pumpe vorher nicht an war oder per Hand gerade auf Automatik gestellt wurde...
          digitalWrite(PUMP1_PIN, LOW); // wird Pumpe eingeschaltet
          lcd.begin(16, 2);
          lcd.setCursor(0, 0);
          lcd.print("Pumpe 1 Aut ON  ");
          pump1_timer_ = millis();
          pump1_state = 1;
        }
      }
      else {
        digitalWrite(PUMP1_PIN, HIGH);      // Wenn Zeit und Distanz passen, aber PUMP1_ON Zeit vergangen ist, schalten wir aus.
        lcd.begin(16, 2);
        lcd.setCursor(0, 0);
        lcd.print("Pumpe 1 AUT OFF  ");
        pump1_timer_ = millis();
        pump1_state = 4;
      }
    }
    else if (tank_level <= 8) {
      lcd.begin(16,2);
      lcd.setCursor(0,0);
      lcd.print("Tank voll, Ablauf pruefen");
      digitalWrite(PUMP1_PIN, HIGH);
      pump1_state = 4;
    }
    else if (tank_level > 30) {
      lcd.begin(16,2);
      lcd.setCursor(0,0);
      lcd.print("Tank Sensor Failure");
      digitalWrite(PUMP1_PIN, HIGH);
      pump1_state = 4;
    }
  }
  else if (dist_cm > 40) {
    digitalWrite(PUMP1_PIN, HIGH);        // Wenn Distanz nicht passt, schalten wir aus. #
    lcd.setCursor(0, 0);
    lcd.print("Fehler 418      ");
  }
}

void get_Distance() {
  unsigned long time_span = millis() - scan_timer_;
  long duration = 0;
  long distance = 0;

  if (time_span > SCAN_FREQ) {
    digitalWrite(TRIGGER_PIN, LOW);
    digitalWrite(ECHO_PIN, LOW);
    delay(5);
    digitalWrite(TRIGGER_PIN, HIGH);
    delay(10);
    digitalWrite(TRIGGER_PIN, LOW);
    duration = pulseIn(ECHO_PIN, HIGH); //returns micros
    distance = ((duration / 2.) * 0.03432) + 0.5; // +0.5 zum Runden
    dist_cm = distance;

    lcd.setCursor(0, 1);
    lcd.print("Abstand: ");
    lcd.print(distance);
    lcd.print(" cm    ");
    scan_timer_ = millis();
  }
}

void get_Tank_level() {
  long duration = 0;
  long distance = 0;

  digitalWrite(TRIG_TANK_PIN, LOW);
  digitalWrite(ECHO_TANK_PIN, LOW);
  delay(5);
  digitalWrite(TRIG_TANK_PIN, HIGH);
  delay(10);
  digitalWrite(TRIG_TANK_PIN, LOW);
  duration = pulseIn(ECHO_TANK_PIN, HIGH);
  distance = ((duration / 2.) * 0.03432) + 0.5;

  tank_level = distance;
}

void loop() {
  unsigned long act_time = millis();

  //  Serial.println(button1_state);
  //  Serial.println(button2_state);


      if (pump1_state == 0) {
        lcd.setCursor(0, 0);
        lcd.print("Pumpe 1 an      ");
        //    Serial.println("Pumpe manuell ein");
        pump1_state = 1;
        digitalWrite(PUMP1_PIN, LOW);
      }
      else if (pump1_state == 1) {
        lcd.setCursor(0, 0);
        lcd.print("Pumpe 1 aus     ");
        //    Serial.println("Pumpe manuell aus");
        pump1_state = 2;
        digitalWrite(PUMP1_PIN, HIGH);
      }
      else if (pump1_state == 2) {
        lcd.setCursor(0, 0);
        lcd.print("Automatik 1 ein   ");
        //    Serial.println("Automatik 1 eingeschaltet");
        pump1_state = 0;
        automatik();
      }
    }
  }
  get_Distance();
  automatik();

}
