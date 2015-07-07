
#define PIN_DIRECTION 2
#define QUARTER_TURN 1024

void setup() 
{ 
  DDRC = 0xFF; // set all to output
  
  pinMode(PIN_DIRECTION, INPUT_PULLUP);
  
  //Serial.begin(9600);
} 

void loop()
{
  static byte active = 1;
  static int count = 0;
  static byte current_dir = 0;
  
  byte dir = digitalRead(PIN_DIRECTION);
  
  
  if (current_dir != dir) {
    current_dir = dir;
    count = 0; 
    active = 1;
  }
  
  if (active) {
    
    
    step(dir);
    
    //Serial.print("steps:" );
    //Serial.println(count);
    count++;
    
    if (count == QUARTER_TURN) {
      // stop
      PORTC = 0;
      active = 0;
    }
  }
  
  delay(1);
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

