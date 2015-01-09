#include<reg51.h>
#include<intrins.h>
#define uint unsigned int
#define uchar unsigned char
uchar string_1[]="welcome to chip_";
uchar string_2[]="partner store   ";
//uchar string_1[]="http://stores.eb";
//uchar string_2[]="ay.com/Chip-Part";
//uchar string_1[]="ner-Store       ";
//uchar string_2[]="                ";
sbit RS=P2^0;
sbit RW=P2^1;
sbit E=P2^2;
void delay(uint x)
{
	uint i,j;
	for(i=0;i<x;i++)
		for(j=0;j<110;j++);
}
void Write_Command(uchar command)
{
	E=0;
	RS=0;
	RW=0;	
	P0=command;
	_nop_();	
	E=1;	
	delay(2);
	E=0;
}
void Write_Data(uchar dat)
{
	E=0;
	RS=1;
	RW=0;
	P0=dat;
	_nop_();
	E=1;
	delay(2);
	E=0;
}
void init(void)
{
	delay(150);
	Write_Command(0x38);
	delay(50);
	Write_Command(0x38);
	delay(50);
	Write_Command(0x38);
	Write_Command(0x38);
	Write_Command(0x08);
	Write_Command(0x01);
	Write_Command(0x06);
	Write_Command(0x0c);		
}
main()
{
	uint i;
	init();
	Write_Command(0x80+0x00);
	for(i=0;i<16;i++)
	{
		Write_Data(string_1[i]);
		//delay(1);
	}
	
	Write_Command(0x80+0x40);
	for(i=0;i<16;i++)
	{
		Write_Data(string_2[i]);
		//delay(1);
	}
	while(1);
}


