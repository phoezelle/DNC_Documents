//Function: This procedure applies Arduino driver LCD1602
//Platform:ARDUINO MEGA 2560
//Time:August 28,2012
int DI = 12;
int RW = 11;
int DB[] = {3, 4, 5, 6, 7, 8, 9, 10};//Pin array to define the bus need
int Enable = 2;
char string_1[]="welcome to chip_";
char string_2[]="partner store   ";
char string_3[]="http://stores.eb";
char string_4[]="ner-Store       ";

 
void LcdCommandWrite(int value) 
{
 //Define all pins
 int i = 0;
 for (i=DB[0]; i <= DI; i++) //Bus assignment
{
   digitalWrite(i,value & 01);//Because the The 1602 LCD signal recognition D7-D0 (not D0-D7), here is used to reverse the signal.
   value >>= 1;
 }
 digitalWrite(Enable,LOW);
 delayMicroseconds(1);
 digitalWrite(Enable,HIGH);
 delayMicroseconds(1);  // 
 digitalWrite(Enable,LOW);
 delayMicroseconds(1);  // 
}
 
void LcdDataWrite(int value) {
 // 
 int i = 0;
 digitalWrite(DI, HIGH);
 digitalWrite(RW, LOW);
 for (i=DB[0]; i <= DB[7]; i++) {
   digitalWrite(i,value & 01);
   value >>= 1;
 }
 digitalWrite(Enable,LOW);
 delayMicroseconds(1);
 digitalWrite(Enable,HIGH);
 delayMicroseconds(1);
 digitalWrite(Enable,LOW);
 delayMicroseconds(1);  // 
}

void setup (void) {
 int i = 0;
 for (i=Enable; i <= DI; i++) {
   pinMode(i,OUTPUT);
 }
 delay(500);

 // After a brief pause initialize the LCD
 // 
 LcdCommandWrite(0x38);  //                     
 delay(64);                      
 LcdCommandWrite(0x38);  //                       
 delay(50);                      
 LcdCommandWrite(0x38);  //                       
 delay(20);                      
 LcdCommandWrite(0x06);  //
 delay(20);                      
 LcdCommandWrite(0x0E);  // 
                         // 
 delay(20);                      
 LcdCommandWrite(0x01);  // 
 delay(100);                      
 LcdCommandWrite(0x80);  //
                         // 
 delay(500);                      
}
 
void loop (void) 
{
  LcdCommandWrite(0x80);
  delay(10);
  LcdDataWrite(43);
  
  
  while(1){int i=0;}
  /*
  int i;
  delay(); 
  LcdCommandWrite(0x80); 
  delay(10);                     
  for(i=0;i<16;i++)
    LcdDataWrite(string_1[i]);
  delay(10);
  LcdCommandWrite(0xc0);  // 
  delay(10); 
  for(i=0;i<16;i++)
    LcdDataWrite(string_2[i]);
  delay(5000);*/
}
