// Relay control for go cart
// Relay ON is for no throttle and relay OFF is for throttle > 0

#include <Servo.h> 
Servo myservo;  // create servo object to control a servo 

int sig_in_pin = A0; //pwn in pin
int relay_pin = 4; //relay out pin
int servo_pin = 9;

int max_throttle = 1834; //min throttle in microseconds
int min_throttle = 1108;
int range_throttle = max_throttle - min_throttle;

int switch_threshold = 3; // trying to avoid some noise at 0
int no_sig_count; //number of timeouts on reading the pwm signal
int pwm_frame_timeout = 27000; //27 millis(full pwm frame plus some)
float range_factor = 180.0 / (float) range_throttle;

void setup() {
  pinMode(sig_in_pin, INPUT);
  pinMode(relay_pin, OUTPUT);
  myservo.attach(servo_pin);  // attaches the servo on pin 9 to the servo object 
  
  Serial.begin(9600);

  // no gas
  digitalWrite(relay_pin, HIGH);
  
  no_sig_count = 0;
}

void loop() {
  //read pwm
  int pwm_time_input = pulseIn(sig_in_pin, HIGH,pwm_frame_timeout);

  if(pwm_time_input > 0){ //no timeout
      no_sig_count = 0; //reset signal lost counter

      int pwm_time = pwm_time_input;
      
      Serial.print (pwm_time);
      Serial.print (" => ");
  
      // bound the values between min_throttle and max_throttle
      if (pwm_time < min_throttle) pwm_time = min_throttle;
      if (pwm_time > max_throttle) pwm_time = max_throttle;

      // map the range of the throttle to 0 to 180
      // get it to from 0 to max_throttle and then map it from 0 to 180
      pwm_time -= min_throttle;
      pwm_time = (float) (pwm_time) * range_factor;

      // throttle down is 180, up is 0
      // reverse it so down is 0 and up is 180
      pwm_time = 180 - pwm_time;

      Serial.print (pwm_time);
      
      myservo.write(pwm_time);         // tell servo to go to position in variable 'pos' 
      delay(15);                       // waits 15ms for the servo to reach the position  
    
      if(pwm_time > switch_threshold) 
         {
             digitalWrite(relay_pin, LOW);
             Serial.println(" gas");
       } 
       else 
       {
            // throttle off 
            digitalWrite(relay_pin, HIGH);
            Serial.println(" no gas");
       }
  }
  else{ //timeout 
    no_sig_count++;
    Serial.println("pwm_input = 0");
    if(no_sig_count > 6){ //no pwm signal for 6 frames
      //Tell system to cut throttle
      digitalWrite(relay_pin, HIGH);
    }
  }
}
