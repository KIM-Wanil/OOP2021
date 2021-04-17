#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <iostream>
#include <conio.h> // console io
#include <cstring> // string.h
#include <cstdlib> // stdlib.h
#include <string> // c++ string class
#include <Windows.h>

// https://github.com/beomjoo90/OOP2021 , branch: 1학기

using namespace std;

int main()
{	
	const int directionToLeft = 0;
	const int directionToRight = 1;

	const int screenSize = 80;
	char screen[screenSize + 1]; // 0 .. 80 : 81

	char playerFace[20]{ "(^_^)" };
	int playerPos = 10;

	char enemyFace[20]{ "(`_#)" };
	int enemyPos = 50;


	const int maxBulletNum = 5;//연사 가능한 최대 총알 개수
	int bulletPos[maxBulletNum] = { 0 };//각 총알의 위치
	bool isFired[maxBulletNum] = {false};//각 총알의 발사상태

	int bulletIndex = 0;//발사할 총알의 번호 
	int firedBulletNum = 0;//발사된 총알의 개수
	int direction = directionToRight;

	int major;
	int minor;

	// game loop
	while (true) {
		memset(screen, ' ', screenSize); // clear screen

		// draw game objects
		strncpy(&screen[playerPos], playerFace, strlen(playerFace) ) ; // draw player
		strncpy(&screen[enemyPos], enemyFace, strlen(enemyFace)); // draw enemy
		
		for (int i = 0 ; i <  maxBulletNum ;i++) {
			if (bulletPos[i] > 0 && bulletPos[i] < screenSize) {
				screen[bulletPos[i]] = '-'; // draw bullet
			}
		}
		
		

		// handle inputs
   		if (_kbhit() ) {
			major = _getch();
			switch (major) {
			case ' ':
				if (firedBulletNum >= maxBulletNum) break;
				if (bulletIndex >= maxBulletNum-1) 
					bulletIndex = 0;
				else 
					bulletIndex += 1;
				isFired[bulletIndex] = true;
				firedBulletNum += 1;

				if (playerPos < enemyPos) direction = directionToRight;
				else direction = directionToLeft;

				if (direction == directionToRight) {
					bulletPos[bulletIndex] = playerPos + strlen(playerFace) - 1;
				}
				else bulletPos[bulletIndex] = playerPos;


				break;
			case 224: // arrow key, function key pressed
				minor = _getch();
				switch (minor) {
				case 75: // left
					break;
				case 77: // right
					break;
				case 72: // up
					break;
				case 80: // down
					break;
				}
				break;
			}
		}

		// update
	 // update bullet
		for (int i = 0; i < maxBulletNum;i++) {
			if (isFired[i] == true)
				direction == directionToRight ? bulletPos[i]++ : bulletPos[i]--; // move bullet automatically

			if ((direction == directionToLeft && enemyPos + strlen(enemyFace) - 1 == bulletPos[i])
				|| (direction == directionToRight && enemyPos == bulletPos[i])) { // 적이 총알을 맞았을 때
				bulletPos[i] = 0;//총알 위치 초기화
				isFired[i] = false;//총알 발사 상태 초기화
				firedBulletNum -= 1;//발사된 총알 개수 -1
				strcpy(enemyFace, "(T_T)");
			}
		}
		

		if (bulletPos[bulletIndex] < 0 || bulletPos[bulletIndex] >= screenSize) {
		}
		
		// update enemy

		screen[screenSize] = '\0';  // render screen
		printf("%s\r", screen);

		Sleep(100);
	}
	printf("\nGame Over\n");
	
	return 0;
}