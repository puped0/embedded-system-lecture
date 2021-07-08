
/*
	buffer�� �Է¿� ���� �����͸� �����ϱ� ���� �ڷᱸ���Դϴ�.	
	buffer�� ����(space)�� �����;���(0)�� �����Ͽ� �����մϴ�.
	controlscreen�Լ��� buffer�� �������� ȭ��� �����͸� ����ȭ�մϴ�.

int main()
{
	while(1)
	{
		/* Ű�� �ϳ� �Է¹��� */
		char key = readkey();
		
		/*
			key�� -1�϶� �ƹ��͵� �Է¹��� ���� ������ �����ϰ� �ƹ��͵� �������� �ʽ��ϴ�.
			-1�� �ƴҶ� ���� �Է¹��� ������ �����ϰ� ó���� ����մϴ�.
		*/
		if(IMG_index != -1)
		{
			if(IMG_index == 111)		/* LEFT */
			{			
				/*
					LEFT ��ư�� �Էµ��� ���� ó���Դϴ�.
					LEFT ��ư�� �ԷµǸ� Ŀ���� �������� �̵��մϴ�.
					���� ���� Ŀ����ġ�� �ִ� ���ڸ� ���� ������ �ٽ� ǥ�����־�� �ϹǷ� ���� ��ġ�� printfont�Լ��� �̿��Ͽ� ��Ʈ�� ������մϴ�
					cursor_pos�� 0���� Ŭ���� Ŀ���̵��� �մϴ�. cursor_pos�� 0�̸� Ŀ���� ���� ���� ��ġ�� ���̹Ƿ� �̵��� �ʿ䰡 ���� �����Դϴ�.
					Ŀ���� �������� �̵��� �� Ŀ���� �ִ� ���� ��ġ�� ���ڰ� �����̰� buffer�� �� ���ڸ��̸� ������ buffer���� �����մϴ�. �������� ���� ���� ���̱� �����Դϴ�.
				*/
			}
			else if(IMG_index == 112)	/* DELETE */
			{
				/*
					DELETE ��ư�� �Էµ��� ���� ó���Դϴ�.
					DELETE ��ư�� ���� Ŀ����ġ�� ���� �ϳ��� ���۴ϴ�.
					���ڸ� �����ϰ� �ڿ� �̾����� ���ڵ��� ������ �� ĭ ���ϴ�.
				*/
			}
			else if(IMG_index == 113)	/* RIGHT */
			{
				/*
					RIGHT ��ư�� �Էµ��� ���� ó���Դϴ�.
					RIGHT ��ư�� �ԷµǸ� Ŀ���� ���������� �̵��մϴ�.
					���� Ŀ�� ��ġ�� ������ ���� ��ġ������ ������ �Է��� ������ �����մϴ�.
				*/
			}
			else				/* INPUT */
			{	
				/*
					a ~ z, ��, ������ �Էµ��� ���� ó���Դϴ�.
					���� Ŀ����ġ�� �Էµ� ���ڸ� �ֽ��ϴ�.
					�� ��ư�� ���� ���ڰ� �Ҵ�Ǿ� �����Ƿ� �ݺ��ؼ� �����ų� �ٸ� �Է¹�ư�� ������ ���ڰ� �ٲ�ϴ�.
					is_repeat ������ �̿��Ͽ� �ݺ��Է����� �ƴ��� ���¸� �����մϴ�.
					���� ���̿��� ��ư�� �Էµ��� ���� ���ڸ� �����ϵ��� ó���մϴ�. ���� Ŀ�� ���ʿ� �ִ� ���ڵ��� ��ĭ�� �и��ϴ�.
				*/
			}
		}
		/*
			� ó���� ��ġ�� ������ �׻� cursor_pos��ġ�� Ŀ���� ����մϴ�.
		*/
	}
}

/* ���ϴ� RGB������ ����� �Լ� */
unsigned short makepixel(ubyte r, ubyte g, ubyte b)

datablock init()
{
	/*
		���α׷� �ʱ�ȭ �Լ��Դϴ�.
		���Ͽ���, mmap, wiringPi�� ���� �ʱ�ȭ�� ó���մϴ�.
	*/
}

char readkey()
{
	/*
		13�� ��ư�� ���� �Է��� �޴� �Լ��Դϴ�.
		�ƹ��͵� �Էµ��� �ʾ��� �� 0�� ��ȯ�մϴ�.
		1���� ��ư�� �Էµ��� �� �ش� ��ư�� ��ȣ�� ��ȯ�մϴ�.
		2�� �̻��� ��ư�� �Էµ����� �ƹ��͵� ��ȯ���� �ʰ� ������ ����մϴ�.
	*/
}
 
int keytoindex(char key)
{
	/*
		readkey() �Լ��� �Էµ� ��ư�� ��ȣ�� ������ ������ ��ȯ�մϴ�.
		a ~ z, ��, ������ �Էµ��� �� IMG�迭�� �����ϱ� ���� index�� ��ȯ�մϴ�.
		�ݺ� �Է½ÿ� ���ڰ� �ٲ�� ����� �����ϰ� �ֽ��ϴ�.
		�� ���� �Է��� ���� ������ ���·� �ٲپ� ��ȯ�մϴ�.

		�ٸ� ������δ� ��ư�� ������ �־ ���� �Է��� ��� ���� �ʰ� �Ͽ����ϴ�.
	*/
}

void printfont(int x, int y, const unsigned char* img)
{
	/*
		(x, y) ��ġ�� ���޹��� ���ڸ� ����ϴ� �Լ��Դϴ�.
	*/
}

void controlscreen(int cmd, int cursor_pos, int buffer_size, const unsigned char** buffer)
{
	/*
		buffer�� ȭ���� ����ȭ���ִ� �Լ��Դϴ�.
		Ŀ���� (x, y) ��ǥ�� �̵���Ű�⵵ �մϴ�.

		�Է¿� ���� �ٸ� ó���� �մϴ�.
		LEFT�� RIGHT�� �ԷµǾ��� �� ���� ��ǥ�� Ȯ���Ͽ� Ŀ���̵�, �ٹٲ��� �ڵ����� �����մϴ�.
		Ŀ���� ���ڰ� ȭ���� �Ѿ���� ��ũ�� ����� ���� �� ���� �Է��� �� �ֵ��� �Ͽ����ϴ�.
	*/
}

void printcursor(int x, int y, const unsigned char* img)
{
	/*
		(x, y) ��ġ�� Ŀ���� ����ϴ� �Լ��Դϴ�.
		printfont�Լ��� �ſ� �����մϴ�.
	*/
}

void increaseposition(int* x, int* y)
{
	/*
		cursor_pos�� �����Ǹ� �׿� �°� ȭ���� x, y��ǥ�� ������Ű�� �Լ��Դϴ�.
	*/
}


void updatescreen(int start, int end, int x, int y, const unsigned char** buffer)
{
	/*
		(x, y)��ġ�� buffer�� start���� end-1������ �����͸� ����մϴ�.
		ȭ���� ������Ʈ�ϱ� ���� �Լ��Դϴ�.
	*/
}



/******************************************************************************************/


/*

*/

int main()
{
	while(1)
	{
		/*  */
		char key = readkey();
		
		/*

		*/
		if(IMG_index != -1)
		{
			if(IMG_index == 111)		/* LEFT */
			{			
				/*

				*/
			}
			else if(IMG_index == 112)	/* DELETE */
			{
				/*

				*/
			}
			else if(IMG_index == 113)	/* RIGHT */
			{
				/*

				*/
			}
			else				/* INPUT */
			{	
				/*

				*/
			}
		}
		/*

		*/
	}
}

/*  */
unsigned short makepixel(ubyte r, ubyte g, ubyte b)

datablock init()
{
	/*

	*/
}

char readkey()
{
	/*

	*/
}
 
int keytoindex(char key)
{
	/*

	*/
}

void printfont(int x, int y, const unsigned char* img)
{
	/*

	*/
}

void controlscreen(int cmd, int cursor_pos, int buffer_size, const unsigned char** buffer)
{
	/*

	*/
}

void printcursor(int x, int y, const unsigned char* img)
{
	/*

	*/
}

void increaseposition(int* x, int* y)
{
	/*

	*/
}


void updatescreen(int start, int end, int x, int y, const unsigned char** buffer)
{
	/*


	*/
}
