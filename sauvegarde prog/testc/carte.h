//
//  carte.h
//  testc
//
//  Created by SuperPierre on 20/03/2015.
//
//

#ifndef __testc__carte__
#define __testc__carte__

#include <iostream>
#include "extSPI.h"

//VALUE FOR PINOUT
#define LEDRVALUE 0
#define LEDVVALUE 1
#define LEDBVALUE 2
#define LED10W1VALUE 3
#define LED10W2VALUE 4
#define INTERRUPT 5
//VALUE FOR PININ
#define PUSH1 6
#define PUSH2 7
#define PUSH3 8
//VALUE FOR PININ ANALOG
#define FLOAT 9
#define UBATT 10
#define CODE 11
//RVB
#define LEDRVBSTROBSPEED 12
#define LEDRVBMODE 13
//GYRO
#define GYROMODE 14
#define GYROSPEED 15
#define GYROSTROBSPEED 16
//10W
#define LED10W1STROBSPEED 17
#define LED10W2STROBSPEED 18
//meta
#define BOARDMODE 19
#define PWMMODE 20
#define INIT 21
#define VOLTAGEMODE 22

//size of table
#define REGISTERSIZE 23

#define READCOMMAND 0x40
#define WRITECOMMANDVALUE 0xc0
#define WRITECOMMANDFADE 0x80
#define COMMANDMASK 0xC0

#define GYROALLON 1
#define GYROALLOFF 0
#define GYRORIGHT 2
#define GYROLEFT 3




#define PWM_LEDB 1 //1 = pwm sur led B ; 0 = pwm sur 2eme led 10w
#define PWM_10W2 0

class Carte{
  
private:
  int gamme_tension;
  int pwm_ledb_or_10w2;
  int GPIO_READ_BATT;
  int value[];
  
protected:
  extSPI SPIcarte;
  
  
  
  public :
  void initCarte(int _pwm_ledb_or_10w2, int _gamme_tension);
  void writeValue(int valueType,int value, int fadetime=0);
  int readValue(int valueType);
  int checkTension();
  void rgbValue(int r, int v, int b, int fadetime=0, int strob=0);
  void led10WValue(int v, int fadetime = 0, int strob=0);
  void setGyro(int mode, int speed, int strob=0);
  
  ~Carte();
  
};

#endif /* defined(__testc__carte__) */
