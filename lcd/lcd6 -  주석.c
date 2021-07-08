
/*
	buffer는 입력에 따라 데이터를 유지하기 위한 자료구조입니다.	
	buffer는 공백(space)와 데이터없음(0)을 구분하여 저장합니다.
	controlscreen함수는 buffer를 바탕으로 화면과 데이터를 동기화합니다.

int main()
{
	while(1)
	{
		/* 키를 하나 입력받음 */
		char key = readkey();
		
		/*
			key가 -1일때 아무것도 입력받지 않은 것으로 간주하고 아무것도 실행하지 않습니다.
			-1이 아닐땐 무언가 입력받은 것으로 간주하고 처리를 계속합니다.
		*/
		if(IMG_index != -1)
		{
			if(IMG_index == 111)		/* LEFT */
			{			
				/*
					LEFT 버튼이 입력됐을 때의 처리입니다.
					LEFT 버튼이 입력되면 커서를 왼쪽으로 이동합니다.
					따라서 기존 커서위치에 있던 글자를 원래 색으로 다시 표현해주어야 하므로 기존 위치에 printfont함수를 이용하여 폰트를 재출력합니다
					cursor_pos가 0보다 클때만 커서이동을 합니다. cursor_pos가 0이면 커서가 왼쪽 끝에 위치한 것이므로 이동할 필요가 없기 때문입니다.
					커서를 왼쪽으로 이동할 때 커서가 있던 기존 위치의 문자가 공백이고 buffer의 맨 끝자리이면 공백을 buffer에서 삭제합니다. 쓸데없는 공간 낭비를 줄이기 위함입니다.
				*/
			}
			else if(IMG_index == 112)	/* DELETE */
			{
				/*
					DELETE 버튼이 입력됐을 때의 처리입니다.
					DELETE 버튼은 현재 커서위치의 문자 하나를 없앱니다.
					문자를 제거하고 뒤에 이어지는 문자들을 앞으로 한 칸 당깁니다.
				*/
			}
			else if(IMG_index == 113)	/* RIGHT */
			{
				/*
					RIGHT 버튼이 입력됐을 때의 처리입니다.
					RIGHT 버튼이 입력되면 커서를 오른쪽으로 이동합니다.
					기존 커서 위치가 오른쪽 끝에 위치했으면 공백을 입력한 것으로 간주합니다.
				*/
			}
			else				/* INPUT */
			{	
				/*
					a ~ z, 점, 공백이 입력됐을 때의 처리입니다.
					현재 커서위치에 입력된 문자를 넣습니다.
					한 버튼에 여러 글자가 할당되어 있으므로 반복해서 누르거나 다른 입력버튼을 누르면 글자가 바뀝니다.
					is_repeat 변수를 이용하여 반복입력인지 아닌지 상태를 결정합니다.
					문자 사이에서 버튼이 입력됐을 때는 글자를 삽입하도록 처리합니다. 따라서 커서 뒤쪽에 있는 문자들이 한칸씩 밀립니다.
				*/
			}
		}
		/*
			어떤 처리를 마치고 나서는 항상 cursor_pos위치에 커서를 출력합니다.
		*/
	}
}

/* 원하는 RGB색상을 만드는 함수 */
unsigned short makepixel(ubyte r, ubyte g, ubyte b)

datablock init()
{
	/*
		프로그램 초기화 함수입니다.
		파일열기, mmap, wiringPi등등에 관한 초기화를 처리합니다.
	*/
}

char readkey()
{
	/*
		13개 버튼에 대한 입력을 받는 함수입니다.
		아무것도 입력되지 않았을 땐 0을 반환합니다.
		1개의 버튼만 입력됐을 땐 해당 버튼의 번호를 반환합니다.
		2개 이상의 버튼이 입력됐으면 아무것도 반환하지 않고 루프를 계속합니다.
	*/
}
 
int keytoindex(char key)
{
	/*
		readkey() 함수로 입력된 버튼의 번호를 적절한 정수로 변환합니다.
		a ~ z, 점, 공백이 입력됐을 땐 IMG배열을 참조하기 위한 index를 반환합니다.
		반복 입력시에 글자가 바뀌는 기능을 포함하고 있습니다.
		그 외의 입력은 구분 가능한 형태로 바꾸어 반환합니다.

		다른 기능으로는 버튼을 누르고 있어도 같은 입력이 계속 들어가지 않게 하였습니다.
	*/
}

void printfont(int x, int y, const unsigned char* img)
{
	/*
		(x, y) 위치에 전달받은 글자를 출력하는 함수입니다.
	*/
}

void controlscreen(int cmd, int cursor_pos, int buffer_size, const unsigned char** buffer)
{
	/*
		buffer와 화면을 동기화해주는 함수입니다.
		커서의 (x, y) 좌표를 이동시키기도 합니다.

		입력에 따라 다른 처리를 합니다.
		LEFT나 RIGHT가 입력되었을 때 현재 좌표를 확인하여 커서이동, 줄바꿈을 자동으로 수행합니다.
		커서나 글자가 화면을 넘어가더라도 스크롤 기능을 통해 더 많이 입력할 수 있도록 하였습니다.
	*/
}

void printcursor(int x, int y, const unsigned char* img)
{
	/*
		(x, y) 위치에 커서를 출력하는 함수입니다.
		printfont함수와 매우 유사합니다.
	*/
}

void increaseposition(int* x, int* y)
{
	/*
		cursor_pos가 증가되면 그에 맞게 화면의 x, y좌표도 조정시키는 함수입니다.
	*/
}


void updatescreen(int start, int end, int x, int y, const unsigned char** buffer)
{
	/*
		(x, y)위치에 buffer의 start에서 end-1까지의 데이터를 출력합니다.
		화면을 업데이트하기 위한 함수입니다.
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
