#include "pins_arduino.h"
#include "pins_arduino.h"
#include <LiquidCrystal595.h>

char buf [17];

LiquidCrystal595 lcd(6, 7, 8);

#include <Encoder.h>
Encoder rotary(2, 3);
long positionLeft  = -999;
long positionRight = -999;

// what to do with incoming data
volatile byte pos;
volatile byte command = 0;
volatile byte adress = 0;



//VALUE FOR PINOUT
#define T_LEDRVALUE 0
#define T_LEDVVALUE 1
#define T_LEDOKVALUE 2
#define T_INTERRUPT 3
//VALUE FOR PININ
#define T_PUSHA 4
#define T_PUSHB 5
#define T_PUSHROTARY 6
#define T_PUSHOK 7
#define T_REED 8
//VALUE FOR PININ ANALOG
#define T_FLOAT 9
//meta
#define T_COUNTVAL 10
#define T_TELECOMODE 11
#define T_BOARDCHECKFLOAT 12
//ledstrob
#define T_STROBLRSPEED 13
#define T_STROBLVSPEED 14
#define T_STROBLOKSPEED 15

#define T_STRING 16
#define T_INIT 17

//size of table
#define T_REGISTERSIZE 18

#define READCOMMAND 0x40
#define WRITECOMMANDVALUE 0xc0
#define WRITECOMMANDFADE 0x80
#define COMMANDMASK 0xC0

#define T_DECINPIN 4
#define T_DECALALOGPIN 9

#define T_MODEBASE 1


byte Value[T_REGISTERSIZE];
byte newValue[T_REGISTERSIZE];

byte outpin[4] = {A5, A5, A0, 4};

byte inpin[5] = {A2, A3, 5, A4, A1};

byte inpinanalog[1] = {A6};

long unsigned lastLRStrob;
byte strobLRStep;
long unsigned lastLVStrob;
byte strobLVStep;
long unsigned lastLOKStrob;
byte strobLOKStep;

long unsigned lastCheckInput;
int checkInputPeriod;

void setup (void) {
  Serial.begin(19200);
  clearRegister();
  initpin();
  initSPIslave();
  Serial.println("telec");
  checkInputPeriod = 100;
  waitforinit();
  Serial.println("init");
}

void initpin() {
  byte i = 0;
  for (i = 0; i < T_DECINPIN; i++) {
    pinMode(outpin[i], OUTPUT);
    updateValue(i);
  }

  for (i = T_DECINPIN; i < T_DECALALOGPIN; i++) {
    pinMode(inpin[i - T_DECINPIN], INPUT);
  }

  strobLRStep = 0;
  strobLVStep = 0;
  strobLOKStep = 0;

  pos = 0;   // buffer empty
  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
  lcd.print("  do not clean");
  lcd.setBackLight(1);
}

void clearRegister() {
  for (byte i = 0; i < T_REGISTERSIZE; i++) {
    Value[i] = 0;
    newValue[i] = 0;
  }
}

void initSPIslave() {
  // have to send on master in, *slave out*
  pinMode(MISO, OUTPUT);
  // turn on SPI in slave mode
  SPCR |= _BV(SPE);
  // turn on interrupts
  SPCR |= _BV(SPIE);
}

void waitforinit(){
  delay(500);
  digitalWrite(outpin[T_INTERRUPT], HIGH);
  while(command==0);
  digitalWrite(outpin[T_INTERRUPT], LOW);
  Value[T_INIT]=1;
  newValue[T_INIT]=1;
}


// SPI interrupt routine
ISR (SPI_STC_vect)
{
  byte c = SPDR;
  //Serial.println(c, HEX);

  if (command < 10) {
    //Serial.print("R add");
    adress = c & ~COMMANDMASK;
    //Serial.print(adress, HEX);
    //Serial.print(" com");
    command = c & COMMANDMASK;
    //Serial.println(command, HEX);
  }
  else {
    //valeur
    if (command == WRITECOMMANDVALUE) {
      //Serial.println ("wv");
      if (adress < T_STRING) {
        newValue[adress] = c;
        command = 1;
        SPDR = 0;
      } else {//case string received
        if (pos < sizeof buf) {
          buf [pos++] = c;
        }
        return;
      }


    }
    //renvoie la valeur enregistree
    if (command == READCOMMAND) {
      SPDR = Value[adress];
      command = 1;
      if (inputRange(adress)) {
        newValue[T_INTERRUPT] = 0;
        Value[T_INTERRUPT] = newValue[T_INTERRUPT];
        digitalWrite(outpin[T_INTERRUPT], LOW);
      }
      //Serial.print("r ");
      //Serial.println (Value[adress],DEC);
    }

  }


}  // end of interrupt service routine (ISR) SPI_STC_vect


//boucle principale

void loop (void) {

  // if SPI not active, clear current command
  if (digitalRead (SS) == HIGH) command = 0;

  //verification changement de valeur
  if (command == 0 && adress < T_STRING) {
    for (byte i = 0; i < T_REGISTERSIZE; i++) {
      if (Value[i] != newValue[i]) {
        if (outputRange(i)) updateValue(i); //cas led output
        if (inputRange(i)) updateInput(i);
        else {
          //cas autre valeur (sans fade)
          Value[i] = newValue[i];
          Serial.print(millis(), DEC);
          Serial.print(" new ");
          Serial.print(i, DEC);
          Serial.print(" - v ");
          Serial.println(Value[i], DEC);


          //cas particulier
          if (i == T_STROBLRSPEED)lastLRStrob = millis();
          if (i == T_STROBLRSPEED && Value[i] == 0) strobLRRoutine(1);
          if (i == T_STROBLVSPEED)lastLVStrob = millis();
          if (i == T_STROBLVSPEED && Value[i] == 0) strobLVRoutine(1);


        }
      }
    }
  }


  if (Value[T_STROBLRSPEED] > 0) strobLRRoutine(0);
  if (Value[T_STROBLVSPEED] > 0) strobLVRoutine(0);
  //if (Value[T_STROBLVSPEED] > 0) strobLVRoutine();
  checkStringReceive();
  //checkInput();

}  // end of loop

void checkStringReceive() {
  if (command == 0 && adress == T_STRING) {
    buf [pos] = 0;
    Serial.println (buf);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(buf);
    pos = 0;
    adress = 0;
  }
}



bool inputRange(byte i) {
  if (i >= T_DECINPIN && i < T_DECALALOGPIN + 1) return true;
  return false;
}

bool outputRange(byte i) {
  if (i < T_DECINPIN - 1) return true;
  return false;
}


//fonction pour generer une interuption sur le rpi
void updateInput(byte i) {
  if (Value[T_INTERRUPT] == 0) {
    Value[i] = newValue[i];
    newValue[T_INTERRUPT] = i;
    Value[T_INTERRUPT] = newValue[T_INTERRUPT];
    Serial.println("interupt");
    digitalWrite(outpin[T_INTERRUPT], HIGH);
    SPDR = i;
  }
}



void checkInput() {
  if (millis() > lastCheckInput + checkInputPeriod) {
    //boutons
    for (byte i = 0; i < T_DECALALOGPIN - T_DECINPIN; i++) {
      newValue[T_DECINPIN + i] = 1 - digitalRead(inpin[i]);
      Serial.print("b");
      Serial.println(i, DEC);
    }
    if (Value[T_BOARDCHECKFLOAT] == 1) newValue[T_FLOAT] = map(analogRead(inpinanalog[T_FLOAT - T_DECALALOGPIN]), 0, 1024, 0, 255);
    lastCheckInput = millis();
  }
}



//courbe pour adapter la luminosite

byte lightfunc(byte v) {
  if (v > 0)return HIGH;
  return LOW;
}

//gestion du changement de valeur sortie LED RVB et led 10W1

void updateValue(byte i) {
  Value[i] = newValue[i];
  /*Serial.print(millis(), DEC);
  Serial.print(" new ");
  Serial.print(i, DEC);
  Serial.print(" - v ");
  Serial.println(Value[i], DEC);*/
  digitalWrite(outpin[i], lightfunc(Value[i]));
}


//stob sur la sortie LED R, LED V, LEDOK

void strobLRRoutine(byte force) {
  if (millis() > lastLRStrob + (10L * Value[T_STROBLRSPEED]) || force) {
    strobLRStep = (strobLRStep + 1) % 2;
    if (force)strobLRStep = 1;
    digitalWrite(outpin[T_LEDRVALUE], lightfunc(Value[T_LEDRVALUE]*strobLRStep));
    lastLRStrob += 10L * Value[T_STROBLRSPEED];
  }
}

void strobLVRoutine(byte force) {
  if (millis() > lastLVStrob + (10L * Value[T_STROBLVSPEED]) || force) {
    strobLVStep = (strobLVStep + 1) % 2;
    if (force)strobLVStep = 1;
    digitalWrite(outpin[T_LEDVVALUE], lightfunc(Value[T_LEDVVALUE]*strobLVStep));
    lastLVStrob += 10L * Value[T_STROBLVSPEED];
  }
}



