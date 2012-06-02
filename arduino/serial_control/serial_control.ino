//Standard PWM DC control
int E1 = 5;     //M1 Speed Control
int E2 = 6;     //M2 Speed Control
int M1 = 4;    //M1 Direction Control
int M2 = 7;    //M1 Direction Control
 
//Wheel Encoders
volatile int LeftCount = 0;
volatile int RightCount = 0;

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
void setup(void) 
{
  attachInterrupt(0, LeftEncoderEvent, CHANGE);
  attachInterrupt(1, RightEncoderEvent, CHANGE);
  
  int i;
  for(i=4;i<=7;i++)
    pinMode(i, OUTPUT);
  Serial.begin(19200);      //Set Baud Rate
  Serial.println("Run keyboard control");
}

void LeftEncoderEvent()
{
  LeftCount++;
}
 
void RightEncoderEvent()
{
  RightCount++;
}

void loop(void) 
{
  static unsigned long timer = 0;
  if(millis() - timer >= 500 && (LeftCount != 0 || RightCount != 0)){   
    Serial.print("Left Speed: ");
    Serial.println(LeftCount);
    Serial.print("Right Speed: ");
    Serial.println(RightCount);
    LeftCount = 0;
    RightCount = 0;
    timer = millis();
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
        turn_L (100,100);
        break;       
      case 'd'://Turn Right
        turn_R (100,100);
        break;
      case 'z':
        Serial.println("Hello");
        break;
      case 'x':
        stop();
        break;
      }
    }
    else stop();  
  }
}
