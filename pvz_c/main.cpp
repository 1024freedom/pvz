#include<stdio.h>
#include<graphics.h>//easyx图形库
#include"tools.h"
#define WIN_WIDTH 900
#define WIN_HEIGHT 600
IMAGE imgBg;//背景图片
IMAGE imgBar;//工具栏

enum{//枚举进行植物计数
	SunFlower,
	PeaShooter,
	PLANT_COUNT
};
IMAGE imgCard[PLANT_COUNT];
void gameInit() {
	//加载游戏背景图片
	//把字符集修改为多字节字符集
	loadimage(&imgBg,"res/bg.jpg");
	loadimage(&imgBar, "res/bar5.png");

	char name[64];
	for (int i = 0;i < PLANT_COUNT;i++) {
		sprintf_s(name, sizeof(name), "res/Cards/card_%d.png", i + 1);
		//植物卡牌的文件路径
		loadimage(&imgCard[i], name);
	}

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

	EndBatchDraw();//结束双缓冲
}

void userClick() {
	ExMessage msg;
	if (peekmessage(&msg)) {
		if (msg.message == WM_LBUTTONDOWN) {//鼠标按下
			if (msg.x > 340 && msg.x < 340 + 72 * PLANT_COUNT && msg.y < 94) {
				int index = (msg.x - 340) / 72;
			}
		}
		else if (msg.message == WM_MOUSEMOVE) {//鼠标移动

		}
		else if (msg.message = WM_LBUTTONUP) {//鼠标抬起

		}
	}
}


int main() {
	gameInit();

	while (1) {//循环刷新以实现交互
		userClick();
		updateWindow();
	}
	system("pause");
	return 0;
}