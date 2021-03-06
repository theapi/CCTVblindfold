
// http://playground.arduino.cc/Main/CapacitiveSensor
#include <CapacitiveSensor.h>


#define PIN_POWER_CAM     2 // mosfet driver to switch power to the camera
#define PIN_IR_LED        3 // the led of the reflective optical sensor - TCRT5000
#define PIN_IR_TRANSISTOR 4 // the phototransistor of the TCRT5000
#define PIN_TOGGLE_SEND   5 // Used to switch between open & closed (capactive touch)
#define PIN_TOGGLE_REC    6 // the receive pin for capactive touch
#define PIN_DEBUG         13

#define QUARTER_TURN 1024 // How many steps to to turn 90 degrees

// The timer states.
enum blindfold_states {
  B_OPEN,
  B_OPENING,
  B_CLOSED,
  B_CLOSING,
  B_CALIBRATING,
  B_TRIM_CW,
  B_TRIM_CCW
};
blindfold_states blindfold_state = B_CALIBRATING;

CapacitiveSensor capSensor = CapacitiveSensor(PIN_TOGGLE_SEND, PIN_TOGGLE_REC);


void setup() 
{ 
  DDRC = 0xFF; // set all to output
  
  pinMode(PIN_IR_TRANSISTOR, INPUT_PULLUP);
  
  pinMode(PIN_POWER_CAM, OUTPUT);
  pinMode(PIN_IR_LED, OUTPUT);
  pinMode(PIN_DEBUG, OUTPUT);
  
  digitalWrite(PIN_POWER_CAM, LOW); // on (npn driving p-channel mosfet)
  digitalWrite(PIN_DEBUG, LOW); // off
  digitalWrite(PIN_IR_LED, LOW); // off
  
  //Serial.begin(9600);
} 

void stateRun() 
{
  static int step_count = 0;
  unsigned long now = millis();
  static unsigned long step_last = 0;
  
  switch (blindfold_state) {
    case B_OPEN: 
    
    //Serial.print("steps:" ); Serial.println(step_count);
    
      // Monitor the toggle switch.
      if (toggled()) {
        step_count = 0;
        blindfold_state = B_CLOSING;
      }
      break;
      
    case B_OPENING:
      // Move to the open position
      if (step_count >= QUARTER_TURN) {
        // stop
        PORTC = 0;
        // power up the camera
        digitalWrite(PIN_POWER_CAM, LOW);
        // set the new state
        blindfold_state = B_OPEN;
      }
      else {
        // Must give time for the motor to move
        if (now - step_last > 1) {
          step_last = now;
          step(1);
          step_count++;
        }
      }
      break;
    
    case B_CLOSED:
      // Monitor the toggle switch.
      if (toggled()) {
        step_count = 0;
        blindfold_state = B_OPENING;
      }
      break;
    
    case B_CLOSING:
      // power down the camera
      digitalWrite(PIN_POWER_CAM, HIGH);
        
      // Move to the closed position
      if (step_count >= QUARTER_TURN) {
        // stop
        PORTC = 0;
        
        // set the new state
        blindfold_state = B_CLOSED;
      } else {
        // Must give time for the motor to move
        if (now - step_last > 1) {
          step_last = now;
          step(0);
          step_count++;
        } 
      }
      break;
    
    case B_CALIBRATING:
      // turn on the ir reflector
      digitalWrite(PIN_IR_LED, HIGH);
    
      // move to find the ir response.
      if (now - step_last > 2) {
        step_last = now;
        if (digitalRead(PIN_IR_TRANSISTOR)) {
          digitalWrite(PIN_DEBUG, HIGH);
          step(1);
        } else {
          // stop
          PORTC = 0;
          blindfold_state = B_OPEN;
          // turn off the ir reflector
          digitalWrite(PIN_IR_LED, LOW);
          digitalWrite(PIN_DEBUG, LOW); // Reflection seen
        }
      }
      break;
    
    case  B_TRIM_CW:
    
      break;
    
    case B_TRIM_CCW:
      
      break;
    
    default:
    
      break;
  }
}

/**
 * Whether the toggle switch is pushed
 */
byte toggled() 
{
  // threshold for toggling the switch
  static int threshold = 1000;

  long sensorValue = capSensor.capacitiveSensor(30);
  if (sensorValue > threshold) {
    return 1;
  }
  
  return 0;
}

void loop()
{
  stateRun();
}

void step(byte dir)
{
  static char i = 0;
  static byte steps[8] = {
    0b00000001,
    0b00000011,
    0b00000010,
    0b00000110,
    0b00000100,
    0b00001100,
    0b00001000,
    0b00001001,
  };
  // Set all the pins at the same time
  PORTC = steps[i];
  
  if (dir) { 
    i++; 
  } else { 
    i--; 
  } 
  
  if (i > 7) { 
    i = 0; 
  } else if (i < 0) { 
    i = 7; 
  } 
}

