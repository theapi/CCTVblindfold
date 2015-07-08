
#define PIN_TOGGLE 2 // Used to switch between open & closed
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

void setup() 
{ 
  DDRC = 0xFF; // set all to output
  
  pinMode(PIN_TOGGLE, INPUT_PULLUP);
  
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
      // Move to the closed position
      
      
      if (step_count >= QUARTER_TURN) {
        // stop
        PORTC = 0;
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
      // for now pretend calibrated and set to closed
      blindfold_state = B_CLOSED;
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
  // Switch is pulled high so active low.
  return !digitalRead(PIN_TOGGLE);
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

