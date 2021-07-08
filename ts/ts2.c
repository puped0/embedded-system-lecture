

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <linux/fb.h>
#include <linux/input.h>
#include <sys/mman.h>
	
typedef unsigned char ubyte;	

unsigned short makepixel(ubyte, ubyte, ubyte);

int main()
{
	int fd, fbfd;
	struct input_event ie;
	int xpos1, xpos2, ypos1, ypos2, pressure = 0;
	int ret, t;
	struct fb_var_screeninfo fbvar;
	unsigned short* pfbdata;
	int rpixel;

	int prev_pressure = 0;

	int lcd_x[3] = {0};
	int lcd_y[3] = {0};
	int ts_x[3] = {0};
	int ts_y[3] = {0};
	int i,j;
	int offset;


	fbfd = open("/dev/fb2", O_RDWR);
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


	fd = open("/dev/input/event4", O_RDONLY);
	if(fd < 0) return -1;
	
	rpixel = makepixel(0,0,0);
	for(i=0; i<fbvar.yres; i++)
        {
                for(j=0; j<fbvar.xres; j++)
                {
                        offset = j + i*fbvar.xres;
                        *(pfbdata+offset) = rpixel;
                }
        }
	
	/*
	for(i=0;i<3;i++)
	{
		lcd_x[0] = rand()&320;
		lcd_y[0] = rand()&240;
	}
	*/
	lcd_x[0] = 100;
	lcd_y[0] = 100;

	lcd_x[1] = 300;
	lcd_y[1] = 200;

	lcd_x[2] = 250;
	lcd_y[2] = 50;

	i = 0;
	while(1)
	{
		int x, y;
		
		offset = lcd_x[i] + lcd_y[i]*fbvar.xres;
		*(pfbdata+offset) = makepixel(255,255,255);

		prev_pressure = pressure;

		for(j=0; j<3; j++)
		{
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
		}
                
		printf("x = %d, y = %d, pressure = %d, prev_pressure = %d, i = %d\n", x, y, pressure, prev_pressure, i);
		
		if(prev_pressure>0 && pressure==0)
		{
			*(pfbdata+offset) = makepixel(0,0,0);
			ts_x[i] = x;
			ts_y[i] = y;
			i++;

			if(i==3)
				break;
		}
        }

	for(i=0;i<3;i++)
		printf("%d, %d / %d, %d\n",lcd_x[i], lcd_y[i], ts_x[i], ts_y[i]);

	/*	
	double k = (lcd_x[0] - lcd_x[2])*(lcd_y[1] - lcd_y[2]) - (lcd_x[1] - lcd_x[2])*(lcd_y[0] - lcd_y[2]);
	double a = ((ts_x[0] - ts_x[2])*(lcd_y[1] - lcd_y[2]) - (ts_x[1] - ts_x[2])*(lcd_y[0] - lcd_y[2]))/k;
	double b = ((lcd_x[0] - lcd_x[2])*(ts_x[1] - ts_x[2]) - (ts_x[0] - ts_x[2])*(lcd_x[1] - lcd_x[2]))/k;
	double c = (lcd_y[0]*(lcd_x[2]*ts_x[1] - lcd_x[1]*ts_x[2]) + lcd_y[1]*(lcd_x[0]*ts_x[2] - lcd_x[2]*ts_x[0]) + lcd_y[2]*(lcd_x[1]*ts_x[0] - lcd_x[0]*ts_x[1]))/k;
	double d = ((ts_y[0] - ts_y[2])*(lcd_y[1] - lcd_y[2]) - (ts_y[1] - ts_y[2])*(lcd_y[0] - lcd_y[2]))/k;
	double e = ((lcd_x[0] - lcd_x[2])*(ts_y[1] - ts_y[2]) - (ts_y[0] - ts_y[2])*(lcd_x[1] - lcd_x[2]))/k;
	double f = (lcd_y[0]*(lcd_x[2]*ts_y[1] - lcd_x[1]*ts_y[2]) + lcd_y[1]*(lcd_x[0]*ts_y[2] - lcd_x[2]*ts_y[0]) + lcd_y[2]*(lcd_x[1]*ts_y[0] - lcd_x[0]*ts_y[1]))/k;
	*/

	double k = (ts_x[0] - ts_x[2])*(ts_y[1] - ts_y[2]) - (ts_x[1] - ts_x[2])*(ts_y[0] - ts_y[2]);
        double a = ((lcd_x[0] - lcd_x[2])*(ts_y[1] - ts_y[2]) - (lcd_x[1] - lcd_x[2])*(ts_y[0] - ts_y[2]))/k;
        double b = ((ts_x[0] - ts_x[2])*(lcd_x[1] - lcd_x[2]) - (lcd_x[0] - lcd_x[2])*(ts_x[1] - ts_x[2]))/k;
        double c = (ts_y[0]*(ts_x[2]*lcd_x[1] - ts_x[1]*lcd_x[2]) + ts_y[1]*(ts_x[0]*lcd_x[2] - ts_x[2]*lcd_x[0]) + ts_y[2]*(ts_x[1]*lcd_x[0] - ts_x[0]*lcd_x[1]))/k;
        double d = ((lcd_y[0] - lcd_y[2])*(ts_y[1] - ts_y[2]) - (lcd_y[1] - lcd_y[2])*(ts_y[0] - ts_y[2]))/k;
        double e = ((ts_x[0] - ts_x[2])*(lcd_y[1] - lcd_y[2]) - (lcd_y[0] - lcd_y[2])*(ts_x[1] - ts_x[2]))/k;
        double f = (ts_y[0]*(ts_x[2]*lcd_y[1] - ts_x[1]*lcd_y[2]) + ts_y[1]*(ts_x[0]*lcd_y[2] - ts_x[2]*lcd_y[0]) + ts_y[2]*(ts_x[1]*lcd_y[0] - ts_x[0]*lcd_y[1]))/k;
	
	printf("k = %lf, a = %lf, b = %lf, c = %lf, d = %lf, e = %lf, f = %lf\n", k, a, b, c, d, e, f);
	printf("%lf, %lf\n", a*1300+b*2400+c, d*1300+e*2400+f);
	

	if(xpos1 > xpos2)
	{
		t = xpos1;
		xpos1 = xpos2;
		xpos2 = t;
	}

	if(ypos1 > ypos2)
	{
		t = ypos1;
		ypos1 = ypos2;
		ypos2 = t;
	}

	rpixel = (int)(65536.0*rand()/(RAND_MAX+1.0));







	close(fd);
	close(fbfd);
	return 0;
}

unsigned short makepixel(ubyte r, ubyte g, ubyte b)
{
        return (((r>>3)<<11) | ((g>>2)<<5) | (b>>3));
}

