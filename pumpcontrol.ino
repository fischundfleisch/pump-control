// todo: Timeout = 5 Minuten, nach jedem Button pressen timeout = 0, if timeout = 5 minuten, DISPLAY_ON = LOW


#include <LiquidCrystal.h>
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

const int TRIGGER = 7;
const int ECHO = 6;
const int DISPLAY_ON = 13;
const int BUTTON_PIN = 8;
const int BUTTON_2 = 9;
const unsigned long SCAN_FREQ = 10UL*1000UL;
const unsigned long DISPLAY_SLEEP = 1UL * 60UL *1000UL;

unsigned long scan_timer_ =0;
unsigned long sleep_timer_ = 0;

int pump_state = 0; //0 = alles aus, 1 = manuell ein, 2 = automatik

void setup() {
  pinMode(DISPLAY_ON, OUTPUT);
  digitalWrite(DISPLAY_ON, HIGH);
  pinMode(TRIGGER, OUTPUT);
  pinMode(ECHO, INPUT);
  digitalWrite(ECHO, LOW);
  pinMode(BUTTON_PIN, INPUT);
  pinMode(BUTTON_2, INPUT);
  digitalWrite(BUTTON_2, HIGH);
  
  lcd.begin(16, 2);  //16 Zeichen, 2 Zeilen
  lcd.setCursor(0,0); //Position Zeichen 0, Zeile 0
  lcd.print("Willkommen!");
  Serial.begin(9600);
  delay(1000);
}

void loop() {
  unsigned long act_time = millis();
  unsigned long time_span = act_time - scan_timer_;
  unsigned long timespan_display = act_time - sleep_timer_;
  
  long duration = 0;
  long distance = 0;

  bool button_state = digitalRead(BUTTON_PIN);
Serial.println(button_state);
  if (button_state == LOW && pump_state ==0) {
//    lcd.display();
//    digitalWrite(DISPLAY_ON, HIGH);
//    digitalWrite(12, HIGH);
    delay(1000);
    lcd.setCursor(0,0);
    lcd.print("Pumpe 1 an      ");
    sleep_timer_ = millis();
    Serial.println("Pumpe manuell ein");
    pump_state = 1;
  }
  else if (button_state == LOW && pump_state == 1){
//    lcd.display();
//    digitalWrite(12, HIGH);
//    digitalWrite(DISPLAY_ON, HIGH);
    lcd.setCursor(0,0);
    lcd.print("Pumpe 1 aus     ");
    sleep_timer_ = millis();
    Serial.println("Pumpe manuell aus");
    pump_state = 2;
  }
  else if (button_state == LOW && pump_state ==2){
    lcd.setCursor(0,0);
    lcd.print("Automatik ein   ");
    Serial.println("Automatik eingeschaltet");
    pump_state = 0;
  }

  if (timespan_display > DISPLAY_SLEEP){
    lcd.noDisplay();

    digitalWrite(12, LOW);
    digitalWrite(DISPLAY_ON, LOW);  //schaltet Display ab
  }

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
    scan_timer_ = millis();
  }

  }
