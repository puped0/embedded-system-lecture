


#include <stdio.h>
#include <string.h>
#include <wiringPi.h>


#define LED0	14	
#define LED1	26
#define LED2	27
#define LED3	24
#define LED4	23
#define LED5	21
#define LED6	22

/*        
 * 	  @@0
 *	@5   @1
 *	@    @
 *	  @@6
 *	@4   @2
 *	@    @
 *	  @@3 
 */

char char_to_dec(char);

int main(int argc, char* argv[])

{
	if (argc != 2 && argc != 3)
	{
		printf("report 1\n");
		printf("report 2 [hex]\n");
		return 0;
	}

	printf("Blinktest!\n");
	
	wiringPiSetup();
	pinMode(LED0, OUTPUT);
	pinMode(LED1, OUTPUT);
	pinMode(LED2, OUTPUT);
	pinMode(LED3, OUTPUT);
	pinMode(LED4, OUTPUT);
	pinMode(LED5, OUTPUT);
	pinMode(LED6, OUTPUT);
	

	if(strcmp(argv[1], "1") == 0)
	{
		char code[16] = {63, 6, 91, 79 ,102, 109, 125, 39, 127, 111,
				 119, 124, 57, 94, 121, 71};
		int i;
			
		for (i = 0; i < 16; i++)
		{			
			
			digitalWrite(LED0, (code[i] & 0x1) == 0x1);
			digitalWrite(LED1, (code[i] & 0x2) == 0x2);
			digitalWrite(LED2, (code[i] & 0x4) == 0x4);
			digitalWrite(LED3, (code[i] & 0x8) == 0x8);
			digitalWrite(LED4, (code[i] & 0x10) == 0x10);
			digitalWrite(LED5, (code[i] & 0x20) == 0x20);
			digitalWrite(LED6, (code[i] & 0x40) == 0x40);

			delay(500);
			
			char bit = 0x40;
			int j;
			for (j = 0; j < 7; j++)
			{				
				printf("%d", (code[i] & bit) == bit);
				bit = bit >> 1;
			}
			printf(" : %d\n", code[i]);
		}
	}
	else if(strcmp(argv[1], "2") == 0)
	{
		if (argc != 3 ||
		strlen(argv[2])>4 ||
		argv[2][0] != '0' || 
		argv[2][1] != 'x' ||
		char_to_dec(argv[2][2]) == 255 ||
		char_to_dec(argv[2][3]) == 255 &&
		argv[2][3] != '\0')
			printf("argument2 must be hex [0x0 ~ 0xff] : %s\n", argv[2]);

		else
		{
			int j;
			char code;
			char bit = 0x40;
			code = char_to_dec(argv[2][2]);
			if(argv[2][3] != '\0')
			{
				code = code << 4;
				code = code + char_to_dec(argv[2][3]);
			}
				
			for (j = 0; j < 7; j++)
			{				
				printf("%d", (code & bit) == bit);
				bit = bit >> 1;
			}
			printf(" : %d\n", code);
			
			digitalWrite(LED0, (code & 0x1) == 0x1);
			digitalWrite(LED1, (code & 0x2) == 0x2);
			digitalWrite(LED2, (code & 0x4) == 0x4);
			digitalWrite(LED3, (code & 0x8) == 0x8);
			digitalWrite(LED4, (code & 0x10) == 0x10);
			digitalWrite(LED5, (code & 0x20) == 0x20);
			digitalWrite(LED6, (code & 0x40) == 0x40);

			delay(60000);
			
		}
	}
	else
		printf("unknown option : %s %s\n", argv[1], argv[2]);

	
	digitalWrite(LED0, LOW);
	digitalWrite(LED1, LOW);
	digitalWrite(LED2, LOW);
	digitalWrite(LED3, LOW);
	digitalWrite(LED4, LOW);
	digitalWrite(LED5, LOW);
	digitalWrite(LED6, LOW);
	

	printf("off\n");
	return 0;
}

char char_to_dec(char c)
{
	if (c >= 48 && c <= 57)
		c = c - 48;
	else if (c >= 65 && c <= 70)
		c = c - 55;
	else if (c >= 97 && c <= 102)
		c = c - 87;
	else
		c = 255;

	return c;
}

