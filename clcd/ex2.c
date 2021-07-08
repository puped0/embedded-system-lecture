


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wiringPi.h>

#define LCD_D4 2
#define LCD_D5 3
#define LCD_D6 1
#define LCD_D7 4

#define BTN1 5
#define BTN2 6

#define LCD_RS 7
#define LCD_EN 0



int main()
{
	wiringPiSetup();

	pinMode(LCD_D5, INPUT);
	pullUpDnControl(LCD_D5, PUD_UP);

	while(1)
	{
		int blink;

		blink = digitalRead(LCD_D5);
		printf("%d\n", blink);
		delay(200);
	}
	
	return 0;
}


