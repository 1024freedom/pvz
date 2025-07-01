#include<stdio.h>
#include<graphics.h>//easyx图形库
#include<time.h>
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
};
struct sunshineBall balls[10];//池
IMAGE imgSunshineBall[29];//阳光帧


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

	//创建游戏的图形窗口
	initgraph(WIN_WIDTH, WIN_HEIGHT);

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

	EndBatchDraw();//结束双缓冲
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
	static int fre = 400;
	cnt++;
	if (cnt >= fre) {//控制阳光产生的随机性
		fre = 200 + rand() % 200;
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
	}
}
void updateGame() {
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
	createSunshine();
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