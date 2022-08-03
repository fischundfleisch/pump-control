const int TRIGGER_FISH_PIN = 2;
const int ECHO_FISH_PIN = 3;
const int TRIGGER_BARREL_PIN = 4;
const int ECHO_BARREL_PIN = 5;

const int PUMP_MAIN_PIN = 6;
const int VALVE_1_PIN = 7; // geht zum Olivenbaum
const int VALVE_2_PIN = 8;
const int VALVE_3_PIN = 9;

const unsigned long SCAN_FREQ = 10UL * 1000UL; // sec * millis = 10 sec
const unsigned long PUMP_MAIN_ON = 60UL * 1000UL;
const unsigned long VALVE_1_ON = 60UL * 1000UL;
const unsigned long VALVE_2_ON = 60UL * 1000UL;
const unsigned long VALVE_3_ON = 60UL * 1000UL;

unsigned long scan_timer_ = 0;
unsigned long pump_timer_ = 0;
unsigned long valve_1_timer_ = 0;
unsigned long valve_2_timer_ = 0;
unsigned long valve_3_timer_ = 0;

long distance_barrel = 0;
long last_distance_barrel = 0;
long distance_fish = 0;
long last_distance_fish = 0;

void setup() {

  Serial.begin(9600);
  Serial.println("booting...");

  pinMode(TRIGGER_FISH_PIN, OUTPUT);
  pinMode(ECHO_FISH_PIN, INPUT);
  pinMode(TRIGGER_BARREL_PIN, OUTPUT);
  pinMode(ECHO_BARREL_PIN, INPUT);

  pinMode(PUMP_MAIN_PIN, OUTPUT);
  digitalWrite(PUMP_MAIN_PIN, HIGH);
  pinMode(VALVE_1_PIN, OUTPUT);
  digitalWrite(VALVE_1_PIN, HIGH);
  pinMode(VALVE_2_PIN, OUTPUT);
  digitalWrite(VALVE_2_PIN, HIGH);
  pinMode(VALVE_3_PIN, OUTPUT);
  digitalWrite(VALVE_3_PIN, HIGH);

  Serial.println("booting complete");
  delay(1000);
}

long get_Distance_Fish() {
  long duration = 0;
  long distance = 0;

  digitalWrite(TRIGGER_FISH_PIN, LOW);
  digitalWrite(ECHO_FISH_PIN, LOW);
  delay(5);
  digitalWrite(TRIGGER_FISH_PIN, HIGH);
  delay(10);
  digitalWrite(TRIGGER_FISH_PIN, LOW);
  duration = pulseIn(ECHO_FISH_PIN, HIGH);
  distance = ((duration / 2.) * 0.03432) + 0, 5;
  return distance;
}

long get_Distance_Barrel() {
  long duration = 0;
  long distance = 0;

  digitalWrite(TRIGGER_BARREL_PIN, LOW);
  digitalWrite(ECHO_BARREL_PIN, LOW);
  delay(5);
  digitalWrite(TRIGGER_BARREL_PIN, HIGH);
  delay(10);
  digitalWrite(TRIGGER_BARREL_PIN, LOW);
  duration = pulseIn(ECHO_BARREL_PIN, HIGH);
  distance = ((duration / 2.) * 0.03432) + 0, 5;
  return distance;
}

void loop() {
  
  unsigned long time_span = millis() - scan_timer_;
  unsigned long time_span_v1 = millis() - valve_1_timer_;
  unsigned long time_span_v2 = millis() - valve_2_timer_;
  unsigned long time_span_v3 = millis() - valve_3_timer_;
  unsigned long time_span_pump = millis() - pump_timer_;
  
  if (time_span < SCAN_FREQ) {
    return;
  }
  
    long distance_fish = get_Distance_Fish();
    long distance_barrel = get_Distance_Barrel();

    if (distance_barrel < 12) { //die 12 unbedingt überprüfen sonst zieht es aus Versehen die ganze Zeit Strom
      int state_valve_1 = digitalRead(VALVE_1_PIN);
      int state_valve_2 = digitalRead(VALVE_2_PIN);
      int state_valve_3 = digitalRead(VALVE_3_PIN);

      if ((state_valve_1 == HIGH) && (state_valve_2 == HIGH) && (state_valve_3 == HIGH)) { // wenn alle Ventile bis dato ausgeschaltet sind
        // wir fangen beim ersten Ventil an, lassen es ein bis Timer abgelaufen und schalten dann zwei und drei
        digitalWrite(VALVE_1_PIN, LOW);
        valve_1_timer_ = millis();
        Serial.println("Ventil 1 ein");
      }
      if (state_valve_1 == LOW) {
        if (time_span_v1 > VALVE_1_ON) { // Von Ventil 1 ist die Zeit abgelaufen
          digitalWrite(VALVE_1_PIN, HIGH);
          digitalWrite(VALVE_2_PIN, LOW);
          valve_2_timer_ = millis();
          Serial.println("Ventil 1 aus");
          Serial.println("Ventil 2 ein");
        }
      }
      if (state_valve_2 == LOW) {
        if (time_span_v2 > VALVE_2_ON) { //Von Ventil 2 ist die Zeit abgelaufen
          digitalWrite(VALVE_2_PIN, HIGH);
          digitalWrite(VALVE_3_PIN, LOW);
          valve_3_timer_ = millis();
          Serial.println("Ventil 2 aus");
          Serial.println("Ventil 3 ein");
        }
      }
      if (state_valve_3 == LOW) {
        if (time_span_v3 > VALVE_3_ON) {
          digitalWrite(VALVE_3_PIN, HIGH);
          Serial.println("Ventil 3 aus");
        }
      }


      if ((distance_barrel > 45) || (distance_fish < 8)) {
        return;
      }

      if (distance_barrel >= last_distance_barrel) {
        int pump_state = digitalRead(PUMP_MAIN_PIN);
        Serial.print("Pumpenstatus: ");
        Serial.println(pump_state);

        if ((pump_state == HIGH) && (time_span_pump > PUMP_MAIN_ON)) {
          digitalWrite(PUMP_MAIN_PIN, LOW);
          pump_timer_ = millis();
          Serial.println("Pumpe ein");
        }
        if ((pump_state == LOW) && (time_span_pump > PUMP_MAIN_ON)) {
          // Pumpe lang genug gelaufen, ausschalten
          // rein nur zur Sicherheit auch eine Minute ausgeschaltet lassen
          // dafür verwenden wir den pump_timer_ noch einmal

          digitalWrite(PUMP_MAIN_PIN, HIGH);
          pump_timer_ = millis();
        }
      }

      scan_timer_ = millis();
      last_distance_barrel = distance_barrel;
      last_distance_fish = distance_fish;
    }
}
