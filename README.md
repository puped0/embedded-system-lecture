# embedded-system-lecture

강의명 : 임베디드 시스템
ordroid-c 보드 기반 linux application 개발 과제

1.LED
 - 빵판에 LED로 7-segment 회로 구성
 - 0 ~ F 까지의 숫자 출력, 원하는 LED만 출력
 
2. CLCD
 - 물리 버튼을 이용한 계산기 구현이 목표
 - 빵판에 물리 버튼으로 0 ~ 9, +, -, = 의 연산자/피연산자 버튼 회로 구성
  * overflow가 발생했을 경우에도 계속 계산할 수 있게 구현하지 못함(대신 overflow 글자가 화면에 출력)
 
3. LCD
 - 물리 버튼을 이용한 메모장 구현이 목표
 - 빵판에 3*3 의 영자 키보드 버튼, <=(커서왼쪽이동), =>(커서오른쪽이동), DEL 버튼 회로 구성

4. TS(Touch Screen)
 - 터치 스크린을 이용한 그림판 구현이 목표
 - 기능
  * Line - 두 점을 잇는 선을 그림
  * Rectangle - 두 점을 맞꼭지점으로 하는 네모를 그림
  * Oval - 두 점을 맞꼭지점으로 하는 직사각형에 내접하는 타원을 그림
  * Free draw - 펜으로 자유롭게 도형을 그림
  * Select - 그려진 도형을 선택하고 드래그해서 도형의 위치를 옮김
  * Erase - Free draw와 같은 원리로 그림을 지움 / Select와 연계하여 하나의 도형을 지움
  * Clear - 도형 전체 지우기
  
