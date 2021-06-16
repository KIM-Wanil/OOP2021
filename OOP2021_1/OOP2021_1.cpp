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

const int directionToLeft = 0;
const int directionToRight = 1;

class Screen;
class Player;
class Enemy;
class Enemy2;
class Bullet;
class Laser;
class Mine;

/*
  C (재사용하기 위해서 a part of ....)
  --> C++ (상속: 재사용하기 위한 관점 ---> 다중 상속)
  --> 상속: is a 관계 표현하는 것이다!!! 단일 상속
  --> java (90중반 SUN james gosling: Object-oriented Language) -> oracle (java)

	  struct Player extends GameObject { ... }
		---> google (android -> mobile platform) -----------------------> kotlin
	  --> MS C# : struct Player : GameObject { ... }

   C++: class ScannerPrinter : public Printer, public IScanner

   java: class ScannerPrinter extends Printer, implements IScanner
   C#: class ScannerPrinter : Printer, IScanner


   polymorphism (다형성)
	- upcasting
	- downcasting
		static_cast
		dynamic_cast
		reinterpret_cast

   encapsulation : information hiding
*/

class Screen {
private:
	int		size;
	char*	canvas;

public:

	// constructor (생성자 함수) 메모리공간상에 적재되는 순간 호출되는
	Screen(unsigned int size)
	{
		if (size == 0) size = 80;
		this->size = size;
		canvas = new char[size + 1];
	}

	void clear()
	{
		memset(canvas, ' ', size);
	}
	void draw(int pos, const char* face)
	{
		strncpy(&(canvas[pos]), face, strlen(face));
	}
	void draw(int pos, char face)
	{
		if (pos < 0 || pos >= size) return;
		canvas[pos] = face;
	}
	void render()
	{
		canvas[size] = '\0';  // render screen
		printf("%s\r", canvas);
	}
	bool isInRange(Bullet* bullet);

	// destructor (소멸자 함수) 메모리공간상에서 없어지는 순간 호출되는 함수
	virtual ~Screen()
	{
		delete[] canvas;
		canvas = nullptr;
		size = 0;
	}
};
class GameObject
{
private:
	char	face[20];
	int		pos;
	int		direction;
	Screen* screen;
	GameObject** gameObjects;

public:

	GameObject(GameObject** gameObjects, Screen* screen, const char* face, int pos, int direction) 
		: pos(pos), direction(direction), screen(screen), gameObjects(gameObjects)
	{
		setFace(face);
	}
	virtual ~GameObject() {}

	void move(int direction)
	{
		direction == directionToRight ? pos++: pos--;
	}
	void move()
	{
		(direction == directionToLeft) ? --pos : ++pos;
	}
	virtual void draw()
	{
		screen->draw(pos, face);
	}
	virtual void update() {}

	int getPos() { return pos; } // getter function
	void setPos(int pos) { this->pos = pos; } // setter function

	int getDirection() { return direction;  }
	void setDirection(int direction) { this->direction = direction; }

	const char* getFace() { return face;  }
	void setFace(const char* face) { strcpy(this->face, face); }

	Screen* getScreen() { return screen; }
	GameObject** getGameObjects() { return gameObjects; }
};
class Player : public GameObject {
private:
	int			nRemaining;
	char*		originalFace;

	Bullet*		find_unused_bullet();
	Mine*		find_unused_mine();
	

public:
	
	Player(GameObject** gameObjects, Screen* screen, const char* face, int pos);
	void fire();
	void fireLaser();
	void fireMine();
	void draw() override;
	void update() override;
	void onEnemyHit()
	{
		setFace("\\(^_^)/");
		nRemaining = 30;
	}
	Enemy* find_closest_enemy();
	~Player() override {
		delete[] originalFace;
	}
};
class Enemy : public GameObject {
protected:
	int		nRemaining;
	int     nMovementInterval;
	float   fPos;
	char	originalFace[20];
public:

	Enemy(GameObject** gameObjects, Screen* screen, const char* face, int pos)
		: GameObject(gameObjects, screen, face, pos, directionToLeft), 
		nRemaining(0), nMovementInterval(1), fPos(pos)
	{	
		strcpy(originalFace, face);
	}

	void update() override
	{
		int movement = rand() % 3 - 1;
		fPos += movement * 0.3f;
		setPos((int)fPos);

		if (nRemaining == 0) return;
		--nRemaining;
		if (nRemaining == 0) setFace(originalFace);
	}
	bool isHit(Bullet* bullet);
	void onHit()
	{
		setFace("(T_T)");
		nRemaining = 10;
	}
};
class Enemy2 : public Enemy {
private:
	int nMoveRemaining;
public:

	Enemy2(GameObject** gameObjects, Screen* screen, const char* face, int pos)
		: Enemy(gameObjects, screen, face, pos),
		nMoveRemaining(10)

	{}
	~Enemy2() override {}
	void update() override
	{
		GameObject** gameObjects = getGameObjects();
		Player* player = nullptr;
		for (int i = 0; i < 83; i++)
		{
			GameObject* obj = gameObjects[i];
			player = dynamic_cast<Player*>(obj);
			if (player != nullptr)
				break;
		}
		if (getPos() < player->getPos())
			setDirection(directionToRight);
		else
			setDirection(directionToLeft);
		if (nMoveRemaining > 0)
		{
			nMoveRemaining--;
			if (nMoveRemaining == 0)
			{
				if (getDirection() == directionToRight)
					setPos(getPos() + 1);
				else
					setPos(getPos() - 1);
				nMoveRemaining = 10;
			}
		}
		if (nRemaining == 0) return;
		--nRemaining;
		if (nRemaining == 0) setFace(originalFace);
	}

};
class Bullet : public GameObject {
private:
	bool	isReady;
	int		bombPos;
	void makeReady()
	{
		isReady = true;
	}
	int nBombRemaining;

public:
	
	Bullet(GameObject** gameObjects, Screen* screen)
		: GameObject(gameObjects, screen, "-", 0, directionToLeft),
		isReady{ true },
		nBombRemaining(0),
		bombPos(0)
	{}

	~Bullet() override {}
	virtual void setFire(Player* player, Enemy* enemy)
	{
		isReady = false; // inUse

		// direction 설정
		int enemy_pos = enemy->getPos();
		int player_pos = player->getPos();
		const char* player_face = player->getFace();

		setDirection(directionToLeft);
		if (player_pos < enemy_pos) setDirection(directionToRight);

		// bullet position 설정
		setPos(player_pos);
		if (getDirection() == directionToRight) 
			setPos( getPos() + (strlen(player_face) - 1));
	}
	
	void draw() override
	{
		if (nBombRemaining > 0) getScreen()->draw(bombPos,"@");
		if (isReady == true) return;
		GameObject::draw();
	}
	void update() override
	{
		if (nBombRemaining > 0) nBombRemaining--;
		if (isReady == true) return;
		move();
		GameObject** gameObjects = getGameObjects();

		Player* player = nullptr;
		for (int i = 0; i < 83; i++)
		{
			GameObject* obj = gameObjects[i];
			player = dynamic_cast<Player *>(obj);
			if (player != nullptr)
				break;
		}
		
		for (int i = 0; i < 83; i++)
		{
			GameObject* obj = gameObjects[i];
			Enemy* enemy = dynamic_cast<Enemy *>(obj);
			if (enemy == nullptr) continue;
			
			// enemy != nullptr
			if (enemy->isHit(this) == false) continue;
			enemy->onHit();
			if (player != nullptr)
				player->onEnemyHit();
			makeReady();
			bomb();
			break;
		}

		Screen* screen = getScreen();
		if (!screen->isInRange(this)) makeReady();
	}
	void bomb()
	{
		int bombPercent = rand() % 2;
		if (bombPercent == 0) return;
		bombPos = getPos();
		nBombRemaining = 10;
	}
	bool isAvailable() { return isReady; }
	void setIsReady(bool isReady) { this->isReady = isReady; }

};

class Laser : public Bullet{
private:
	int length;
	char* face;
public:
	Laser(GameObject** gameObjects, Screen* screen) : Bullet(gameObjects, screen),
		length(0)
		
	{
		face = nullptr;
	}
	~Laser() override{} 
	void setFire (Player* player, Enemy* enemy) override
	{
		
		setIsReady(false);
		// direction 설정
		int enemy_pos = enemy->getPos();
		int player_pos = player->getPos();
		const char* player_face = player->getFace();
		const char* enemy_face = enemy->getFace();

		setDirection(directionToLeft);
		length = player_pos - (enemy_pos + strlen(enemy_face))   ;
		setPos(enemy_pos + strlen(enemy_face) );
		if (player_pos < enemy_pos)
		{
			setDirection(directionToRight);
			length = enemy_pos - (player_pos + strlen(player_face))  ;
			setPos(player_pos + strlen(player_face) - 1);
		} 
	}
	void update() override
	{
		GameObject** gameObjects = getGameObjects();

		Player* player = nullptr;
		for (int i = 0; i < 83; i++)
		{
			GameObject* obj = gameObjects[i];
			player = dynamic_cast<Player*>(obj);
			if (player != nullptr)
				break;
		}
		Enemy* enemy = player-> find_closest_enemy();

		if (isAvailable() == false)
		{
			enemy->onHit();
			if (player != nullptr)
				player->onEnemyHit();
		}
	}
	void draw() override
	{
		if (isAvailable() == true) return;
		char* laserFace = new char[length+1];
		for (int i = 0;i < length;i++)
		{
			laserFace[i] = '-';
		}
		laserFace[length] = '\0';
		face = laserFace;
		//GameObject::draw();
		getScreen()->draw(getPos(), face);
		setIsReady(true);
	}
};
class Mine : public Bullet {
private:

public:
	Mine(GameObject** gameObjects, Screen* screen, const char* face) : Bullet(gameObjects, screen)
	{
		setFace(face);
	}
	~Mine() override {}
	void update() override
	{
		if (isAvailable() == true) return;
		GameObject** gameObjects = getGameObjects();

		Player* player = nullptr;
		for (int i = 0; i < 83; i++)
		{
			GameObject* obj = gameObjects[i];
			player = dynamic_cast<Player*>(obj);
			if (player != nullptr)
				break;
		}

		for (int i = 0; i < 83; i++)
		{
			GameObject* obj = gameObjects[i];
			Enemy* enemy = dynamic_cast<Enemy*>(obj);
			if (enemy == nullptr) continue;

			// enemy != nullptr
			if (enemy->isHit(this) == false) continue;
			enemy->onHit();
			if (player != nullptr)
				player->onEnemyHit();
			setIsReady(true);
			bomb();
			break;
		}

	}
	

};


// forward declaration 전방위 선언
// function prototype declaration 함수 원형 선언
bool Screen::isInRange(Bullet* bullet)
{
	int bullet_pos = bullet->getPos();
	return bullet_pos >= 0 && bullet_pos < size;
}
Player::Player(GameObject** gameObjects, Screen* screen, const char* face, int pos)
	: GameObject(gameObjects, screen, face, pos, directionToRight), nRemaining(0)
{
	originalFace = new char[20];
	strcpy(originalFace, face);
}
void Player::fire()
{
	Bullet* bullet = find_unused_bullet();
	if (bullet == nullptr) return;
	Enemy* enemy = find_closest_enemy();
	if (enemy == nullptr) return;
	bullet->setFire(this, enemy);
}
void Player::fireLaser()
{
	GameObject** gameObjects = getGameObjects();
	Laser* laser=nullptr;
	for (int i = 0; i < 87; i++)
	{
		GameObject* obj = gameObjects[i];
		laser = dynamic_cast<Laser*>(obj);
		if (laser != nullptr) break;
		//if (laser->isAvailable() == true)
	}
	Enemy* enemy = find_closest_enemy();
	if (enemy == nullptr) return;
	laser->setFire(this, enemy);
}
void Player::fireMine()
{

	Mine* mine = find_unused_mine();
	if (mine == nullptr) return;
	Enemy* enemy = find_closest_enemy();
	if (enemy == nullptr) return;
	mine->setFire(this, enemy);
}
Bullet* Player::find_unused_bullet()
{
	GameObject** gameObjects = getGameObjects();
	for (int i = 0; i < 87; i++)
	{
		GameObject* obj = gameObjects[i];
		Bullet* bullet = dynamic_cast<Bullet *>(obj);
		if (bullet == nullptr) continue;		
		if (bullet->isAvailable() == true) 
			return bullet;
	}
	return nullptr;
}
Mine* Player::find_unused_mine()
{
	GameObject** gameObjects = getGameObjects();
	for (int i = 0; i < 87; i++)
	{
		GameObject* obj = gameObjects[i];
		Mine* mine = dynamic_cast<Mine*>(obj);
		if (mine == nullptr) continue;
		if (mine->isAvailable() == true)
			return mine;
	}
	return nullptr;
}
Enemy* Player::find_closest_enemy() 
{
	GameObject** gameObjects = getGameObjects();
	Enemy* closest = nullptr;
	for (int i = 0; i < 84; i++) {
		GameObject* obj = gameObjects[i];
		Enemy* enemy = dynamic_cast<Enemy *>(obj);
		if (enemy == nullptr) continue;		
		if (closest == nullptr) {
			closest = enemy;
			continue;
		}
		// closest != nullptr
		int player_pos = getPos();
		int enemy_pos = enemy->getPos();
		int closest_pos = closest->getPos();
		if (abs(player_pos - enemy_pos) < abs(player_pos - closest_pos)) {
			closest = enemy;
		}
	}
	return closest;
}
void Player::draw()
{
	GameObject::draw();
}
void Player::update()
{
	if (nRemaining == 0) return;
	--nRemaining;
	if (nRemaining == 0) setFace(originalFace);
}
bool Enemy::isHit(Bullet* bullet)
{
	int bullet_direction = bullet->getDirection();
	int bullet_pos = bullet->getPos();
	return bullet_pos >= getPos() && bullet_pos < getPos() + strlen(getFace()) - 1;
}

int main()
{	
	int major;
	int minor;

	Screen  screen(80);
	GameObject* gameObjects[80+1+2+1+3]; // game object pool

	gameObjects[0] = new Player(gameObjects, &screen, "(-_-)", 50);
	gameObjects[1] = new Enemy(gameObjects, &screen, "(`_#)", 10);
	gameObjects[2] = new Enemy2(gameObjects, &screen, "(*_*)", 30);
	for (int i = 0; i < 80; i++)
		gameObjects[i+3] = new Bullet(gameObjects, &screen);
	gameObjects[83] = new Laser(gameObjects, &screen);
	for (int i = 84; i < 87; i++)
		gameObjects[i] = new Mine(gameObjects, &screen, "■");

	// game loop

	bool isLooping = true;
	while (isLooping) {
		screen.clear();		  		

		for (int i = 0; i < 87; i++) gameObjects[i]->update();

		for (int i = 0; i < 87; i++) gameObjects[i]->draw();
		
		screen.render();
		Sleep(100);

		if (!_kbhit()) continue;

		major = _getch();
		switch (major) {
		case 'q':
			isLooping = false;
			break;

		case ' ':
			(static_cast<Player *>(gameObjects[0]))->fire();
			break;
		case 'z':
			(static_cast<Player*>(gameObjects[0]))->fireLaser();
			break;
		case 'x':
			(static_cast<Player*>(gameObjects[0]))->fireMine();
			break;
		case 224: // arrow key, function key pressed
			minor = _getch();
			switch (minor) {
			case 75: // left
				gameObjects[0]->move(directionToLeft); // 				
				break;
			case 77: // right
				gameObjects[0]->move(directionToRight);
				break;
			case 72: // up
				gameObjects[1]->move(directionToLeft);
				break;
			case 80: // down
				gameObjects[1]->move(directionToRight);
				break;
			}
			break;
		}
	}
	printf("\nGame Over\n");

	for (int i = 0; i < 84; i++) delete gameObjects[i];

	return 0;
}