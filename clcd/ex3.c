


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

	pinMode(BTN1, INPUT);
	pullUpDnControl(BTN1, PUD_UP);

	pinMode(BTN2, INPUT);
	pullUpDnControl(BTN2, PUD_UP);
	while(1)
	{
		int blink1;
		int blink2;

		blink1 = digitalRead(BTN1);
		blink2 = digitalRead(BTN2);

		printf("%d %d\n", blink1, blink2);
		delay(200);
	}
	
	return 0;
}


