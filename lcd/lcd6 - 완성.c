

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
#define KEY_SPACE 21

#define KEY_LEFT 1
#define KEY_DEL 4
#define KEY_RIGHT 5 

#define LETTER_SIZE 24
#define LETTER_INTERVAL 4 
#define LINE_INTERVAL 4 

#define MAX_LETTER 10
#define MAX_LINE 8

typedef unsigned char ubyte;
typedef struct datablock
{
	int fbfd;
	struct fb_var_screeninfo fbvar;
	unsigned short* pfbdata;
	int prev_key;
	int last_key;
	int x, y;
}datablock;


datablock init();
char readkey();
int keytoindex(char);
void printfont(int, int, const unsigned char*);
void printcursor(int, int, const unsigned char*);
void controlscreen(int, int, int, const unsigned char**);
void updatescreen(int, int, int, int, const unsigned char**);
void increaseposition(int*, int*);

unsigned short makepixel(ubyte, ubyte, ubyte);

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

const unsigned char IMG_point[] = {0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xc3,0xff,0xff,0xc3,0xff,0xff,0xc3,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff}; 

const unsigned char IMG_empty[] = {0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff}; 

const unsigned char IMG_cursor[] = {0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}; 

const unsigned char* IMG[30] = {
	IMG_point, IMG_q, IMG_z,
	IMG_a, IMG_b, IMG_c,
	IMG_d, IMG_e, IMG_f,
	IMG_g, IMG_h, IMG_i,
	IMG_j, IMG_k, IMG_l,
	IMG_m, IMG_n, IMG_o,
	IMG_p, IMG_r, IMG_s,
	IMG_t, IMG_u, IMG_v,
	IMG_w, IMG_x, IMG_y,
	IMG_empty, IMG_empty, IMG_empty
};


char* img[30] = {
	".", "q", "z",
	"a", "b", "c",
	"d", "e", "f",
	"g", "h", "i",
	"j", "k", "l",
	"m", "n", "o",
	"p", "r", "s",
	"t", "u", "v",
	"w", "x", "y",
	"_", "_", "_"
};


datablock db;

int main()
{
	int IMG_index;
	int is_repeat = 0;	

	const unsigned char* buffer[1000] = {0};
	int buffer_size = 0;
	int cursor_pos = 0;
	
	/*************************/	
	char* buf[1000] = {0};
	int i;	
	/*************************/

	db = init();	
	
	while(1)
	{
		char key = readkey();
		IMG_index = keytoindex(key);
		
		if(IMG_index != -1)
		{
			if(IMG_index == 111)		/* LEFT */
			{				
				if(buffer[cursor_pos] == 0)
					printfont(db.x, db.y, IMG_empty);
				else
					printfont(db.x, db.y, buffer[cursor_pos]);

				if(cursor_pos>0)
				{
					controlscreen(IMG_index, cursor_pos, buffer_size, buffer);
					cursor_pos--;
				}
				
				if(cursor_pos+1 == buffer_size && buffer[cursor_pos] == IMG_empty)
				{
					buffer[cursor_pos] = 0;

					/********************/
					buf[cursor_pos] = 0;
					/********************/

					buffer_size--;
				}
				
				is_repeat = 0;
			}
			else if(IMG_index == 112)	/* DELETE */
			{
				if(buffer[cursor_pos] != 0)
				{
					memmove(buffer+cursor_pos, buffer+cursor_pos+1, sizeof(const unsigned char*)*(buffer_size-cursor_pos));

					/*********************/
					memmove(buf+cursor_pos, buf+cursor_pos+1, sizeof(char*)*(buffer_size-cursor_pos));
					/*********************/
					
					buffer_size--;
				}
			
				controlscreen(IMG_index, cursor_pos, buffer_size, buffer);
				is_repeat = 0;
			}
			else if(IMG_index == 113)	/* RIGHT */
			{
				if(buffer[cursor_pos] == 0)
					printfont(db.x, db.y, IMG_empty);
				else
					printfont(db.x, db.y, buffer[cursor_pos]);

				if(cursor_pos>=buffer_size)
				{
					buffer[cursor_pos] = IMG_empty;

					/********************/
					buf[cursor_pos] = "_";
					/********************/

					buffer_size++;
				}

				if(cursor_pos<1000)
				{
					controlscreen(IMG_index, cursor_pos, buffer_size, buffer);
					cursor_pos++;
				}
				is_repeat = 0;
			}
			else				/* INPUT */
			{	
				if(is_repeat==0)
				{
					if(cursor_pos != buffer_size)
					{
						memmove(buffer+cursor_pos+1, buffer+cursor_pos, sizeof(const unsigned char*)*(buffer_size-cursor_pos));
						
						/*********************/
						memmove(buf+cursor_pos+1, buf+cursor_pos, sizeof(char*)*(buffer_size-cursor_pos));
						/*********************/
					
					}
					buffer_size++;
					is_repeat=1;
				}
				buffer[cursor_pos] = IMG[IMG_index];
				controlscreen(IMG_index, cursor_pos, buffer_size, buffer);

				/*******************/
				buf[cursor_pos] = img[IMG_index];
				/******************/
			}
			
			/**********************/
			for(i=0; i<buffer_size; i++)
				printf("%s", buf[i]);
			printf(" : %d, %d, (%d, %d)\n", buffer_size, cursor_pos, db.x, db.y);
			for(i=0; i<cursor_pos; i++)
				printf(" ");
			printf("X\n");
			/**********************/
		}
				
		if(buffer[cursor_pos] == 0)
			printcursor(db.x, db.y, IMG_empty);
		else
			printcursor(db.x, db.y, buffer[cursor_pos]);
		
		delay(50);
		
	}
	
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
	int i,j,offset;	
	unsigned short px = makepixel(0, 0, 0);

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
	
	/* screen init... bg : white */
	for(i=0; i<fbvar.yres; i++)
	{
		for(j=0; j<fbvar.xres; j++)
		{
			offset = j + i*fbvar.xres;
			*(pfbdata+offset) = px;
		}
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
	pinMode(KEY_SPACE, INPUT);
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
	pullUpDnControl(KEY_SPACE, PUD_UP);
	pullUpDnControl(KEY_LEFT, PUD_UP);
	pullUpDnControl(KEY_DEL, PUD_UP);
	pullUpDnControl(KEY_RIGHT, PUD_UP);

	db.fbfd = fbfd;		/* screen file pointer	*/
	db.fbvar = fbvar;	/* screen info		*/
	db.pfbdata = pfbdata;	/* mmap memory pointer	*/
	db.prev_key = 0;	/* for input exception	*/
	db.last_key = 0;	/* for input rotation	*/
	db.x = 0;		/* x pos init		*/
	db.y = 0;		/* y pos init		*/

	return db;
}

char readkey()
{
	int i, count, key;
	char keys[13] = {0};
	
	while(1)
	{
		count = 0;
		keys[0] = digitalRead(KEY_1);
		keys[1] = digitalRead(KEY_2);
		keys[2] = digitalRead(KEY_3);
		keys[3] = digitalRead(KEY_4);
		keys[4] = digitalRead(KEY_5);
		keys[5] = digitalRead(KEY_6);
		keys[6] = digitalRead(KEY_7);
		keys[7] = digitalRead(KEY_8);
		keys[8] = digitalRead(KEY_9);
		keys[9] = digitalRead(KEY_SPACE);
		keys[10] = digitalRead(KEY_LEFT);
		keys[11] = digitalRead(KEY_DEL);
		keys[12] = digitalRead(KEY_RIGHT);

		for(i=0; i<13; i++)
		{
			if(keys[i] == 0)
			{
				key = i+1;
				count++;
			}
		}

		/* if count >= 2, continue loop */
		if(count==1)
			return key;
		else if(count==0)
			return 0;
		
		delay(50);
	}
}
 
int keytoindex(char key)
{
	int index;
	static int i = 0;
	if(key !=0)
	{
		if(db.prev_key != key)
		{
			if(key>=1 && key<=10)
			{
				if(db.last_key != key || i==2)
					i = 0;
				else
					i++;

				index = (key-1)*3 + i;				
			}
			else
				index = key + 100;
			db.last_key = key;
		}
		else
			index = -1;
	}
	else
		index = -1;
	
	db.prev_key = key;
	return index;
}

void printfont(int x, int y, const unsigned char* img)
{
	int i,j,k;
	int offset;
	unsigned short bg = makepixel(255,255,255);
	unsigned short px = makepixel(0,0,0);

	x = 22 + x*(LETTER_SIZE+LETTER_INTERVAL);
	y = 10 + y*(LETTER_SIZE+LINE_INTERVAL);

	for(i=0; i<24; i++)
	{
		for(j=0; j<3; j++)
		{
			unsigned char bits = img[j+i*3];
			unsigned char bit = 0x80;
			for(k=0; k<8; k++)
			{
				offset = x+k+j*8 + (y+i)*db.fbvar.xres;
				if((bits&bit)==bit)
					*(db.pfbdata+offset) = px;
				else
					*(db.pfbdata+offset) = bg;
				bit = bit>>1;
			}
		}
	}
}

void controlscreen(int cmd, int cursor_pos, int buffer_size, const unsigned char** buffer)
{
	static int scrolled = 0;
	if(cmd == 111)		/* LEFT	*/	
	{
		if(db.x - 1 < 0)
		{
			if(db.y - 1 >= 0)
			{
				db.y = db.y - 1;
				db.x = MAX_LETTER - 1;
			}
			else
			{
				if(scrolled > 0)
				{
					scrolled--;

					int start = scrolled*MAX_LETTER;
					int end = MAX_LETTER*MAX_LINE + scrolled*MAX_LETTER;

					updatescreen(start, end, 0, 0, buffer);
					db.x = 9;
				}
			}
		}
		else
			db.x = db.x - 1;
	}
	else if(cmd == 112)	/* DELETE */	
	{
		int start = cursor_pos;
		int end = MAX_LETTER*MAX_LINE + scrolled*MAX_LETTER;

		if(buffer_size < end)
			end = buffer_size+1;

		updatescreen(start, end, db.x, db.y, buffer);
	}
	else if(cmd == 113)	/* RIGHT */	
	{	
		if(db.x + 1 > MAX_LETTER - 1)
		{
			if(db.y + 1 <= MAX_LINE - 1)
			{
				db.y = db.y + 1;
				db.x = 0;
			}
			else
			{	
				scrolled++;
				
				int start = scrolled*MAX_LETTER;
				int end = MAX_LETTER*MAX_LINE + scrolled*MAX_LETTER;

				updatescreen(start, end, 0, 0, buffer);
				db.x = 0;
			}	
		}
		else
			db.x = db.x + 1;
	}
	else			/* INPUT */	
	{
		int start = cursor_pos;
		int end = MAX_LETTER*MAX_LINE + scrolled*MAX_LETTER;
		
		if(buffer_size < end)
			end = buffer_size;

		updatescreen(start, end, db.x, db.y, buffer);
	}
}

void printcursor(int x, int y, const unsigned char* img)
{
	int i,j,k;
	int offset;
	unsigned short px = makepixel(255,255,255);
	unsigned short bg = makepixel(0,0,0);

	x = 22 + x*(LETTER_SIZE+LETTER_INTERVAL);
	y = 10 + y*(LETTER_SIZE+LINE_INTERVAL);

	for(i=0; i<24; i++)
	{
		for(j=0; j<3; j++)
		{
			unsigned char bits = img[j+i*3];
			unsigned char bit = 0x80;
			for(k=0; k<8; k++)
			{
				offset = x+k+j*8 + (y+i)*db.fbvar.xres;
				if((bits&bit)==bit)
					*(db.pfbdata+offset) = px;
				else
					*(db.pfbdata+offset) = bg;
				bit = bit>>1;
			}
		}
	}
}

void increaseposition(int* x, int* y)
{
	if(*x + 1 > MAX_LETTER - 1)
	{
		if(*y + 1 <= MAX_LINE - 1)
		{
			*y = *y + 1;
			*x = 0;
		}
	}
	else
		*x = *x + 1;
}


void updatescreen(int start, int end, int x, int y, const unsigned char** buffer)
{
	int i;

	for(i=start; i<end; i++)
	{
		if(buffer[i] != 0)
			printfont(x, y, buffer[i]);
		else
			printfont(x, y, IMG_empty);
		increaseposition(&x, &y);
	}
}
