#include <iostream>
#include <graphics.h>

IMAGE background;
IMAGE plane_img[4];
IMAGE bullet_img[2];
IMAGE enemy_img[4];

//the piexel value of the image
enum PIXEL_INFO
{
	WIDTH = 591,
	HEIGHT = 864,
	NUM_BULLETS = 15,
	NUM_ENEMY = 10,
	BIG, SMALL
};

//information of the plane
struct Plane
{
	int x;
	int y;
	bool is_live;

	int width;
	int height;
	int hp;
	int type;
}player, bullets[NUM_BULLETS], enemy[NUM_ENEMY];

// A timer to track time
bool timer(int ms, int id)
{
	bool ret = false;
	static DWORD timeCnt[3];
	if (clock() - timeCnt[id] >= ms)
	{
		timeCnt[id] = clock();
		ret = true;
	}
	return ret;
}

//load the image source
void load()
{
	//pixel info: 480 X 612, load the image into backgroud(IMAGE object)
	loadimage(&background, "D:/Visual Studio/Aircraft_battle/res/images/background.jpg");
	//load the images into plane_img array
	loadimage(&plane_img[0], "D:/Visual Studio/Aircraft_battle/res/images/planeNormal_1.jpg");
	loadimage(&plane_img[1], "D:/Visual Studio/Aircraft_battle/res/images/planeNormal_2.jpg");
	loadimage(&plane_img[2], "D:/Visual Studio/Aircraft_battle/res/images/planeBoom_1.jpg");
	loadimage(&plane_img[3], "D:/Visual Studio/Aircraft_battle/res/images/planeBoom_2.jpg");
	loadimage(&bullet_img[0], "D:/Visual Studio/Aircraft_battle/res/images/bullet1.jpg");
	loadimage(&bullet_img[1], "D:/Visual Studio/Aircraft_battle/res/images/bullet2.jpg");
	loadimage(&enemy_img[0], "D:/Visual Studio/Aircraft_battle/res/images/enemy_1.jpg");
	loadimage(&enemy_img[1], "D:/Visual Studio/Aircraft_battle/res/images/enemy_2.jpg");
	loadimage(&enemy_img[2], "D:/Visual Studio/Aircraft_battle/res/images/enemyPlane1.jpg");
	loadimage(&enemy_img[3], "D:/Visual Studio/Aircraft_battle/res/images/enemyPlane2.jpg");
}

//draw the image
void draw()
{
	putimage(0, 0, &background);
	putimage(player.x, player.y, &plane_img[0], NOTSRCERASE);
	putimage(player.x, player.y, &plane_img[1], SRCINVERT);
	//draw bullet
	for (int i = 0; i < NUM_BULLETS; ++i)
	{
		if (bullets[i].is_live)
		{
			putimage(bullets[i].x, bullets[i].y, &bullet_img[0], NOTSRCERASE);
			putimage(bullets[i].x, bullets[i].y, &bullet_img[1], SRCINVERT);
		}
	}
	//draw enemy
	for (int i = 0; i < NUM_ENEMY; ++i)
	{
		if (enemy[i].is_live && enemy[i].type == SMALL)
		{
			putimage(enemy[i].x, enemy[i].y, &enemy_img[0], NOTSRCERASE);
			putimage(enemy[i].x, enemy[i].y, &enemy_img[1], SRCINVERT);
		}
		if (enemy[i].is_live && enemy[i].type == BIG)
		{
			putimage(enemy[i].x, enemy[i].y, &enemy_img[2], NOTSRCERASE);
			putimage(enemy[i].x, enemy[i].y, &enemy_img[3], SRCINVERT);
		}
	}
}

//function for initialzing the enemy
void initEnemy(int i)
{
	enemy[i].is_live = false;
	if (rand() % NUM_ENEMY == 0 || rand() % NUM_ENEMY == 1)
	{
		enemy[i].type = BIG;
		enemy[i].width = 104;
		enemy[i].height = 148;
		enemy[i].hp = 3;
	}
	else
	{
		enemy[i].type = SMALL;
		enemy[i].width = 52;
		enemy[i].height = 39;
		enemy[i].hp = 1;
	}
}

//initializing function
void init()
{
	//load all the needed image
	load();
	//initializing the player's position and status
	player.x = WIDTH / 2;
	player.y = HEIGHT - plane_img[0].getheight();
	player.hp = 10;
	player.is_live = true;
	player.height = plane_img[0].getheight();
	player.width = plane_img[0].getwidth();

	//initializing the bullet info
	for (int i = 0; i < NUM_BULLETS; ++i)
	{
		bullets[i].x = 0;
		bullets[i].y = 0;
		bullets[i].is_live = false;
	}

	//initializing the enemy plane info
	for (int i = 0; i < NUM_ENEMY; ++i)
		initEnemy(i);
}

//this function will create a bullet
void createBullet()
{
	for (int i = 0; i < NUM_BULLETS; ++i)
	{
		if (!bullets[i].is_live)
		{
			bullets[i].x = player.x + 45;
			bullets[i].y = player.y;
			bullets[i].is_live = true;
			break;
		}
	}
}

//function to creat enemy plane
void createEnemy()
{
	for (int i = 0; i < NUM_ENEMY; ++i)
	{
		if (!enemy[i].is_live)
		{
			enemy[i].x = rand() % (WIDTH - enemy[i].width);
			enemy[i].y = 0;
			enemy[i].is_live = true;
			enemy[i].hp = enemy[i].type == BIG ? 3 : 1;
			break;
		}
	}
}

//function to handle how enemy moves
void enemyMove(int speed)
{
	for (int i = 0; i < NUM_ENEMY; ++i)
	{
		if (enemy[i].is_live)
		{
			enemy[i].y += speed;
			if (enemy[i].y > HEIGHT)
			{
				enemy[i].is_live = false;
				--player.hp;
			}
		}
	}
}

//move function: control player move
void move(int speed)
{
	if (GetAsyncKeyState(VK_UP) || GetAsyncKeyState('W'))
		player.y = max(0, player.y - speed);
	if (GetAsyncKeyState(VK_DOWN) || GetAsyncKeyState('S'))
		player.y = min(HEIGHT - plane_img[0].getheight(), player.y + speed);
	if (GetAsyncKeyState(VK_LEFT) || GetAsyncKeyState('A'))
		player.x = max(-plane_img[0].getwidth() / 2, player.x - speed);
	if (GetAsyncKeyState(VK_RIGHT) || GetAsyncKeyState('D'))
		player.x = min(WIDTH - plane_img[0].getwidth() / 2, player.x + speed);

	//shoot
	static DWORD t1 = 0, t2 = 0;
	if (GetAsyncKeyState(VK_SPACE) && timer(100, 0))
		createBullet();
}

//bullet move function
void bulletMove()
{
	for (int i = 0; i < NUM_BULLETS; ++i)
	{
		if (bullets[i].is_live)
		{
			bullets[i].y -= 1;
			if (bullets[i].y < 0)
				bullets[i].is_live = false;
		}
	}
}

//this function will check both if the player plane crashed the enemy plane
//and if any buttle hits the enemy plane
void checkHit()
{
	for (int i = 0; i < NUM_ENEMY; ++i)
	{
		if (!enemy[i].is_live) continue;
		//check if the player and the enemy overlapped
		if ((player.x >= enemy[i].x && player.x <= enemy[i].x + enemy[i].width
			&& player.y >= enemy[i].y && player.y <= enemy[i].y + enemy[i].height) ||
			(player.x <= enemy[i].x && player.x + player.width >= enemy[i].x
				&& player.y <= enemy[i].y && player.y + player.height >= enemy[i].y))
		{
			player.hp -= enemy[i].hp;
			enemy[i].is_live = false;
		}
		//check if any bullet hits the enemy plane
		for (int j = 0; j < NUM_BULLETS; ++j)
		{
			if (!bullets[j].is_live) continue;
			if (bullets[j].x >= enemy[i].x && bullets[j].x <= enemy[i].x + enemy[i].width
				&& bullets[j].y >= enemy[i].y && bullets[j].y <= enemy[i].y + enemy[i].height)
			{
				bullets[j].is_live = false;
				if (--enemy[i].hp == 0)
					enemy[i].is_live = false;
			}
		}
	}
}

int main()
{
	//creating a window and initialize the window size
	//the window size is corresponding to the background image pixel, in our case 480 X 612
	initgraph(WIDTH, HEIGHT);
	init();

	BeginBatchDraw();
	while (player.hp > 0)
	{
		draw();
		FlushBatchDraw();
		move(1);
		bulletMove();
		if (timer(500, 1))
			createEnemy();
		if(timer(25, 2))
			enemyMove(1);
		checkHit();
	}
	EndBatchDraw();
	putimage(player.x, player.y, &plane_img[2], NOTSRCERASE);
	putimage(player.x, player.y, &plane_img[3], SRCINVERT);

	while (!GetAsyncKeyState(VK_ESCAPE));

	return 0;
}