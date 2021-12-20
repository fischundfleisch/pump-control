// todo: Timeout = 5 Minuten, nach jedem Button pressen timeout = 0, if timeout = 5 minuten, DISPLAY_ON = LOW


#include <LiquidCrystal.h>
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

const int TRIGGER = 7;
const int ECHO = 6;
const int DISPLAY_ON = 13;
const unsigned long SCAN_FREQ = 10UL*1000UL;

unsigned long scan_timer_ =0;

void setup() {
  pinMode(DISPLAY_ON, OUTPUT);
  digitalWrite(DISPLAY_ON, HIGH);
  pinMode(TRIGGER, OUTPUT);
  pinMode(ECHO, INPUT);
  digitalWrite(ECHO, LOW);
  
  lcd.begin(16, 2);  //16 Zeichen, 2 Zeilen
  lcd.setCursor(0,0); //Position Zeichen 0, Zeile 0
  lcd.print("Willkommen!");
  Serial.begin(9600);
  delay(2000);
}

void loop() {
  unsigned long act_time = millis();
  unsigned long time_span = act_time - scan_timer_;
  long duration = 0;
  long distance = 0;

  if (time_span > SCAN_FREQ) {
    digitalWrite(TRIGGER, LOW);
    delay(5);
    digitalWrite(TRIGGER, HIGH);
    delay(10);
    digitalWrite(TRIGGER, LOW);
    duration = pulseIn(ECHO, HIGH); //returns micros
    distance = ((duration/2.) *0.03432)+0.5; // +0.5 zum Runden
    lcd.setCursor(0,0);
    lcd.print("Entfernung: ");
    lcd.setCursor(0,1);
    lcd.print(distance);
    lcd.print(" cm");
  }

  }
