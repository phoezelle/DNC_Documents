
#include<avr/io.h>
#define uint unsigned int
#define uchar unsigned char
uchar string_1[]="welcome to chip_";
uchar string_2[]="partner store   ";
//uchar string_1[]="http://stores.eb";
//uchar string_2[]="ay.com/Chip-Part";
//uchar string_1[]="ner-Store       ";
//uchar string_2[]="                ";
//RS--PC0
//RW--PC1
//E--PC2
void delay(uint x)
{
	uint i,j;
	for(i=0;i<x;i++)
		for(j=0;j<100;j++);
}

void Write_Command(uchar command)
{
	//E--PC2
	//RS--PC0
	//RW--PC1
	PORTC &=~(1<<PC0)|(1<<PC1)|(1<<PC2);
	PORTA = command;
	PORTC |=(1<<PC2);//E--PORTC2
	delay(2);
	PORTC &=~(1<<PC2);//E--PORTC2
}
void Write_Data(uchar dat)
{
	//E--PC2
	//RS--PC0
	//RW--PC1
	PORTC &=~(1<<PC1)|(1<<PC2);
	PORTC |=(1<<PC0);
	PORTA = dat;
	//_nop_();
	PORTC |=(1<<PC2);	//E--PC2
	delay(2);
	PORTC &=~(1<<PC2);	//E--PC2
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
int main()
{
	uint i;
	DDRA |=0XFF;
	DDRC |=0XFF;
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
