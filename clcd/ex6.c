


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wiringPi.h>

#define LCD_D4 2
#define LCD_D5 3
#define LCD_D6 1
#define LCD_D7 4

#define KEY_0	23
#define KEY_1	13
#define KEY_2	21
#define KEY_3	22
#define KEY_4	27
#define KEY_5	12
#define KEY_6	14
#define KEY_7	10	
#define KEY_8	11
#define KEY_9	26
#define KEY_EQ	24


#define KEY_P 5
#define KEY_M 6

#define LCD_RS 7
#define LCD_EN 0

void init();
void putdatacmd4(unsigned char cmd);
void write4bits(unsigned char cmd);
void putcmd(unsigned char cmd);
void putdata(char c);

int main()
{
	init();

	char key[13] = {};
	char prev_key = 0;
	char current_key = 0;
	char key_code[13] = {
			'0', '1', '2', '3', '4',
			'5', '6', '7', '8' ,'9',
			'=', '+', '-'};	

	while(1)
	{
		int i;	
		int count = 0;

		key[0] = digitalRead(KEY_0);
		key[1] = digitalRead(KEY_1);
		key[2] = digitalRead(KEY_2);
		key[3] = digitalRead(KEY_3);
		key[4] = digitalRead(KEY_4);
		key[5] = digitalRead(KEY_5);
		key[6] = digitalRead(KEY_6);
		key[7] = digitalRead(KEY_7);
		key[8] = digitalRead(KEY_8);
		key[9] = digitalRead(KEY_9);
		key[10] = digitalRead(KEY_EQ);
		key[11] = digitalRead(KEY_P);
		key[12] = digitalRead(KEY_M);
		
		for(i=0; i<13; i++)
		{
			if(key[i] == 0x00)
			{
				current_key = key_code[i];
				count++;
			}
		}
		
		if(count==0)
			prev_key = 0;
		else if(count==1 && prev_key != current_key)
		{
			putdata(current_key);	
			prev_key = current_key;
			printf("%c\n", current_key);
		}
			
		delay(100);
	}
	
	return 0;
}

void init()
{
	wiringPiSetup();

	pinMode(KEY_0, INPUT);
	pinMode(KEY_1, INPUT);
	pinMode(KEY_2, INPUT);
	pinMode(KEY_3, INPUT);
	pinMode(KEY_4, INPUT);
	pinMode(KEY_5, INPUT);
	pinMode(KEY_6, INPUT);
	pinMode(KEY_7, INPUT);
	pinMode(KEY_8, INPUT);
	pinMode(KEY_9, INPUT);
	pinMode(KEY_EQ, INPUT);
	pinMode(KEY_P, INPUT);
	pinMode(KEY_M, INPUT);

	pinMode(LCD_RS, OUTPUT);
	pinMode(LCD_EN, OUTPUT);
	pinMode(LCD_D4, OUTPUT);
	pinMode(LCD_D5, OUTPUT);
	pinMode(LCD_D6, OUTPUT);
	pinMode(LCD_D7, OUTPUT);

	pullUpDnControl(KEY_0, PUD_UP);
	pullUpDnControl(KEY_1, PUD_UP);
	pullUpDnControl(KEY_2, PUD_UP);
	pullUpDnControl(KEY_3, PUD_UP);
	pullUpDnControl(KEY_4, PUD_UP);
	pullUpDnControl(KEY_5, PUD_UP);
	pullUpDnControl(KEY_6, PUD_UP);
	pullUpDnControl(KEY_7, PUD_UP);
	pullUpDnControl(KEY_8, PUD_UP);
	pullUpDnControl(KEY_9, PUD_UP);
	pullUpDnControl(KEY_EQ, PUD_UP);
	pullUpDnControl(KEY_P, PUD_UP);
	pullUpDnControl(KEY_M, PUD_UP);

	digitalWrite(LCD_RS, 0);
	digitalWrite(LCD_EN, 0);
	digitalWrite(LCD_D4, 0);
	digitalWrite(LCD_D5, 0);
	digitalWrite(LCD_D6, 0);
	digitalWrite(LCD_D7, 0);

	delay(50);

	// Function Set
	putcmd(0x28); // 0010 1000
	putcmd(0x28);
	putcmd(0x28);
	delayMicroseconds(50);

	// Display On/OFf Shift
	putcmd(0x0e); // 0000 1110
	delayMicroseconds(50);

	// Entry Mode Set
	putcmd(0x06); // 0000 0110
	delayMicroseconds(50);

	// Curser Home
	putcmd(0x02); // 0000 0010
	delay(2);

	// Display Clear
	putcmd(0x01); // 0000 0001
	delay(2);
}	

void write4bits(unsigned char cmd)
{
	/*
	digitalWrite(LCD_D4, (cmd&0x01)==0x01);
	digitalWrite(LCD_D5, (cmd&0x02)==0x02);
	digitalWrite(LCD_D6, (cmd&0x04)==0x04);
	digitalWrite(LCD_D6, (cmd&0x08)==0x08);
	*/

	digitalWrite(LCD_D4, cmd&1);
	cmd = cmd >> 1;
	digitalWrite(LCD_D5, cmd&1);
	cmd = cmd >> 1;
	digitalWrite(LCD_D6, cmd&1);
	cmd = cmd >> 1;
	digitalWrite(LCD_D7, cmd&1);

	digitalWrite(LCD_EN, 1);
	delayMicroseconds(10);
	digitalWrite(LCD_EN, 0);
	delayMicroseconds(10);
}

void putdatacmd4(unsigned char cmd)
{
	write4bits((cmd>>4) & 0x0f);
	write4bits(cmd & 0x0f);
	delayMicroseconds(100);
}

void putcmd(unsigned char cmd)
{
	digitalWrite(LCD_RS, 0);
	putdatacmd4(cmd);
}

void putdata(char c)
{
	digitalWrite(LCD_RS, 1);
	putdatacmd4(c);
}
