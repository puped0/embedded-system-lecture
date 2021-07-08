

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <linux/fb.h>
#include <linux/input.h>
#include <sys/mman.h>
#include <math.h>

#define FBDEVFILE "/dev/fb2"
#define EVENTFILE "/dev/input/event4"

#define BUTTON_XSIZE 40
#define BUTTON_YSIZE 25

#define COLOR_SIZE 20
#define PAPER_SIZE 192 

typedef unsigned char ubyte;
typedef unsigned int dtype;

typedef struct tag_point
{
	int x;
	int y;
}point;

typedef struct tag_datablock
{
	int fd;
	int fbfd;
	struct fb_var_screeninfo fbvar;
	unsigned short* pfbdata;
}datablock;

typedef struct tag_input
{
	int x;
	int y;
	int pressure;
}input;

typedef struct tag_paint
{
	point min_pos;
	point max_pos;
	point flag;
	unsigned char color;
	unsigned int map[PAPER_SIZE][6];
}paint;

void init();
input readtouch();
int readcommand(input);
void setcolor(int);
void setdrawmode(int);
void setpenmode(int);
void drawing(input);
void dotinmap(int,int);
void clearpaper();
void createbg(paint* p);
void printbg(int,int,int,int);
void printmap(paint*);
paint* selectpaint(int,int);
void dotineraser(int, int);
void erasemap();

void printconsole(unsigned int (*)[6]);
void printcolorconsole(unsigned char (*)[PAPER_SIZE]);

unsigned short makepixel(ubyte, ubyte, ubyte);

const unsigned char IMG_line[] = {0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xfe,0xff,0xff,0xff,0xfd,0xfe,0xff,0xff,0xff,0xfd,0xff,0xff,0xff,0xff,0xfd,0xff,0xff,0xf8,0x7f,0xfd,0xfe,0xe8,0xf3,0x3f,0xfd,0xfe,0xea,0x77,0xbf,0xfd,0xfe,0xe3,0x77,0xbf,0xfd,0xfe,0xf7,0x30,0x3f,0xfd,0xfe,0xf7,0xb7,0xff,0xfd,0xff,0x77,0xb7,0xff,0xfe,0xff,0x77,0xb3,0xdf,0xfe,0x07,0x7f,0xf8,0x3f,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff}; 
const unsigned char IMG_rect[] = {0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xbf,0xff,0xff,0xff,0xfe,0x4f,0xff,0xff,0xff,0xfe,0x77,0xff,0xff,0xff,0xfe,0xf3,0xff,0xff,0xbf,0xfe,0xfb,0xff,0xff,0xbf,0xfe,0xf3,0x8f,0xe3,0xbf,0xfe,0xf7,0x33,0xdb,0xbf,0xfe,0xef,0x7b,0xbe,0x07,0xfe,0x9e,0x73,0xbf,0xbf,0xfe,0x1e,0x07,0xbf,0xbf,0xfe,0xe7,0x7f,0xbf,0xbf,0xfe,0xf3,0x3d,0x9f,0xbf,0xfe,0xf9,0x83,0xc1,0xc1,0xfe,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff};
const unsigned char IMG_oval[] = {0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xfc,0x1f,0xff,0xfe,0xff,0xf8,0x1f,0xff,0xfe,0xff,0xf2,0xef,0xff,0xfe,0xff,0xf7,0xe7,0xff,0xff,0x7f,0xef,0xf5,0xef,0xff,0x7f,0xef,0xf5,0xe8,0x3f,0x7f,0xef,0xf6,0xdb,0x9f,0x7f,0xf7,0xf6,0xdb,0x9f,0x7f,0xf7,0xf6,0x5b,0x8f,0x7f,0xf7,0xef,0x1b,0xa7,0x7f,0xfb,0xcf,0x3c,0x77,0x7f,0xfc,0x1f,0xfe,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff}; 
const unsigned char IMG_freedraw[] = {0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xc0,0x7f,0xff,0xff,0xff,0xcf,0xff,0xff,0xff,0xff,0xef,0xdf,0xff,0xff,0xff,0xe0,0xde,0x30,0x7f,0xff,0xef,0xc2,0x97,0x7f,0xff,0xef,0xde,0x30,0x7f,0xff,0xef,0xde,0xf3,0xff,0xff,0xff,0xde,0x19,0x7f,0xff,0xff,0xff,0x3c,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xef,0xff,0xff,0xff,0xff,0xef,0xff,0xff,0xff,0xff,0xed,0xcf,0xff,0xff,0xff,0xec,0x87,0xf7,0xef,0xff,0xee,0x3c,0x76,0xef,0xff,0x0e,0x7d,0x32,0x5f,0xff,0x4f,0x7d,0x1a,0x5f,0xff,0x6f,0x7c,0x59,0x1f,0xff,0x07,0x7f,0xcd,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff}; 
const unsigned char IMG_select[] = {0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xfe,0x7f,0xcf,0xff,0xff,0xf9,0x3f,0xdf,0xff,0xff,0xfb,0xff,0xcf,0xff,0xff,0xf7,0xff,0xef,0xff,0x9f,0xf7,0xff,0xef,0xff,0xdf,0xf7,0xf9,0xef,0xff,0xdf,0xfb,0xf0,0xec,0x39,0x03,0xfc,0xf6,0x69,0xb0,0xdf,0xfe,0x77,0x69,0x27,0xdf,0xff,0x30,0xe8,0x6f,0xdf,0xff,0xb7,0xeb,0xe7,0xdf,0xff,0x33,0x69,0xb0,0xdf,0xf8,0x78,0x6c,0x39,0xc3,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff}; 
const unsigned char IMG_erase[] = {0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xe0,0x3f,0xff,0xff,0xff,0xef,0xff,0xff,0xff,0xff,0xcf,0xff,0xff,0xff,0xff,0xdf,0xff,0xff,0xff,0xff,0xdf,0xff,0xff,0xff,0xff,0xdf,0xef,0xff,0xff,0xff,0xc0,0xe6,0x3f,0xf3,0x87,0xef,0xf4,0xa1,0xe5,0x73,0xef,0xf1,0xed,0xef,0x7b,0xef,0xf3,0xec,0xe7,0x33,0xef,0xf3,0xec,0xf9,0x07,0xe7,0xfb,0xed,0x7d,0x7f,0xf0,0x3b,0xf1,0x2d,0x3d,0xff,0xff,0xff,0xf3,0x93,0xff,0xff,0xff,0xff,0xc7,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff}; 
const unsigned char IMG_clear[] = {0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xfb,0xff,0xff,0xff,0xfc,0x7b,0xff,0xff,0xff,0xf3,0x3b,0xff,0xff,0xff,0xf7,0xfb,0xff,0xff,0xff,0xef,0xfb,0xff,0xff,0xff,0xcf,0xfb,0xff,0xfe,0xff,0xdf,0xfb,0xc3,0x0e,0x67,0xdf,0xf9,0x99,0x6f,0x43,0xdf,0xfd,0x33,0x67,0x1f,0xcf,0xfd,0x07,0x73,0x3f,0xef,0xfd,0x3f,0x73,0x7f,0xf7,0xfd,0xbf,0x31,0x7f,0xf0,0x7d,0x9f,0x85,0x7f,0xff,0xfd,0xe1,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff}; 
const unsigned char IMG_pen[] = {0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xfc,0x1f,0xff,0xff,0xff,0xfc,0xc7,0xff,0xff,0xff,0xfe,0xfb,0xff,0xff,0xff,0xfe,0xfb,0xff,0xff,0xff,0xfe,0xfb,0xff,0xff,0xff,0xfe,0xfb,0xc1,0xff,0xff,0xfe,0xfb,0x9e,0xd8,0x7f,0xfe,0xf7,0x7e,0xd3,0x3f,0xfe,0x0f,0x7c,0xd7,0xbf,0xfe,0xff,0x79,0xd7,0xbf,0xfe,0xff,0x07,0xc7,0xbf,0xfe,0xff,0x7f,0xef,0xdf,0xfe,0xff,0x7f,0xef,0xdf,0xfe,0xff,0x3f,0xef,0xdf,0xfe,0xff,0x91,0xef,0xff,0xfe,0xff,0xe7,0xff,0xff,0xfe,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff}; 
const unsigned char IMG_fill[] = {0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xfc,0x07,0x7d,0xef,0xff,0xfc,0xff,0x7d,0xef,0xff,0xfe,0xff,0xfd,0xef,0xff,0xfe,0xff,0xfd,0xef,0xff,0xfe,0xff,0xbd,0xef,0xff,0xfe,0xff,0xbd,0xef,0xff,0xfe,0x07,0xbd,0xef,0xff,0xfe,0xff,0xbd,0xef,0xff,0xfe,0xff,0xbd,0xef,0xff,0xfe,0xff,0xbd,0xef,0xff,0xfe,0xff,0xbd,0xe7,0xff,0xfe,0xff,0xbd,0xf7,0xff,0xfe,0xff,0xbe,0xf7,0xff,0xfe,0xff,0xbe,0xfb,0xff,0xff,0xff,0xbf,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff}; 

const point line_pos = {10,20};
const point rect_pos = {10,50};
const point oval_pos = {10,80};
const point freedraw_pos = {10,110};
const point select_pos = {10,140};
const point erase_pos = {10,170};
const point clear_pos = {10,200};
const point pen_pos = {270,160};
const point fill_pos = {270,200};
const point white_pos = {265,20};
const point orange_pos = {295,20};
const point red_pos = {265,50};
const point green_pos = {295,50};
const point yellow_pos = {265,80};
const point navy_pos = {295,80};
const point blue_pos = {265,110};
const point black_pos = {295,110};
const point paper_pos = {64,24};

double K;
double A;
double B;
double C;
double D;
double E;
double F;

datablock db;
int prev_pressure = 0;

unsigned short colors[8];
paint* buffer[1000];
int buffer_count;

unsigned int bgdot[PAPER_SIZE][6];
unsigned char bgcolor[PAPER_SIZE][PAPER_SIZE];
paint eraser;

int drawmode;
int penmode;
char current_color; 

paint* newpaint;
paint* selected;

int main()
{
	init();
	input ip;
	
	int i, j, k;
	struct input_event ie;

	int offset, px;
	int cmd;

	while(1)
	{

		ip = readtouch();
		cmd = readcommand(ip);

		if(cmd>=0 && cmd<=5)			// set drawmode
			setdrawmode(cmd);
		else if(cmd == 6)			// clear
		{
			/************************/
		//	createbg(NULL);
			/************************/
							
			clearpaper();
			
			/************************/
			
		//	printconsole(bgdot);
		//	printcolorconsole(bgcolor);
			/*	
			for(i=0;i<192;i++)
			{
				offset = (i+paper_pos.y)*db.fbvar.xres;
				for(j=0; j<6; j++)
				{
					unsigned int bits = bgdot[i][j];
					unsigned int bit = 0x80000000;

					if(bits == 0)
						continue;

					for(k=0; k<32; k++)
					{
						if((bits&bit)==bit)
							*(db.pfbdata+offset+j*32+k+paper_pos.x) = colors[bgcolor[i][j*32+k]];
						bit = bit>>1;
					}
				}
			}
			*/
			/***************************************************/
		}
		else if(cmd == 10 || cmd == 11)		// set pen or fill mode
			setpenmode(cmd);
		else if(cmd>=20 && cmd<=27)		// set color
			setcolor(cmd);
		else if(cmd==100)			// drawing
			drawing(ip);		
		
		prev_pressure = ip.pressure;
	}

	munmap(db.pfbdata, db.fbvar.xres*db.fbvar.yres*(16/8));
	close(db.fd);
	close(db.fbfd);
	return 0;
}

void init()
{	
	int fd, fbfd;
	struct input_event ie;
	int xpos1, xpos2, ypos1, ypos2;
	int ret, t;
	struct fb_var_screeninfo fbvar;
	int bg, px;

	int offset, i, j, k;
	unsigned short* pfbdata;

	int x, y, pressure = 0, prev_pressure = 0;	
	int lcd_x[3] = {100, 300, 250};
	int lcd_y[3] = {100, 200, 50};
	int ts_x[3] = {0};
	int ts_y[3] = {0};

	const unsigned char* func[9] = {IMG_line, IMG_rect, IMG_oval, IMG_freedraw, IMG_select, IMG_erase, IMG_clear, IMG_pen, IMG_fill};

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
	
	fd = open(EVENTFILE, O_RDONLY);
	if(fd < 0)
		exit(1);
	
	bg = makepixel(34, 177, 76);
	for(i=0; i<fbvar.yres; i++)
        {
                for(j=0; j<fbvar.xres; j++)
                {
                        offset = j + i*fbvar.xres;
                        *(pfbdata+offset) = bg;
                }
        }


	px = makepixel(0,0,0);
	
	for(j=lcd_y[0]-3; j<lcd_y[0]+3; j++)
	{
		for(k=lcd_x[0]-3; k<lcd_x[0]+3; k++)
		{
			 offset = j*fbvar.xres + k;
			 *(pfbdata+offset) = px;
		}
	}

	i = 0;
	while(1)
	{
                prev_pressure = pressure;

		read(fd, &ie, sizeof(struct input_event));
		if(ie.type==3)
		{
			if(ie.code==0)
				x = ie.value;
			else if(ie.code==1)
				y = ie.value;
			else if(ie.code==24)
				pressure = ie.value;
		}
	

                if(prev_pressure>0 && pressure==0)
                {
			for(j=lcd_y[i]-3; j<lcd_y[i]+3; j++)
			{
				for(k=lcd_x[i]-3; k<lcd_x[i]+3; k++)
				{
				 	offset = j*fbvar.xres + k;
					*(pfbdata+offset) = bg;
				}
			}

                        ts_x[i] = x;
                        ts_y[i] = y;
                        i++;

                        if(i==3)
                                break;
          
			for(j=lcd_y[i]-3; j<lcd_y[i]+3; j++)
			{
				for(k=lcd_x[i]-3; k<lcd_x[i]+3; k++)
				{
				 	offset = j*fbvar.xres + k;
					*(pfbdata+offset) = px;
				}
			}
		}
		printf("x = %d, y = %d, pressure = %d, prev_pressure = %d, i = %d\n", x, y, pressure, prev_pressure, i);
        }

	K = (ts_x[0] - ts_x[2])*(ts_y[1] - ts_y[2]) - (ts_x[1] - ts_x[2])*(ts_y[0] - ts_y[2]);
        A = ((lcd_x[0] - lcd_x[2])*(ts_y[1] - ts_y[2]) - (lcd_x[1] - lcd_x[2])*(ts_y[0] - ts_y[2]))/K;
        B = ((ts_x[0] - ts_x[2])*(lcd_x[1] - lcd_x[2]) - (lcd_x[0] - lcd_x[2])*(ts_x[1] - ts_x[2]))/K;
        C = (ts_y[0]*(ts_x[2]*lcd_x[1] - ts_x[1]*lcd_x[2]) + ts_y[1]*(ts_x[0]*lcd_x[2] - ts_x[2]*lcd_x[0]) + ts_y[2]*(ts_x[1]*lcd_x[0] - ts_x[0]*lcd_x[1]))/K;
        D = ((lcd_y[0] - lcd_y[2])*(ts_y[1] - ts_y[2]) - (lcd_y[1] - lcd_y[2])*(ts_y[0] - ts_y[2]))/K;
        E = ((ts_x[0] - ts_x[2])*(lcd_y[1] - lcd_y[2]) - (lcd_y[0] - lcd_y[2])*(ts_x[1] - ts_x[2]))/K;
        F = (ts_y[0]*(ts_x[2]*lcd_y[1] - ts_x[1]*lcd_y[2]) + ts_y[1]*(ts_x[0]*lcd_y[2] - ts_x[2]*lcd_y[0]) + ts_y[2]*(ts_x[1]*lcd_y[0] - ts_x[0]*lcd_y[1]))/K;

	printf("K = %lf, A = %lf, B = %lf, C = %lf, D = %lf, E = %lf, F = %lf\n", K, A, B, C, D, E, F);
        printf("%lf, %lf\n", A*1300+B*2400+C, D*1300+E*2400+F);

	colors[0] = makepixel(255,255,255);
	colors[1] = makepixel(255,127,39);
	colors[2] = makepixel(237,28,36);
	colors[3] = makepixel(181,230,29);
	colors[4] = makepixel(255,242,0);
	colors[5] = makepixel(63,72,204);
	colors[6] = makepixel(0,162,232);
	colors[7] = makepixel(0,0,0);

	x = 10;
	y = 20;

	px = makepixel(0,0,0);
	bg = makepixel(255,255,255);
	
	for(k=0; k<7; k++)
	{
		for(i=0; i<BUTTON_YSIZE; i++)
		{
			for(j=0; j<5; j++)
			{
				unsigned char bits = func[k][j+i*5];
				unsigned char bit = 0x80;
				for(t=0; t<8; t++)
				{
					offset = (x+t+j*8) + (y+i)*fbvar.xres;
					if((bits&bit)==bit)
						*(pfbdata+offset) = bg;
					else
						*(pfbdata+offset) = px;
					bit = bit>>1;
				}
			}
		}
		y = y + BUTTON_YSIZE + 5;
	}

	x = 270;
	y = 160;
	
	for(k=7; k<9; k++)
	{
		for(i=0; i<BUTTON_YSIZE; i++)
		{
			for(j=0; j<5; j++)
			{
				unsigned char bits = func[k][j+i*5];
				unsigned char bit = 0x80;
				for(t=0; t<8; t++)
				{
					offset = (x+t+j*8) + (y+i)*fbvar.xres;
					if((bits&bit)==bit)
						*(pfbdata+offset) = bg;
					else
						*(pfbdata+offset) = px;
					bit = bit>>1;
				}
			}
		}
		y = y + BUTTON_YSIZE + 15;
	}

	x = paper_pos.x;
	y = paper_pos.y;

	px = bg;

	for(i=y; i<y+PAPER_SIZE; i++)
	{
		offset = i*fbvar.xres;
		for(j=x; j<x+PAPER_SIZE; j++)
			*(pfbdata+offset+j) = px;
	}
	
	x = 265;
	y = 20;
	t = 0;
	
	for(k=0; k<4; k++)
	{
		for(i=y; i<y+COLOR_SIZE; i++)
		{
			offset = i*fbvar.xres;
			for(j=x; j<x+COLOR_SIZE; j++)
				*(pfbdata+offset+j) = colors[t];
		}
		t++;
		for(i=y; i<y+COLOR_SIZE; i++)
		{
			offset = i*fbvar.xres;
			for(j=x+30; j<x+30+COLOR_SIZE; j++)
				*(pfbdata+offset+j) = colors[t];
		}
		t++;
		y = y + 30;
	}

	memset(buffer, 0, sizeof(buffer));
	buffer_count = 0;

	drawmode = 3;
	penmode = 0;
	current_color = 7;
	newpaint = NULL;

	eraser.flag.x = 0;
	eraser.flag.y = 0;
	eraser.color = 0;

	db.fd = fd;
	db.fbfd = fbfd;
	db.fbvar = fbvar;
	db.pfbdata = pfbdata;
}

input readtouch()
{
	static int x = 0, y = 0;
	static int pressure = 0;
	struct input_event ie;
	int i;
		
	input new;
	
	read(db.fd, &ie, sizeof(struct input_event));
	if(ie.type==3)
	{
		if(ie.code==0)
			x = ie.value;
		else if(ie.code==1)
			y = ie.value;
		else if(ie.code==24)
			pressure = ie.value;
	}
	
	new.x = (int)(A*x+B*y+C);
	new.y = (int)(D*x+E*y+F);
	new.pressure = pressure;
	
	return new;
}

int readcommand(input ip)
{
	int x = ip.x;
	int y = ip.y;
	int pressure = ip.pressure;
		
	int cmd = -1;
	
	if(prev_pressure>0 && pressure==0)
	{
		if(x>=line_pos.x && x<line_pos.x+BUTTON_XSIZE)
		{
			if(y>=line_pos.y && y<line_pos.y+BUTTON_YSIZE)
				cmd = 0;
			else if(y>=rect_pos.y && y<rect_pos.y+BUTTON_YSIZE)
				cmd = 1;
			else if(y>=oval_pos.y && y<oval_pos.y+BUTTON_YSIZE)
				cmd = 2;
			else if(y>=freedraw_pos.y && y<freedraw_pos.y+BUTTON_YSIZE)
				cmd = 3;
			else if(y>=select_pos.y && y<select_pos.y+BUTTON_YSIZE)
				cmd = 4;
			else if(y>=erase_pos.y && y<erase_pos.y+BUTTON_YSIZE)
				cmd = 5;
			else if(y>=clear_pos.y && y<clear_pos.y+BUTTON_YSIZE)
				cmd = 6;
			else
				cmd = -1;
		}
		else if(x>=white_pos.x && x<orange_pos.x+COLOR_SIZE)
		{
			if(y>=pen_pos.y && y<pen_pos.y+BUTTON_YSIZE)
				cmd = 10;	
			else if(y>=fill_pos.y && y<fill_pos.y+BUTTON_YSIZE)
				cmd = 11;
			else
			{
				if(y>=white_pos.y && y<white_pos.y+COLOR_SIZE)
				{
					if(x<white_pos.x+COLOR_SIZE)
						cmd = 20;
					else if(x>=orange_pos.x)
						cmd = 21;
					else
						cmd = -1;
				}
				else if(y>=red_pos.y && y<red_pos.y+COLOR_SIZE)
				{
					if(x<white_pos.x+COLOR_SIZE)
						cmd = 22;
					else if(x>=orange_pos.x)
						cmd = 23;
					else
						cmd = -1;
				}
				else if(y>=yellow_pos.y && y<yellow_pos.y+COLOR_SIZE)
				{
					if(x<white_pos.x+COLOR_SIZE)
						cmd = 24;
					else if(x>=orange_pos.x)
						cmd = 25;
					else
						cmd = -1;
				}
				else if(y>=blue_pos.y && y<blue_pos.y+COLOR_SIZE)
				{
					if(x<white_pos.x+COLOR_SIZE)
						cmd = 26;
					else if(x>=orange_pos.x)
						cmd = 27;
					else
						cmd = -1;
				}
				else
					cmd = -1;
			}
		}
		else if(x>=paper_pos.x+1 && x<paper_pos.x+PAPER_SIZE-1) 
		{
			if(y>=paper_pos.y+1 && y<paper_pos.y+PAPER_SIZE-1)
				cmd = 100;
			else
				cmd = -1;
		}
		else
			cmd = -1;
	}
	else
	{
		if(x>=paper_pos.x+1 && x<paper_pos.x+PAPER_SIZE-1 && y>=paper_pos.y+1 && y<paper_pos.y+PAPER_SIZE-1)
			cmd = 100;
		else
		{
			if(newpaint!=NULL)
			{
				/****************************************************/
				//printconsole(newpaint->map);			
				printf("%d : buffer_count = %d\n",newpaint, buffer_count);			
				printf("(%d, %d)\n", newpaint->min_pos.x, newpaint->min_pos.y);
				printf("(%d, %d)\n\n", newpaint->max_pos.x, newpaint->max_pos.y);
				/****************************************************/

				buffer[buffer_count] = newpaint;
				buffer_count++;
				newpaint = NULL;
			}

			if(drawmode == 5 && eraser.color==1)
			{
				erasemap();
				eraser.color==0;	// color variable is not for color
			}				// it is for eraser mode(able / disable)
							// if color var is 0 : disable
							// if color var is 1 : able
								

			selected = NULL;
			cmd = -1;
		}
	}
	
	return cmd;	
}

void drawing(input ip)
{
	int x = ip.x;
	int y = ip.y;
	int pressure = ip.pressure;

	static int xpos1, ypos1;
	static int xpos2 = -100, ypos2 = -100;

	int i, j, k, offset;
	
	if(drawmode>=0 && drawmode <=3)
	{	
		if(prev_pressure == 0 && pressure > 0)			// on
		{
			newpaint = (paint*)malloc(sizeof(paint));
				
			newpaint->color = current_color;
			newpaint->min_pos.x = 500;
			newpaint->min_pos.y = 500;
			newpaint->max_pos.x = -100;
			newpaint->max_pos.y = -100;
			newpaint->flag.x = 0;
			newpaint->flag.y = 0;
			
			memset(newpaint->map, 0, sizeof(newpaint->map));
				
			if(drawmode!=3)
			{
				xpos1 = x - paper_pos.x;
				ypos1 = y - paper_pos.y;
				createbg(NULL);
			}
		}
		else if(prev_pressure > 0 && pressure == 0 && newpaint != NULL)		// off
		{
			buffer[buffer_count] = newpaint;
			buffer_count++;
			
			/****************************************************/
			//printfconsole(newpaint->map);
			printf("%d : buffer_count = %d\n",newpaint, buffer_count);			
			printf("(%d, %d)\n", newpaint->min_pos.x, newpaint->min_pos.y);
			printf("(%d, %d)\n\n", newpaint->max_pos.x, newpaint->max_pos.y);
			/****************************************************/

			newpaint = NULL;
		}
		else if(prev_pressure > 0 & pressure > 0 && newpaint != NULL)		// touching
		{
			if(drawmode == 0)						// line
			{
				x = x - paper_pos.x;
				y = y - paper_pos.y;
	
				if(x>xpos2+2 || x<xpos2-2 || y>ypos2+2 || y<ypos2-2)
				{
					double a, b;

					xpos2 = x;
					ypos2 = y;
					
					if(xpos2 != xpos1 && xpos2 > 0 && ypos2 > 0)
					{
						static int xhigh, xlow, ylow, yhigh;
						int result, q, r;
						a = (ypos2 - ypos1)/(double)(xpos2 - xpos1);
						b = ypos1 - a*xpos1;
						
						if(xhigh != 0)
							printbg(xlow-1, ylow-1, xhigh+1, yhigh+1);
						
						memset(newpaint->map, 0, sizeof(newpaint->map));
						
						if(xpos1<xpos2)
						{
							xlow = xpos1;
							xhigh = xpos2;
						}
						else
						{
							xlow = xpos2;
							xhigh = xpos1;
						}	

						for(i=xlow; i<=xhigh; i++)
						{
							result =(int)(i*a+b);
							dotinmap(i, result);
						}

						if(ypos1<ypos2)
						{
							ylow = ypos1;
							yhigh = ypos2;
						}
						else
						{
							ylow = ypos2;
							yhigh = ypos1;
						}

						for(i=ylow; i<=yhigh; i++)
						{
							result = (int)((i-b)/a);
							dotinmap(result, i);
						}						
						
						newpaint->min_pos.x = xlow-1;
						newpaint->min_pos.y = ylow-1;
						newpaint->max_pos.x = xhigh+1;
						newpaint->max_pos.y = yhigh+1;
						
						printmap(newpaint);	
					}
				}

			}
			else if(drawmode == 1)					// rect
			{
				x = x - paper_pos.x;
				y = y - paper_pos.y;
	
				if(x>xpos2+2 || x<xpos2-2 || y>ypos2+2 || y<ypos2-2)
				{
					double a, b;

					xpos2 = x;
					ypos2 = y;
					
					if(xpos2 > 0 && ypos2 > 0)
					{
						static int xhigh, xlow, ylow, yhigh;
					
						if(xhigh != 0)
							printbg(xlow-1, ylow-1, xhigh+1, yhigh+1);
						
						memset(newpaint->map, 0, sizeof(newpaint->map));
						
						if(xpos1<xpos2)
						{
							xlow = xpos1;
							xhigh = xpos2;
						}
						else
						{
							xlow = xpos2;
							xhigh = xpos1;
						}	

						if(ypos1<ypos2)
						{
							ylow = ypos1;
							yhigh = ypos2;
						}
						else
						{
							ylow = ypos2;
							yhigh = ypos1;
						}

						if(penmode == 0)
						{
							for(i=xlow; i<=xhigh; i=i+1)
								dotinmap(i, ylow);	
						
							for(i=xlow; i<=xhigh; i=i+1)
								dotinmap(i, yhigh);	
						
							for(i=ylow; i<=yhigh; i=i+1)
								dotinmap(xlow, i);	
						
							for(i=ylow; i<=yhigh; i=i+1)
								dotinmap(xhigh, i);	

						}
						else if(penmode == 1)
						{
							for(i=ylow; i<=yhigh; i=i+1)
							{
								for(j=xlow; j<=xhigh; j=j+1)
									dotinmap(j, i);
							}
						}					

						newpaint->min_pos.x = xlow-1;
						newpaint->min_pos.y = ylow-1;
						newpaint->max_pos.x = xhigh+1;
						newpaint->max_pos.y = yhigh+1;
						
						printmap(newpaint);	
					}
				}
			}
			else if(drawmode == 2)			// oval
			{
				x = x - paper_pos.x;
				y = y - paper_pos.y;
	
				if(x>xpos2+2 || x<xpos2-2 || y>ypos2+2 || y<ypos2-2)
				{
					double a, b;

					xpos2 = x;
					ypos2 = y;
					
					if(xpos1 != xpos2 && ypos1 != ypos2 && xpos2 > 0 && ypos2 > 0)
					{
						double a, b, center_x, center_y, result;
						static int xhigh, xlow, ylow, yhigh;
					
						if(xhigh != 0)
							printbg(xlow-1, ylow-1, xhigh+1, yhigh+1);
						
						memset(newpaint->map, 0, sizeof(newpaint->map));
						
						if(xpos1<xpos2)
						{
							xlow = xpos1;
							xhigh = xpos2;
						}
						else
						{
							xlow = xpos2;
							xhigh = xpos1;
						}	

						if(ypos1<ypos2)
						{
							ylow = ypos1;
							yhigh = ypos2;
						}
						else
						{
							ylow = ypos2;
							yhigh = ypos1;
						}

						a = (xhigh - xlow)/2.0;
						b = (yhigh - ylow)/2.0;

						center_x = (xhigh+xlow)/2.0;
						center_y = (yhigh+ylow)/2.0;
						
						if(penmode == 0)
						{			
							for(i=xlow; i<=xhigh; i++)
							{
								result = (b/a)*(sqrt(a*a-(i-center_x)*(i-center_x)));
								dotinmap(i, (int)(result+center_y));
								dotinmap(i, (int)(-result+center_y));
							}
							
							for(i=ylow; i<yhigh; i++)
							{
								int t1, t2;
								result = (a/b)*(sqrt(b*b-(i-center_y)*(i-center_y)));
								
								t1 = (int)(result+center_x);
								t2 = (int)(-result+center_x);
								
								dotinmap(t1, i);
								dotinmap(t2, i);

								if(penmode == 1)
								{
									for(j=t2; j<=t1; j++)
										dotinmap(j, i);
								}
							}
							
						}
						else if(penmode == 1)
						{
							for(i=ylow; i<yhigh; i++)
							{
								int t1, t2;
								result = (a/b)*(sqrt(b*b-(i-center_y)*(i-center_y)));
								
								t1 = (int)(result+center_x);
								t2 = (int)(-result+center_x);
								
								for(j=t2; j<=t1; j++)
									dotinmap(j, i);
							}
						}
						
						newpaint->min_pos.x = xlow-1;
						newpaint->min_pos.y = ylow-1;
						newpaint->max_pos.x = xhigh+1;
						newpaint->max_pos.y = yhigh+1;
					
						printmap(newpaint);	
					}
				}

			}
			else if(drawmode == 3)					// free draw
			{
					
				int _x = x - paper_pos.x;
				int _y = y - paper_pos.y;

				if(_x-1<newpaint->min_pos.x)
					newpaint->min_pos.x = _x-1;

				if(_y-1<newpaint->min_pos.y)
					newpaint->min_pos.y = _y-1;

				if(_x+1>newpaint->max_pos.x)
					newpaint->max_pos.x = _x+1;

				if(_y+1>newpaint->max_pos.y)
					newpaint->max_pos.y = _y+1;
				
				printf("(%d, %d)\n", _x, _y);
		
				dotinmap(_x, _y);				
					
				for(i=-1; i<2; i++)
				{
					offset = (y+i)*db.fbvar.xres;
					for(j=-1; j<2; j++)
						*(db.pfbdata+offset+x+j) = colors[newpaint->color];
				}
			}	
		}
	}
	else if(drawmode == 4)					// select
	{
		static int prev_fx, prev_fy;

		if(prev_pressure==0 && pressure>0)		// on
		{
			xpos1 = x - paper_pos.x;
			ypos1 = y - paper_pos.y;			
			
			selected = selectpaint(xpos1, ypos1);
		
			if(selected != NULL)
			{
				createbg(selected);
				prev_fx = selected->flag.x;
				prev_fy = selected->flag.y;
			}

			printf("select : %d\n", selected);
		}
		else if(prev_pressure>0 && pressure==0 && selected != NULL)		// off
		{
		//	printconsole(selected->map);
		//	printcolorconsole(bgcolor);	
			prev_fx = 0;
			prev_fy = 0;
		}
		else if(prev_pressure>0 && pressure>0 && selected != NULL)		// touching
		{
			
			x = x - paper_pos.x;
			y = y - paper_pos.y;

			if(x>=xpos2+1 || x<=xpos2-1 || y>=ypos2+1 || y<=ypos2-1 && selected != NULL)
			{
				
				int minx, miny, maxx, maxy, fx, fy;
				int _x, _y;
				
				xpos2 = x;
				ypos2 = y;

				_x = xpos2-xpos1;
				_y = ypos2-ypos1;

				fx = selected->flag.x;
				fy = selected->flag.y;
				minx = selected->min_pos.x;
				miny = selected->min_pos.y;
				maxx = selected->max_pos.x;
				maxy = selected->max_pos.y;

				if(minx+prev_fx+_x>=0 && maxx+prev_fx+_x<PAPER_SIZE && miny+prev_fy+_y>=0 && maxy+prev_fy+_y<PAPER_SIZE)
				{
					printbg(minx+fx, miny+fy, maxx+fx, maxy+fy);
					
					printf("(%d, %d) ==> ", selected->flag.x, selected->flag.y);

					selected->flag.x = prev_fx + _x;
					selected->flag.y = prev_fy + _y;

					printf("(%d, %d)\n", selected->flag.x, selected->flag.y);

					printmap(selected);
				}
			}
			
		}
	}
	else if(drawmode == 5)					// erase
	{
		if(prev_pressure==0 && pressure>0)		// on
		{
			xpos1 = x - paper_pos.x;
			ypos1 = y - paper_pos.y;

			eraser.color = 1;
			eraser.min_pos.x = 500;
			eraser.min_pos.y = 500;
			eraser.max_pos.x = -100;
			eraser.max_pos.y = -100;
			memset(eraser.map, 0xffffffff, sizeof(eraser.map));
		}
		else if(prev_pressure>0 && pressure==0 && eraser.color == 1)		// off
		{
			erasemap();
			eraser.color = 0;
		}
		else if(prev_pressure>0 && pressure>0 && eraser.color == 1)		// touching
		{
			x = x - paper_pos.x;
			y = y - paper_pos.y;

			if(x>=2 && x<PAPER_SIZE-2 && y>=2 && y<PAPER_SIZE-2)
			{
				if(x>=xpos2+1 || x<=xpos2-1 || y>=ypos2+1 || y<=ypos2-1)
				{
					xpos2 = x;
					ypos2 = y;
				
					if(x-2<eraser.min_pos.x)
						eraser.min_pos.x = x-2;

					if(y-2<eraser.min_pos.y)
						eraser.min_pos.y = y-2;

					if(x+2>eraser.max_pos.x)
						eraser.max_pos.x = x+2;

					if(y+2>eraser.max_pos.y)
						eraser.max_pos.y = y+2;
					//	printf("(%d, %d)\n", x, y);
					
					dotineraser(x,y);
				
					for(i=-2; i<3; i++)
					{
						offset = (y+paper_pos.y+i)*db.fbvar.xres;
						for(j=-2; j<3; j++)
							*(db.pfbdata+offset+x+paper_pos.x+j) = colors[0];
					}

				}
			}
		}
	}
}

void dotinmap(int x, int y) // q is xpos of buffer array (0 ~ 5)
{
	int q = (int)(x/32);
	int r = x%32;

	unsigned int bit = (0x80000000>>r);
	
	if(bit == 0x80000000)
	{
		newpaint->map[y-1][q-1] |= 0x00000001;
		newpaint->map[y][q-1] |= 0x00000001;
		newpaint->map[y+1][q-1] |= 0x00000001;	
	}
	else if(bit == 0x00000001)
	{
		newpaint->map[y-1][q+1] |= 0x80000000;
		newpaint->map[y][q+1] |= 0x80000000;
		newpaint->map[y+1][q+1] |= 0x80000000;	
	}

	newpaint->map[y-1][q] = newpaint->map[y-1][q] | bit | (bit<<1) | (bit>>1);
	newpaint->map[y][q] = newpaint->map[y][q] | bit | (bit<<1) | (bit>>1);
	newpaint->map[y+1][q] = newpaint->map[y+1][q] | bit | (bit<<1) | (bit>>1);
}

void clearpaper()
{
	int i,j,offset;

	/**********************/
	printf("buffer_count = %d\n", buffer_count);
	/**********************/
	for(i=0; i<buffer_count; i++)
	{
		/*********************************/
//		printconsole(buffer[i]->map);
		printf("%d : (%d, %d) ~ (%d, %d)\n", buffer[i], buffer[i]->min_pos.x, buffer[i]->min_pos.y, buffer[i]->max_pos.x, buffer[i]->max_pos.y);
		/*********************************/
		free(buffer[i]);
	}
	/**********************************/
	printf("\n");
	/**********************************/
	buffer_count = 0;

	for(i=0; i<PAPER_SIZE; i++)
	{
		offset = (i+paper_pos.y)*db.fbvar.xres;
		for(j=0; j<PAPER_SIZE; j++)
			*(db.pfbdata+offset+j+paper_pos.x) = colors[0];
	}
}

/*
void createbg(paint* p)
{
	int i,j,k,t;
	
	memset(bgdot, 0, sizeof(bgdot));
	memset(bgcolor, 0, sizeof(bgcolor));

	for(i=0; i<buffer_count; i++)
	{
		if(buffer[i] != p)
		{
			for(j=0; j<PAPER_SIZE; j++)
			{
				for(k=0; k<6; k++)
				{
					unsigned int bits = buffer[i]->map[j][k];
					unsigned char color = buffer[i]->color;
					if(bits != 0)
					{
						unsigned int bit = 0x80000000;
						bgdot[j][k] |= bits;

						for(t=0; t<32; t++)
						{
							if((bits&bit)==bit)
								bgcolor[j][k*32+t] = color;
							bit = bit>>1;
						}
					}
				}	
			}		
		}
	}
}
*/


void createbg(paint* p)
{
	int i,j,k,t;
	int xpos1, ypos1, xpos2, ypos2;
	int fx, fy;

	memset(bgdot, 0, sizeof(bgdot));
	memset(bgcolor, 0, sizeof(bgcolor));

	for(i=0; i<buffer_count; i++)
	{
		if(buffer[i] != p)
		{
			unsigned char color = buffer[i]->color;

			xpos1 = buffer[i]->min_pos.x;
			ypos1 = buffer[i]->min_pos.y;
			xpos2 = buffer[i]->max_pos.x;
			ypos2 = buffer[i]->max_pos.y;
			fx = buffer[i]->flag.x;
			fy = buffer[i]->flag.y;
			
			for(j=ypos1; j<=ypos2; j++)
			{
				for(k=xpos1; k<=xpos2; k++)
				{
					int q = (int)(k/32);
					int _q = (int)((k+fx)/32);
					int r = k%32;
					int _r = (k+fx)%32;

					unsigned int bits = buffer[i]->map[j][q];
					unsigned int bit = 0x80000000>>r;
					
					if(bits != 0)
					{
						if((bits&bit)==bit)
						{
							bgcolor[j+fy][k+fx] = color;
							bgdot[j+fy][_q] |= 0x80000000>>_r;
						}
					}
				}	
			}		
		}
	}
}


void printbg(int xpos1, int ypos1, int xpos2, int ypos2)
{
	int i,j, offset;
	int q, r;

	for(i=ypos1; i<=ypos2; i++)
	{
		offset = (i+paper_pos.y)*db.fbvar.xres;		
		for(j=xpos1; j<=xpos2; j++)
		{
			unsigned int bits, bit = 0x80000000;
			q = (int)(j/32);
			r = j%32;
			
			bits = bgdot[i][q];
			bit = bit >> r;

			*(db.pfbdata+j+offset+paper_pos.x) = colors[bgcolor[i][j]]; 
		}
	}	
}

void printmap(paint* p)
{
	int i,j, offset;
	int q, r;
	int fx, fy;	
	unsigned char color = p->color;

	fx = p->flag.x;
	fy = p->flag.y;

	for(i=p->min_pos.y; i<=p->max_pos.y; i++)
	{
		offset = (i+paper_pos.y+fy)*db.fbvar.xres;		
		for(j=p->min_pos.x; j<=p->max_pos.x; j++)
		{
			unsigned int bits, bit = 0x80000000;
			q = (int)(j/32);
			r = j%32;
			
			bits = p->map[i][q];
			bit = bit >> r;
			
			if(bits != 0 && (bits&bit)==bit)
				*(db.pfbdata+j+offset+paper_pos.x+fx) = colors[color];	
	
			/*
			if(bits != 0 && (bits&bit)==bit && bgcolor[i][j] != color)
				*(db.pfbdata+j+offset+paper_pos.x+fx) = colors[color];	
			*/
		}
	}	
}

paint* selectpaint(int x, int y)
{
	int i;
	paint* s = NULL;
	int minx, miny, maxx, maxy;
	int fx, fy;
	for(i=buffer_count-1; i>=0; i--)
	{
		fx = buffer[i]->flag.x;
		fy = buffer[i]->flag.y;
		minx = buffer[i]->min_pos.x+fx;
		miny = buffer[i]->min_pos.y+fy;
		maxx = buffer[i]->max_pos.x+fx;
		maxy = buffer[i]->max_pos.y+fy;

		if(x>=minx && x<=maxx && y>=miny && y<=maxy)
		{
			s = buffer[i];

			buffer[buffer_count] = buffer[i];
			memmove(buffer+i, buffer+i+1, sizeof(paint*)*(buffer_count-i)); 

			break;
		}
	}
	return s;
}

void dotineraser(int x, int y)
{
	int q = (int)(x/32);
	int r = x%32;

	unsigned int bit = (0x80000000>>r);

	if(bit == 0x40000000)
	{
		eraser.map[y-2][q-1] &= 0xfffffffe;
		eraser.map[y-1][q-1] &= 0xfffffffe;
		eraser.map[y][q-1] &= 0xfffffffe;
		eraser.map[y+1][q-1] &= 0xfffffffe;	
		eraser.map[y+2][q-1] &= 0xfffffffe;
	}
	else if(bit == 0x00000002)
	{
		eraser.map[y-2][q+1] &= 0x7fffffff;
		eraser.map[y-1][q+1] &= 0x7fffffff;
		eraser.map[y][q+1] &= 0x7fffffff;
		eraser.map[y+1][q+1] &= 0x7fffffff;	
		eraser.map[y+2][q+1] &= 0x7fffffff;
	}
	else if(bit == 0x80000000)
	{
		eraser.map[y-2][q-1] &= 0xfffffffc;
		eraser.map[y-1][q-1] &= 0xfffffffc;
		eraser.map[y][q-1] &= 0xfffffffc;
		eraser.map[y+1][q-1] &= 0xfffffffc;	
		eraser.map[y+2][q-1] &= 0xfffffffc;
	}
	else if(~bit == 0x00000001)
	{
		eraser.map[y-2][q+1] &= 0x3fffffff;
		eraser.map[y-1][q+1] &= 0x3fffffff;
		eraser.map[y][q+1] &= 0x3fffffff;
		eraser.map[y+1][q+1] &= 0x3fffffff;	
		eraser.map[y+2][q+1] &= 0x3fffffff;
	}

	eraser.map[y-2][q] = eraser.map[y-2][q] & ~bit & ~(bit<<1) & ~(bit>>1) & ~(bit<<2) & ~(bit>>2);
	eraser.map[y-1][q] = eraser.map[y-1][q] & ~bit & ~(bit<<1) & ~(bit>>1) & ~(bit<<2) & ~(bit>>2);
	eraser.map[y][q] = eraser.map[y][q] & ~bit & ~(bit<<1) & ~(bit>>1) & ~(bit<<2) & ~(bit>>2);
	eraser.map[y+1][q] = eraser.map[y+1][q] & ~bit & ~(bit<<1) & ~(bit>>1) & ~(bit<<2) & ~(bit>>2);
	eraser.map[y+2][q] = eraser.map[y+2][q] & ~bit & ~(bit<<1) & ~(bit>>1) & ~(bit<<2) & ~(bit>>2);
}

/*
void erasemap()
{
	int i,j,k;
	int xpos1, ypos1, xpos2, ypos2, fx, fy;
	int q, r;
	
	printconsole(eraser);

	for(i=0;i<buffer_count; i++)
	{
		fx = buffer[i]->flag.x;
		fy = buffer[i]->flag.y;

		if(fx<0)
		{
			xpos1 = 0;
			xpos2 = PAPER_SIZE+fx;
			q = (int)(fx/32)+1;
			r = fx%32+32;
		}
		else
		{
			xpos1 = fx;
			xpos2 = PAPER_SIZE;
			r = (int)(fx%32);
			q = fx/32;
		}

		if(fy<0)
		{
			ypos1 = 0;
			ypos2 = PAPER_SIZE+fy;
		}
		else
		{
			ypos1 = fy;
			ypos2 = PAPER_SIZE;
		}

		for(j=ypos1; j<ypos2; j++)
		{
			for(k=(int)(xpos1/32); k<(int)(xpos2/32); k++)
			{
				unsigned int bits = ~eraser[j][k];
				
				if(fx>0)
				{
					buffer[i]->map[j][k+q] &= (bits>>r);
					if(k!=(int)(xpos2/32)-1)
						buffer[i]->map[j][k+q+1] &= ~(bits<<(32-r));
				}
				else if(fx<0)
				{
					buffer[i]->map[j][k+q] &= (bits<<r);
					if(k!=0)
						buffer[i]->map[j][k+q+1] &= ~(bits>>(32-r));
				}
				else
					buffer[i]->map[j][k+q] &= ~bits;
			}
		}
	}
}
*/

void erasemap()
{
	int i,j,k;
	int fx, fy;
	int q,r,_q,_r;
	int xpos1 = eraser.min_pos.x;
	int ypos1 = eraser.min_pos.y;
	int xpos2 = eraser.max_pos.x;
	int ypos2 = eraser.max_pos.y;

	for(i=0; i<buffer_count; i++)
	{
		int is_empty;

		fx = buffer[i]->flag.x;
		fy = buffer[i]->flag.y;
		for(j=ypos1; j<=ypos2; j++)
		{
			for(k=xpos1; k<=xpos2; k++)
			{
				q = (int)(k/32);
				_q = (int)((k-fx)/32);
				r = k%32;
				_r = (k-fx)%32;

				unsigned int bits = ~eraser.map[j][q];
				unsigned int bit = 0x80000000>>r;

				if(bits != 0)
				{
					if((bits&bit)==bit)
						buffer[i]->map[j-fy][_q] &= ~(0x80000000>>_r);
				}	
			}
		}

		is_empty = 1;
		for(j=0; j<PAPER_SIZE; j++)
		{
			for(k=0; k<6; k++)
			{
				if(buffer[i]->map[j][k]!=0)
				{
					is_empty = 0;
					break;
				}
				else
					printf("(%d, %d) : 0\n", k, j);
			}

			if(is_empty == 0)
				break;
		}

		if(is_empty == 1)
		{
			printf("erased : %d\n", buffer[i]);
			free(buffer[i]);
			memmove(buffer+i, buffer+i+1,sizeof(paint*)*(buffer_count-i-1));
			buffer_count--;
			i--;
		}
	}
}
void printconsole(unsigned int (*map)[6])
{
	int i,j,k;
	
	printf("	0123456789012345678901234567890123456789012345678901234567890123456789\n");

	for(i=0; i<192; i++)
	{
		printf("%d	", i);
		
	eraser.color = 0;
		for(j=0; j<6; j++)
		{
			unsigned int bits = map[i][j];
			unsigned int bit = 0x80000000;

			if(bits == 0)
			{	
				printf("                                ");
				continue;
			}

			for(k=0; k<32; k++)
			{
				if((bits&bit)==bit)
					printf("0");
				else
					printf(" ");
				bit = bit>>1;
			}
		}
		printf("\n");
	}
	printf("\n\n");
}

void printcolorconsole(unsigned char (*color)[PAPER_SIZE])
{
	int i,j;
	printf("	0123456789012345678901234567890123456789012345678901234567890123456789\n");
	
	for(i=0;i<192;i++)
	{
		printf("%d	", i);
		for(j=0;j<192;j++)
			printf("%d", color[i][j]);
		printf("\n");
	}
	printf("\n\n");
}

void setcolor(int cmd)
{
	printf("color set : %d\n", cmd - 20);
	current_color = cmd-20;
}

void setdrawmode(int cmd)
{
	printf("drawmode set : %d\n", cmd);
	drawmode = cmd;
}

void setpenmode(int cmd)
{
	printf("penmode set : %d\n", cmd-10);
	penmode = cmd - 10;
}

unsigned short makepixel(ubyte r, ubyte g, ubyte b)
{
        return (((r>>3)<<11) | ((g>>2)<<5) | (b>>3));
}
