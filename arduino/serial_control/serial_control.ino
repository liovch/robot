//Standard PWM DC control
int E1 = 5;     //M1 Speed Control
int E2 = 6;     //M2 Speed Control
int M1 = 4;    //M1 Direction Control
int M2 = 7;    //M1 Direction Control
 
void stop(void)                    //Stop
{
  digitalWrite(E1,LOW);   
  digitalWrite(E2,LOW);      
}   
void advance(char a,char b)          //Move forward
{
  analogWrite (E1,a);      //PWM Speed Control
  digitalWrite(M1,HIGH);    
  analogWrite (E2,b);    
  digitalWrite(M2,HIGH);
}  
void back_off (char a,char b)          //Move backward
{
  analogWrite (E1,a);
  digitalWrite(M1,LOW);   
  analogWrite (E2,b);    
  digitalWrite(M2,LOW);
}
void turn_L (char a,char b)             //Turn Left
{
  analogWrite (E1,a);
  digitalWrite(M1,LOW);    
  analogWrite (E2,b);    
  digitalWrite(M2,HIGH);
}
void turn_R (char a,char b)             //Turn Right
{
  analogWrite (E1,a);
  digitalWrite(M1,HIGH);    
  analogWrite (E2,b);    
  digitalWrite(M2,LOW);
}

//Wheel Encoders
int WheelEncoderLeft = 2;
int WheelEncoderRight = 3;
volatile unsigned int _encoder_step_left = 0;
volatile unsigned int _encoder_step_right = 0;
volatile unsigned int _old_state_right = 0;
volatile unsigned int _old_state_left = 0;
volatile boolean isUpdateRequired = true;

void reinit_encoders()
{
  _encoder_step_left = _encoder_step_right = 0;
  _old_state_right=digitalRead(WheelEncoderRight);
  _old_state_left=digitalRead(WheelEncoderLeft);  
}

void read_encoders()
{
    if(get_encoder_step(WheelEncoderRight, _old_state_right))
    {
      _encoder_step_right++;
      isUpdateRequired = true;
    }
    
    if(get_encoder_step(WheelEncoderLeft, _old_state_left))
    {
      _encoder_step_left++;
      isUpdateRequired = true;
    }
}

boolean get_encoder_step(const int iEnc, volatile unsigned int &old_dr)
{
  int dr=digitalRead(iEnc);
  if(dr!=old_dr)
  {
    old_dr = dr;
    return true;
  }
  
  return false;
}

void setup(void) 
{
  pinMode(WheelEncoderLeft, INPUT);
  pinMode(WheelEncoderRight, INPUT);
  reinit_encoders();

  attachInterrupt(0, read_encoders, CHANGE);
  attachInterrupt(1, read_encoders, CHANGE);

  int i;
  for (i=4; i<=7; i++)
    pinMode(i, OUTPUT);
  Serial.begin(9600);      //Set Baud Rate
}

void writeInt(unsigned int value)
{
  Serial.write(value & 0xFF);
  Serial.write(value >> 8);
}

void loop(void) 
{
  if (isUpdateRequired) {
	isUpdateRequired = false; // TODO: Not sure how to achieve thread safeness here
        writeInt(_encoder_step_left);
        writeInt(_encoder_step_right);
  }

  if (Serial.available()) {
    char val = Serial.read();
    if(val != -1)
    {
      switch(val)
      {
      case 'w'://Move Forward
        advance (255,255);   //move forward in max speed
        break;
      case 's'://Move Backward
        back_off (127,127);   //move back at half the speed
        break;
      case 'a'://Turn Left
        turn_L (127,127);
        break;       
      case 'd'://Turn Right
        turn_R (127,127);
        break;
      case 'z':
        Serial.println("Hello");
        break;
      case 'x':
        stop();
        break;
      case 't': // Test
        back_off (127, 0); // Move only one wheel
        break;
      }
    }
    else stop();  
  }
}

