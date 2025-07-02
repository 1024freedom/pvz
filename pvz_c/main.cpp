#include<stdio.h>
#include<graphics.h>//easyx图形库
#include<time.h>
#include<math.h>
#include<mmsystem.h>
#pragma comment(lib,"winmm.lib")
#include"tools.h"
#define WIN_WIDTH 900
#define WIN_HEIGHT 600
IMAGE imgBg;//背景图片
IMAGE imgBar;//工具栏

int curX, curY;//选中的植物在移动过程中的位置
int curPlant = 0;//当前选择植物的序号（从1开始）

struct Plant {
	int type;//与curPlant保持一致
	int framIndex;//姿态帧序号
};
struct Plant map[3][9];

struct sunshineBall {
	int x, y;//飘落过程中的坐标
	int frameIndex;//当前显示的图片帧序号
	int destY;//阳光飘落的目标位置的y坐标
	bool used;//是否在显示
	int timer;//计时器（阳光存在多久）
	float xoff;
	float yoff;//点击阳光球后往bar飞跃时的偏移量
};
struct sunshineBall balls[10];//阳光池
IMAGE imgSunshineBall[29];//阳光帧
int sunshine;

struct zm {
	int x, y;
	int frameIndex;
	bool used;
	int speed;
};
struct zm zms[10];//僵尸池
IMAGE imgZM[22];

enum{//枚举进行植物计数
	SunFlower,
	PeaShooter,
	PLANT_COUNT
};
IMAGE imgCard[PLANT_COUNT];
IMAGE* imgPlant[PLANT_COUNT][20];//植物种植后的姿态（指针数组）

bool fileExist(const char* name) {
	FILE* fp = fopen(name, "r");
	if (fp == NULL) {
		return false;
	}
	else {
		fclose(fp);
		return true;
	}
}
void gameInit() {
	//加载游戏背景图片
	//把字符集修改为多字节字符集
	loadimage(&imgBg,"res/bg.jpg");
	loadimage(&imgBar, "res/bar5.png");

	memset(imgPlant, 0, sizeof(imgPlant));
	memset(map, 0, sizeof(map));
	memset(balls, 0, sizeof(balls));

	char name[64];
	for (int i = 0;i < PLANT_COUNT;i++) {
		sprintf_s(name, sizeof(name), "res/Cards/card_%d.png", i + 1);
		//植物卡牌的文件路径
		loadimage(&imgCard[i], name);

		for (int j = 0;j < 20;j++) {//植物种植后的姿态和动作
			sprintf_s(name, sizeof(name), "res/zhiwu/%d/%d.png", i, j + 1);
			//先判断文件是否存在
			if (fileExist(name)) {
				imgPlant[i][j] = new IMAGE;//不分配内存会导致加载空指针而崩溃
				loadimage(imgPlant[i][j], name);
			}
			else {
				break;
			}
		}
	}
	for (int i = 0;i < 29;i++) {
		sprintf_s(name, sizeof(name), "res/sunshine/%d.png", i+1);
		loadimage(&imgSunshineBall[i], name);
	}
	srand(time(NULL));//配置随机种子
	sunshine = 75;//初始化阳光值

	//创建游戏的图形窗口
	initgraph(WIN_WIDTH, WIN_HEIGHT);

	//设置字体
	LOGFONT f;
	gettextstyle(&f);
	f.lfHeight = 30;
	f.lfWeight = 15;
	strcpy(f.lfFaceName, "Segoe UI Black");
	f.lfQuality = ANTIALIASED_QUALITY;//抗锯齿效果
	settextstyle(&f);
	setbkmode(TRANSPARENT);
	setcolor(BLACK);

	//初始化僵尸数据
	memset(zms, 0, sizeof(zms));
	for (int i = 0;i < 22;i++) {
		sprintf_s(name, sizeof(name), "res/zm/%d.png", i + 1);
		loadimage(&imgZM[i], name);
	}
}

void updateWindow() {

	BeginBatchDraw();//开始缓冲(双缓冲解决画面刷新的撕裂感)

	//渲染图片
	putimage(0, 0, &imgBg);//图片左上角的坐标(y轴朝下，原点是窗口左上角)
	putimagePNG(260, 0, &imgBar);
	for (int i = 0;i < PLANT_COUNT;i++) {
		int x = 340 + i * 72;
		int y = 4;
		putimagePNG(x, y, &imgCard[i]);
	}


	//渲染种植后的植物
	for (int i = 0;i < 3;i++) {
		for (int j = 0;j < 9;j++) {
			if (map[i][j].type) {
				int x = 256 + j * 81;
				int y = 179 + i * 102;
				int type = map[i][j].type - 1;
				int frameindex = map[i][j].framIndex;
				putimagePNG(x, y, imgPlant[type][frameindex]);
			}
		}
	}
	//渲染拖动过程的植物
	if (curPlant) {
		IMAGE* img = imgPlant[curPlant - 1][0];
		putimagePNG(curX - img->getwidth() / 2, curY - img->getheight() / 2, img);
		//使拖动时光标居中
	}

	//渲染阳光球
	int ballMax = sizeof(balls) / sizeof(balls[0]);
	for (int i = 0;i < ballMax;i++) {
		if (balls[i].used||balls[i].xoff) {
			IMAGE* img = &imgSunshineBall[balls[i].frameIndex];
			putimagePNG(balls[i].x, balls[i].y, img);

		}
	}
	//渲染阳光球被选择后的飞跃
	
	//渲染阳光值
	char scoreText[8];
	sprintf_s(scoreText, sizeof(scoreText), "%d", sunshine);
	outtextxy(285, 67, scoreText);//输出阳光值

	//渲染僵尸
	int zmcnt = sizeof(zms) / sizeof(zms[0]);
	for (int i = 0;i < zmcnt;i++) {
		if (zms[i].used) {
			IMAGE* img = &imgZM[zms[i].frameIndex];
			putimagePNG(zms[i].x, zms[i].y-img->getheight(), img);
		}
	}
	EndBatchDraw();//结束双缓冲
}
void collectSunshine(ExMessage* msg) {
	int cnt = sizeof(balls) / sizeof(sizeof(balls[0]));
	int width = imgSunshineBall[0].getwidth();
	int height = imgSunshineBall[0].getheight();
	
	for (int i = 0;i < cnt;i++) {
		if (balls[i].used) {
			int x = balls[i].x;
			int y = balls[i].y;
			if (msg->x > x && msg->x<x + width
				&& msg->y>y && msg->y < y + height) {
				balls[i].used = false;//收集阳光
				mciSendString("play res/sunshine.mp3",0,0,0);
				//设置飞移的偏移量
				float destY = 0;
				float destX = 262;
				float angle = atan((y - destY) / (x - destX));
				balls[i].xoff = 8 * cos(angle);
				balls[i].yoff = 8 * sin(angle);
			
			}
		}
	}
}
void userClick() {
	ExMessage msg;
	static int status = 0;
	if (peekmessage(&msg)) {
		if (msg.message == WM_LBUTTONDOWN) {//鼠标按下
			if (msg.x > 340 && msg.x < 340 + 72 * PLANT_COUNT && msg.y < 94) {
				int index = (msg.x - 340) / 72;
				status = 1;
				curPlant = index + 1;
			}
			else {
				collectSunshine(&msg);
			}
		}
		else if (msg.message == WM_MOUSEMOVE && status) {//鼠标移动
			curX = msg.x;
			curY = msg.y;
		}
		else if (msg.message = WM_LBUTTONUP && status) {//鼠标抬起
			if (msg.x > 256 && msg.y > 179 && msg.y < 489) {
				int row = (msg.y - 179) / 102;
				int column = (msg.x - 256) / 81;
				if (map[row][column].type == 0) {
					map[row][column].type = curPlant;
					map[row][column].framIndex = 0;
				}
			}
			status = 0;
			curPlant = 0;
		}
	}
}
void createSunshine() {
	static int cnt = 0;
	static int fre = 200;
	cnt++;
	if (cnt >= fre) {//控制阳光产生的随机性
		fre = 100 + rand() % 300;
		cnt = 0;
		//从阳光池中取一个可以使用的
		int ballMax = sizeof(balls) / sizeof(balls[0]);
		int i;
		for (i = 0;i < ballMax && balls[i].used;i++);
		if (i >= ballMax)return;
		balls[i].used = true;
		balls[i].frameIndex = 0;
		balls[i].x = 260 + rand() % (900 - 260);
		balls[i].y = 60;
		balls[i].destY = 200 + (rand() % 4) * 90;
		balls[i].timer = 0;
		balls[i].xoff = 0;
		balls[i].yoff = 0;
	}
}
void updateSunshine() {
	int ballMax = sizeof(balls) / sizeof(balls[0]);
	for (int i = 0;i < ballMax;i++) {
		if (balls[i].used) {
			balls[i].frameIndex=(balls[i].frameIndex+1)%29;//29帧
			if (balls[i].timer == 0) {
				balls[i].y += 2;
			}//到达目标降落位置前
			if (balls[i].y >= balls[i].destY) {
				balls[i].timer++;
				if (balls[i].timer > 300) {
					balls[i].used = false;balls[i].used = false;
				}//阳光消失

			}//到达目标降落位置后启动计时器
		}
		else if (balls[i].xoff) {
			float destY = 0;
			float destX = 262;
			float angle = atan((balls[i].y - destY) / (balls[i].x - destX));
			balls[i].xoff = 8 * cos(angle);
			balls[i].yoff = 8 * sin(angle);//每次都更新一下，减少误差
			balls[i].x -= balls[i].xoff;
			balls[i].y -= balls[i].yoff;
			if (balls[i].y < 0 || balls[i].x < 262) {
				balls[i].xoff = 0;
				balls[i].yoff = 0;
				sunshine += 25;
			}
		}
	}
}
void createZM() {
	static int cnt = 0;
	static int zmFre = 300;
	cnt++;
	if (cnt > zmFre) {
		cnt = 0;
		zmFre = 300 + rand() % 150;
		int i;
		int zmMax = sizeof(zms) / sizeof(zms[0]);
		for (i = 0;i < zmMax && zms[i].used;i++);
		if (i < zmMax) {
			zms[i].used = true;
			zms[i].x = WIN_WIDTH;
			zms[i].y = 172 + (1 + rand() % 3) * 100;
			zms[i].speed = 1;
		}
	}
}
void updateZM() {
	//更新位置
	int zmMax = sizeof(zms) / sizeof(zms[0]);
	for (int i = 0;i < zmMax;i++) {
		if (zms[i].used) {
			zms[i].frameIndex = (zms[i].frameIndex + 1) % 22;
			zms[i].x -= zms[i].speed;
			if (zms[i].x < 165) {
				printf("game over");
				MessageBox(NULL, "over", "over", 0);
				exit(0);
			}
		}
	}
}
void updateGame() {//更新游戏数据
	for (int i = 0;i < 3;i++) {
		for (int j = 0;j < 9;j++) {
			if (map[i][j].type) {
				map[i][j].framIndex++;
				int type = map[i][j].type - 1;
				int frameindex = map[i][j].framIndex;
				if (imgPlant[type][frameindex] == NULL) {
					map[i][j].framIndex = 0;
				}
			}
		}
	}
	createSunshine();//创建阳光
	updateSunshine();//更新阳光状态
	createZM();//生成僵尸
	updateZM();//更新僵尸状态
}

void startUI() {
	IMAGE imgBg, imgMenu1, imgMenu2;
	loadimage(&imgBg, "res/menu.png");
	loadimage(&imgMenu1, "res/menu1.png");
	loadimage(&imgMenu2, "res/menu2.png");
	int flag = 0;

	while (1) {
		BeginBatchDraw();
		putimage(0, 0, &imgBg);
		putimagePNG(475, 75, flag ? &imgMenu1 : &imgMenu2);
		ExMessage msg;
		if (peekmessage(&msg)) {
			if (msg.message == WM_LBUTTONDOWN 
					&& msg.x>475 && msg.x<475+325 
					&& msg.y>75 && msg.y<140) {
				flag = 1;
			}
			else if (msg.message == WM_LBUTTONUP && flag) {
				return;
			}
		}
		EndBatchDraw();
	}
}
int main() {
	gameInit();
	startUI();
	int timer = 0;//控制帧率
	bool flag = true;
	while (1) {//循环刷新以实现交互
		userClick();
		timer += getDelay();//与上次刷新的时间差
		if (timer > 55) {
			flag = true;
			timer = 0;
		}
		if (flag) {
			flag = false;
			updateWindow();
			updateGame();
		}

	}

	system("pause");
	return 0;
}