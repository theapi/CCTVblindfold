
void setup() 
{ 
  DDRC = 0xFF; // set all to output
} 

void loop()
{
  step(1);
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

