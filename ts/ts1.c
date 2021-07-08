

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/input.h>

int main()
{
	int fd;
	struct input_event ie;
	int x=0,y=0,pressure=0;

	fd = open("/dev/input/event4", O_RDWR);
	if(fd<0) return -1;

	while(1)
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
			printf("x = %d, y = %d, pressure = %d\n", x, y, pressure);
		}
	}
	close(fd);
	return 0;
}
