

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <linux/fb.h>
#include <linux/input.h>
#include <sys/mman.h>



#define FBDEVFILE "/dev/fb2"
#define EVENTFILE "/dev/input/event4"

typedef unsigned char ubyte;

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
	
	int center_x, center_y;
}paint;

void init();
input readtouch();

unsigned short makepixel(ubyte, ubyte, ubyte);

double K;
double A;
double B;
double C;
double D;
double E;
double F;

datablock db;

unsigned char background[160][240]

int main()
{
	init();
	input ip;
	
	int i, j, k;
	int prev_pressure = 0;
	struct input_event ie;

	int offset, px;


	px = makepixel(255,255,255);

	while(1)
	{
		ip = readtouch();		
			
		if(ip.x>=3 && ip.x<317 && ip.y>=3 && ip.y<237 && ip.pressure != 0)
		{
			for(i=ip.y-1; i<ip.y+2; i++)
			{
				for(j=ip.x-1; j<ip.x+2; j++)
				{
					offset = j + i*db.fbvar.xres;
					*(db.pfbdata+offset) = px;
				}
			}
		}
		
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
	
	px = makepixel(0,0,0);
	for(i=0; i<fbvar.yres; i++)
        {
                for(j=0; j<fbvar.xres; j++)
                {
                        offset = j + i*fbvar.xres;
                        *(pfbdata+offset) = px;
                }
        }


	bg = makepixel(0,0,0);
	px = makepixel(255,255,255);
	
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
		int x, y;

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


unsigned short makepixel(ubyte r, ubyte g, ubyte b)
{
        return (((r>>3)<<11) | ((g>>2)<<5) | (b>>3));
}
