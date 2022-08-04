class non_blocking_timed_pin
{
  public:
      non_blocking_timed_pin()
      {}
        non_blocking_timed_pin(int pin, unsigned long time_span, const String& name): pin_(pin),time_span_(time_span), name_(name)
        {
        }
      bool digital_write_timed(unsigned int state_start, unsigned int state_end)
        {
          if(running_)
          {
              unsigned long diff = millis() -timer_;
              if(diff < time_span_){
                  return !running_;
              }
              digitalWrite(pin_, state_end);
              Serial.print(name_);
              Serial.print(name_);
              Serial.println(state_start);
              running_ = false;
              return !running_;
          }
          digitalWrite(pin_, state_start);
          Serial.print(name_);
          Serial.println(state_start);
          timer_ = millis();
          running_ = true;
          return !running_;
        };
  private:
        unsigned long timer_;
        bool running_ = false;
        
        unsigned int pin_;
        unsigned long time_span_;
        String name_;
};

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
const unsigned long NO_RAIN = 2UL * 60UL * 60UL * 1000UL;

unsigned long scan_timer_ = 0;
unsigned long pump_timer_ = 0;

unsigned long no_rain_timer_ = 0;

long distance_barrel = 0;
long last_distance_barrel = 0;
long distance_fish = 0;
long last_distance_fish = 0;

bool start_ventil_1_=true,start_ventil_2_=false,start_ventil_3_=false;
non_blocking_timed_pin ventil1_;
non_blocking_timed_pin ventil2_;
non_blocking_timed_pin ventil3_;

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
  
 ventil1_ = non_blocking_timed_pin(VALVE_1_PIN,VALVE_1_ON,"ventil 1");
 ventil2_ = non_blocking_timed_pin(VALVE_2_PIN,VALVE_2_ON,"ventil 2");
 ventil3_ = non_blocking_timed_pin(VALVE_2_PIN,VALVE_3_ON,"ventil 3");
  
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
  
  unsigned long time_span_pump = millis() - pump_timer_;
  unsigned long time_span_norain = millis() - no_rain_timer_;

  if (time_span < SCAN_FREQ) {
    return;
  }

  long distance_fish = get_Distance_Fish();
  long distance_barrel = get_Distance_Barrel();

  if (distance_fish < 12) { //die 12 unbedingt überprüfen sonst zieht es aus Versehen die ganze Zeit Strom
    if(start_ventil_1_)
    {
      start_ventil_2_ = ventil1_.digital_write_timed(HIGH,LOW);
      start_ventil_1_ = !start_ventil_2_;
    }else if(start_ventil_2_)
    {
      start_ventil_3_ = ventil2_.digital_write_timed(HIGH,LOW);
      start_ventil_2_ = !start_ventil_3_;
    }else if(start_ventil_3_)
    {
      start_ventil_1_ = ventil3_.digital_write_timed(HIGH,LOW);
      start_ventil_3_ = !start_ventil_1_;
    }

    if ((distance_barrel > 45) || (distance_fish < 8)) {
      return;
    }

    if (distance_barrel > last_distance_barrel) {       //wenn es regnet und aktuelle Messung mehr als letzte, wird die Pumpe für eine Minute eingeschaltet wenn sie aus war, und für eine Minute
      // ausgeschaltet wenn sie ein war
      switch_pump();
    }

    if (distance_barrel <= last_distance_barrel) {      // wenn es schon länger nicht geregnet hat, wird ein anderer Timer dazwischen geschaltet und nur jede 2. Stunde für eine Minute
      if (time_span_norain > NO_RAIN) {                 // eingeschaltet
        digitalWrite(PUMP_MAIN_PIN, LOW);
        delay(60000);
        digitalWrite(PUMP_MAIN_PIN, HIGH);
        no_rain_timer_ = millis();
      }
    }

    scan_timer_ = millis();
    last_distance_barrel = distance_barrel;
    last_distance_fish = distance_fish;
  }
}

void switch_pump() {
  unsigned long time_span_pump = millis() - pump_timer_;
  if(time_span_pump <= PUMP_MAIN_ON){
    return;
  }
  int pump_state = digitalRead(PUMP_MAIN_PIN);
  Serial.print("Pumpenstatus: ");
  Serial.println(pump_state);
 
    
  if (pump_state == HIGH) {
    digitalWrite(PUMP_MAIN_PIN, LOW);
    Serial.println("Pumpe ein");
  }
  if (pump_state == LOW) {
    // Pumpe lang genug gelaufen, ausschalten
    // rein nur zur Sicherheit auch eine Minute ausgeschaltet lassen
    // dafür verwenden wir den pump_timer_ noch einmal
    digitalWrite(PUMP_MAIN_PIN, HIGH);
    Serial.println("Pumpe aus");
  }
  pump_timer_ = millis();
}
