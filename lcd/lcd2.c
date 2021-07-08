

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <linux/fb.h>
#include <wiringPi.h>
#include <string.h>

#define FBDEVFILE "/dev/fb2"

#define KEY_1 6
#define KEY_2 26
#define KEY_3 11
#define KEY_4 0
#define KEY_5 22
#define KEY_6 27
#define KEY_7 7
#define KEY_8 24
#define KEY_9 23

#define KEY_LEFT 1
#define KEY_DEL 4
#define KEY_RIGHT 5 

#define LETTER_SIZE 24
#define LETTER_INTERVAL 4
#define LINE_INTERVAL 8

typedef unsigned char ubyte;
typedef struct datablock
{
	int fbfd;
	struct fb_var_screeninfo fbvar;
	unsigned short* pfbdata;
}datablock;


datablock init();
char readkey();

unsigned short makepixel(ubyte r, ubyte g, ubyte b);

const unsigned char IMG_a[] = {0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xfe,0x03,0xff,0xfc,0x01,0xff,0xf8,0xf1,0xff,0xf1,0xf1,0xff,0xf3,0xf1,0xff,0xe3,0xf1,0xff,0xe7,0xf1,0xff,0xe7,0xf1,0xff,0xe7,0xf1,0xff,0xe7,0xf0,0xff,0xe3,0xf0,0xff,0xf3,0xe0,0x7f,0xf1,0xe6,0x7f,0xf8,0xc6,0x07,0xfc,0x0f,0x83,0xfe,0x1f,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff}; 

const unsigned char IMG_b[] = {0xff,0xff,0xff,0xff,0xff,0xff,0xfe,0x7f,0xff,0xfe,0x7f,0xff,0xfe,0x7f,0xff,0xfe,0x7f,0xff,0xfe,0x7f,0xff,0xfe,0x7f,0xff,0xfe,0x7f,0xff,0xfe,0x7f,0xff,0xfe,0x43,0xff,0xfe,0x00,0xff,0xfe,0x1c,0x7f,0xfe,0x3e,0x3f,0xfe,0x3f,0x3f,0xfe,0x3f,0x3f,0xfe,0x7f,0x3f,0xfe,0x7f,0x3f,0xfe,0x3e,0x3f,0xfe,0x3e,0x7f,0xfe,0x00,0x7f,0xff,0x00,0xff,0xff,0xff,0xff,0xff,0xff,0xff}; 

const unsigned char IMG_c[] = {0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x8f,0xff,0xff,0x03,0xff,0xfe,0x20,0xff,0xfc,0x7c,0x7f,0xfc,0xfe,0xff,0xfc,0xff,0xff,0xf8,0xff,0xff,0xf8,0xff,0xff,0xfc,0xff,0xff,0xfc,0x7c,0xff,0xfe,0x78,0xff,0xfe,0x01,0xff,0xff,0x03,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff};

const unsigned char IMG_d[] = {0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xfe,0x7f,0xff,0xfe,0x7f,0xff,0xfe,0x7f,0xff,0xfe,0x7f,0xff,0xfe,0x7f,0xff,0xfe,0x7f,0xff,0xfe,0x7f,0xff,0xfe,0x7f,0xff,0xfe,0x7f,0xff,0x8e,0x7f,0xff,0x06,0x7f,0xfe,0x22,0x7f,0xfe,0x70,0x7f,0xfc,0x78,0x7f,0xfc,0xfc,0x7f,0xfc,0x7c,0x7f,0xfe,0x00,0x7f,0xff,0x01,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff};

const unsigned char IMG_e[] = {0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xc1,0xff,0xff,0x80,0xff,0xff,0x1c,0x7f,0xfe,0x3e,0x3f,0xfe,0x7f,0x1f,0xfc,0x7e,0x3f,0xfc,0xfe,0x7f,0xfc,0x78,0x7f,0xfc,0x00,0xff,0xfc,0x03,0xff,0xfc,0x7f,0xff,0xfe,0x7f,0xff,0xfe,0x3e,0x3f,0xff,0x00,0x3f,0xff,0x80,0xff,0xff,0xff,0xff,0xff,0xff,0xff}; 

const unsigned char IMG_f[] = {0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xf3,0xff,0xff,0xe1,0xff,0xff,0xc0,0xff,0xff,0xcc,0x7f,0xff,0xce,0x7f,0xff,0xce,0x7f,0xff,0xcf,0xff,0xff,0xcf,0xff,0xff,0xcf,0xff,0xff,0x01,0xff,0xfe,0x00,0xff,0xfe,0x67,0xff,0xff,0xe7,0xff,0xff,0xe7,0xff,0xff,0xe7,0xff,0xff,0xe7,0xff,0xff,0xe7,0xff,0xff,0xe7,0xff,0xff,0xe7,0xff,0xff,0xe7,0xff,0xff,0xff,0xff,0xff,0xff,0xff};

const unsigned char IMG_g[] = {0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xfe,0x1f,0xff,0xfc,0x0f,0xff,0xf8,0xc7,0xff,0xf9,0xc7,0xff,0xf9,0xc3,0xff,0xf9,0xc3,0xff,0xf9,0xc3,0xff,0xf9,0x81,0xff,0xf8,0x19,0xff,0xfc,0x19,0xff,0xff,0xf9,0xff,0xff,0x01,0xff,0xfe,0x00,0x3f,0xfe,0x72,0x1f,0xfe,0x73,0xff,0xfe,0x73,0xff,0xfe,0x63,0xff,0xfe,0x07,0xff}; 

const unsigned char IMG_h[] = {0xff,0xff,0xff,0xff,0xff,0xff,0xfc,0xff,0xff,0xfc,0xff,0xff,0xfc,0xff,0xff,0xfc,0xff,0xff,0xfc,0xff,0xff,0xfc,0xff,0xff,0xfc,0xff,0xff,0xfc,0xff,0xff,0xfc,0xff,0xff,0xfc,0x8f,0xff,0xfc,0x07,0xff,0xfc,0x23,0xff,0xfc,0x73,0xff,0xfc,0x71,0xff,0xfc,0x79,0xff,0xfc,0x79,0xff,0xfc,0xf9,0xff,0xfc,0xf8,0xff,0xfc,0xfc,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff}; 

const unsigned char IMG_i[] = {0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xcf,0xff,0xff,0xcf,0xff,0xff,0xff,0xff,0xff,0xef,0xff,0xff,0xc7,0xff,0xff,0xcf,0xff,0xff,0xcf,0xff,0xff,0xcf,0xff,0xff,0xcf,0xff,0xff,0xcf,0xff,0xff,0xcf,0xff,0xff,0xcf,0xff,0xff,0xcf,0xff,0xff,0xc7,0xff,0xff,0xe3,0xff,0xff,0xf7,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff}; 

const unsigned char IMG_j[] = {0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xe7,0xff,0xff,0xe7,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xe7,0xff,0xff,0xe7,0xff,0xff,0xe7,0xff,0xff,0xe7,0xff,0xff,0xe7,0xff,0xff,0xe7,0xff,0xff,0xe7,0xff,0xff,0xe7,0xff,0xff,0x27,0xff,0xff,0x07,0xff,0xff,0x87,0xff,0xff,0xcf,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff}; 

const unsigned char IMG_k[] = {0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x9f,0xff,0xff,0x9f,0xff,0xff,0x9f,0xff,0xff,0x9f,0xff,0xff,0x9f,0xff,0xff,0x9c,0x7f,0xff,0x98,0x7f,0xff,0x81,0xff,0xff,0x83,0xff,0xff,0x8f,0xff,0xff,0x87,0xff,0xff,0x83,0xff,0xff,0x91,0xff,0xff,0x98,0x7f,0xff,0x9e,0x3f,0xff,0x9f,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff}; 

const unsigned char IMG_l[] = {0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x9f,0xff,0xff,0x9f,0xff,0xff,0x8f,0xff,0xff,0xcf,0xff,0xff,0xcf,0xff,0xff,0xcf,0xff,0xff,0xcf,0xff,0xff,0xcf,0xff,0xff,0xcf,0xff,0xff,0xcf,0xff,0xff,0xcf,0xff,0xff,0xcf,0xff,0xff,0xcf,0xff,0xff,0xcf,0xff,0xff,0xcf,0xff,0xff,0xc7,0xff,0xff,0xe3,0xff,0xff,0xf7,0xff,0xff,0xff,0xff}; 

const unsigned char IMG_m[] = {0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xfc,0xce,0x7f,0xfc,0x84,0x3f,0xfc,0x80,0x1f,0xfc,0x10,0x9f,0xfc,0x31,0x9f,0xfc,0x31,0x9f,0xfc,0x31,0x9f,0xfc,0x71,0x9f,0xfc,0x71,0x9f,0xfe,0x71,0x8f,0xfe,0x79,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff}; 

const unsigned char IMG_n[] = {0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xf7,0xff,0xf3,0xe1,0xff,0xf1,0xc0,0xff,0xf8,0x8c,0xff,0xfc,0x9c,0x7f,0xfc,0x1e,0x7f,0xfc,0x3e,0x7f,0xfe,0x3e,0x7f,0xfe,0x3e,0x3f,0xfe,0x3f,0x3f,0xfe,0x7f,0x3f,0xfe,0x7f,0x3f,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff}; 

const unsigned char IMG_o[] = {0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xc3,0xff,0xff,0x80,0xff,0xff,0x1c,0x7f,0xff,0x3e,0x7f,0xfe,0x3e,0x7f,0xfe,0x7e,0x7f,0xfe,0x7c,0x7f,0xfe,0x38,0xff,0xff,0x01,0xff,0xff,0x83,0xff,0xff,0xff,0xff,0xff,0xff,0xff}; 

const unsigned char IMG_p[] = {0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x80,0xff,0xff,0x00,0x7f,0xff,0x3e,0x7f,0xff,0x3e,0x7f,0xff,0x38,0x7f,0xff,0x30,0xff,0xff,0x03,0xff,0xff,0x07,0xff,0xff,0x3f,0xff,0xff,0x3f,0xff,0xff,0x3f,0xff,0xff,0x3f,0xff,0xff,0x3f,0xff,0xff,0x3f,0xff}; 

const unsigned char IMG_q[] = {0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x87,0xff,0xfe,0x03,0xff,0xfc,0x33,0xff,0xf8,0xf3,0xff,0xf9,0xf3,0xff,0xf8,0xe3,0xff,0xfc,0x03,0xff,0xff,0x03,0xff,0xff,0xf3,0xff,0xff,0xf3,0xff,0xff,0xf3,0xff,0xff,0xf3,0xff,0xff,0xf3,0xff}; 

const unsigned char IMG_r[] = {0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xfe,0xff,0xff,0xfc,0x7f,0xff,0xfc,0x38,0x7f,0xff,0x30,0x3f,0xff,0x23,0x1f,0xff,0x07,0xff,0xff,0x87,0xff,0xff,0x8f,0xff,0xff,0x8f,0xff,0xff,0x8f,0xff,0xff,0x9f,0xff,0xff,0x9f,0xff,0xff,0x9f,0xff,0xff,0x9f,0xff,0xff,0xff,0xff}; 

const unsigned char IMG_s[] = {0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x81,0xff,0xff,0x00,0xff,0xfe,0x3f,0xff,0xfe,0x7f,0xff,0xfe,0x1f,0xff,0xff,0x07,0xff,0xff,0xc1,0xff,0xff,0xf0,0xff,0xff,0xfc,0xff,0xff,0xfc,0xff,0xfc,0xf0,0xff,0xfc,0x01,0xff,0xfe,0x07,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff}; 

const unsigned char IMG_t[] = {0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xe7,0xff,0xff,0xe7,0xff,0xff,0xe7,0xff,0xfd,0xe7,0xff,0xfc,0x00,0x1f,0xfc,0x00,0x0f,0xff,0xe7,0xff,0xff,0xe7,0xff,0xff,0xe7,0xff,0xff,0xe7,0xff,0xff,0xe3,0xdf,0xff,0xf0,0x0f,0xff,0xf8,0x1f,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff}; 

const unsigned char IMG_u[] = {0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xfc,0xfc,0xff,0xf8,0xf8,0xff,0xf9,0xf8,0xff,0xf9,0xf8,0xff,0xf9,0xf8,0xff,0xf9,0xf8,0xff,0xf9,0xf8,0xff,0xf9,0xf0,0x7f,0xf9,0xf2,0x7f,0xf9,0xe2,0x3f,0xf9,0xc7,0x1f,0xf8,0x0f,0x8f,0xfc,0x1f,0xc7,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff}; 

const unsigned char IMG_v[] = {0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xfb,0xfc,0xff,0xf1,0xfc,0xff,0xf9,0xf8,0xff,0xf8,0xf9,0xff,0xfc,0xf9,0xff,0xfc,0x71,0xff,0xfe,0x73,0xff,0xfe,0x73,0xff,0xfe,0x23,0xff,0xff,0x07,0xff,0xff,0x87,0xff,0xff,0xcf,0xff,0xff,0xff,0xff,0xff,0xff,0xff}; 

const unsigned char IMG_w[] = {0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x3f,0xe7,0x8f,0x3f,0xe7,0x0f,0x3f,0xe7,0x0e,0x3f,0xe3,0x0e,0x7f,0xf3,0x06,0x7f,0xf3,0x26,0x7f,0xf0,0x24,0x7f,0xf8,0x64,0xff,0xf8,0x60,0xff,0xfc,0x70,0xff,0xfc,0x7f,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff}; 

const unsigned char IMG_x[] = {0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xfd,0xff,0xfe,0x78,0xff,0xfc,0x79,0xff,0xfe,0x31,0xff,0xff,0x33,0xff,0xff,0x03,0xff,0xff,0x87,0xff,0xff,0xcf,0xff,0xff,0x87,0xff,0xff,0x03,0xff,0xff,0x31,0xff,0xfe,0x38,0xff,0xfc,0x7c,0x7f,0xfc,0xfe,0x7f,0xff,0xff,0xff,0xff,0xff,0xff}; 

const unsigned char IMG_y[] = {0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x7f,0xfe,0x7e,0x3f,0xfe,0x3e,0x7f,0xff,0x1c,0x7f,0xff,0x8c,0xff,0xff,0xc0,0xff,0xff,0xe1,0xff,0xff,0xf0,0xff,0xff,0xe3,0xff,0xff,0xe7,0xff,0xff,0xc7,0xff,0xff,0xcf,0xff,0xff,0x8f,0xff,0xff,0x9f,0xff}; 

const unsigned char IMG_z[] = {0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xfc,0x03,0xff,0xf8,0x01,0xff,0xff,0xf9,0xff,0xff,0xe1,0xff,0xff,0xc3,0xff,0xff,0x8f,0xff,0xff,0x9f,0xff,0xff,0x1f,0xff,0xfe,0x30,0xff,0xfc,0x00,0x7f,0xfc,0x07,0xff,0xff,0xff,0xff,0xff,0xff,0xff}; 

const unsigned char IMG_point[] = {0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xe3,0xff,0xff,0xe3,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff}; 

/*
const unsigned char* IMG[27] = {
	IMG_point, IMG_q, IMG_z,
	IMG_a, IMG_b, IMG_c,
	IMG_d, IMG_e, IMG_f,
	IMG_g, IMG_h, IMG_i,
	IMG_j, IMG_k, IMG_l,
	IMG_m, IMG_n, IMG_o,
	IMG_p, IMG_r, IMG_s,
	IMG_t, IMG_u, IMG_v,
	IMG_w, IMG_x, IMG_y
};
*/

const unsigned char* IMG[27] = {
	".", "q", "z",
	"a", "b", "c",
	"d", "e", "f",
	"g", "h", "i",
	"j", "k", "l",
	"m", "n", "o",
	"p", "r", "s",
	"t", "u", "v",
	"w", "x", "y",
};

int main()
{
	int offset;
	unsigned short pixel;
	int x_t, y_t;
	int i = 0;
	int prev = 0;
	int last_key = 0;
	int x = 22;
	int y = 0;
	datablock db = init();	
	
	while(1)
	{
		char key = readkey();	
		if(key != 0)
		{
			if(key>=1 && key<=9)
			{
				if(prev != key)
				{
					if(last_key != key || i==2)
						i = 0;
					else
						i++;
					
					offset = (key-1)*3 + i;
				//	printf("%s, %d\n", IMG[offset], i);
				}
				
				printf("%d %d\n", prev, key);
				prev = key;
			}
			else if(key == 10);
			else if(key == 11);
			else if(key == 12);
			else;
		}
		else
			printf("nothing\n");

		delay(100);
	}
	
	/*
	offset = 100 + db.fbvar.xres*100;
	pixel = makepixel(255, 255, 255);
	*(db.pfbdata+offset) = pixel;
	printf("%d\n", offset);
	*/

	munmap(db.pfbdata, db.fbvar.xres*db.fbvar.yres*(16/8));
	close(db.fbfd);

	return 0;
}

unsigned short makepixel(ubyte r, ubyte g, ubyte b)
{
	return (((r>>3)<<11) | ((g>>2)<<5) | (b>>3));
}

datablock init()
{
	datablock db;
	
	int fbfd;
	int ret;
	struct fb_var_screeninfo fbvar;
	unsigned short* pfbdata;

	fbfd = open(FBDEVFILE, O_RDWR);
	if(fbfd < 0)
	{
		perror("fbdev open");
		exit(1);
	}

	ret = ioctl(fbfd, FBIOGET_VSCREENINFO, &fbvar);
	if(ret < 0)
	{
		perror("fbdev ioctl");
		exit(1);
	}

	if(fbvar.bits_per_pixel != 16)
	{
		fprintf(stderr, "bpp is not 16\n");
		exit(1);
	}

	pfbdata = (unsigned short*)mmap(0, fbvar.xres*fbvar.yres*(16/8), PROT_READ|PROT_WRITE, MAP_SHARED, fbfd, 0);
	if((unsigned)pfbdata == (unsigned)(-1))
	{
		perror("fbdev mmap");
		exit(1);
	}

	wiringPiSetup();

	pinMode(KEY_1, INPUT);
	pinMode(KEY_2, INPUT);
	pinMode(KEY_3, INPUT);
	pinMode(KEY_4, INPUT);
	pinMode(KEY_5, INPUT);
	pinMode(KEY_6, INPUT);
	pinMode(KEY_7, INPUT);
	pinMode(KEY_8, INPUT);
	pinMode(KEY_9, INPUT);
	pinMode(KEY_LEFT, INPUT);
	pinMode(KEY_DEL, INPUT);
	pinMode(KEY_RIGHT, INPUT);

	pullUpDnControl(KEY_1, PUD_UP);
	pullUpDnControl(KEY_2, PUD_UP);
	pullUpDnControl(KEY_3, PUD_UP);
	pullUpDnControl(KEY_4, PUD_UP);
	pullUpDnControl(KEY_5, PUD_UP);
	pullUpDnControl(KEY_6, PUD_UP);
	pullUpDnControl(KEY_7, PUD_UP);
	pullUpDnControl(KEY_8, PUD_UP);
	pullUpDnControl(KEY_9, PUD_UP);
	pullUpDnControl(KEY_LEFT, PUD_UP);
	pullUpDnControl(KEY_DEL, PUD_UP);
	pullUpDnControl(KEY_RIGHT, PUD_UP);

	db.fbfd = fbfd;
	db.fbvar = fbvar;
	db.pfbdata = pfbdata;

	return db;
}

char readkey()
{
	int i, count, key;
	char keys[12] = {0};

	keys[0] = digitalRead(KEY_1);
	keys[1] = digitalRead(KEY_2);
	keys[2] = digitalRead(KEY_3);
	keys[3] = digitalRead(KEY_4);
	keys[4] = digitalRead(KEY_5);
	keys[5] = digitalRead(KEY_6);
	keys[6] = digitalRead(KEY_7);
	keys[7] = digitalRead(KEY_8);
	keys[8] = digitalRead(KEY_9);
	keys[9] = digitalRead(KEY_LEFT);
	keys[10] = digitalRead(KEY_DEL);
	keys[11] = digitalRead(KEY_RIGHT);

	for(i=0; i<12; i++)
	{
		if(keys[i] == 0)
		{
			key = i+1;
			count++;
		}
	}

	if(count==1)
		return key;
	else
		return 0;
}
	
	
