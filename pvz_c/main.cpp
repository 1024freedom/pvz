#include<stdio.h>
#include<graphics.h>//easyxͼ�ο�
#include<time.h>
#include"tools.h"
#define WIN_WIDTH 900
#define WIN_HEIGHT 600
IMAGE imgBg;//����ͼƬ
IMAGE imgBar;//������

int curX, curY;//ѡ�е�ֲ�����ƶ������е�λ��
int curPlant = 0;//��ǰѡ��ֲ�����ţ���1��ʼ��

struct Plant {
	int type;//��curPlant����һ��
	int framIndex;//��̬֡���
};
struct Plant map[3][9];

struct sunshineBall {
	int x, y;//Ʈ������е�����
	int frameIndex;//��ǰ��ʾ��ͼƬ֡���
	int destY;//����Ʈ���Ŀ��λ�õ�y����
	bool used;//�Ƿ�����ʾ
};
struct sunshineBall balls[10];//��
IMAGE imgSunshineBall[29];//����֡


enum{//ö�ٽ���ֲ�����
	SunFlower,
	PeaShooter,
	PLANT_COUNT
};
IMAGE imgCard[PLANT_COUNT];
IMAGE* imgPlant[PLANT_COUNT][20];//ֲ����ֲ�����̬��ָ�����飩

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
	//������Ϸ����ͼƬ
	//���ַ����޸�Ϊ���ֽ��ַ���
	loadimage(&imgBg,"res/bg.jpg");
	loadimage(&imgBar, "res/bar5.png");

	memset(imgPlant, 0, sizeof(imgPlant));
	memset(map, 0, sizeof(map));
	memset(balls, 0, sizeof(balls));

	char name[64];
	for (int i = 0;i < PLANT_COUNT;i++) {
		sprintf_s(name, sizeof(name), "res/Cards/card_%d.png", i + 1);
		//ֲ�￨�Ƶ��ļ�·��
		loadimage(&imgCard[i], name);

		for (int j = 0;j < 20;j++) {//ֲ����ֲ�����̬�Ͷ���
			sprintf_s(name, sizeof(name), "res/zhiwu/%d/%d.png", i, j + 1);
			//���ж��ļ��Ƿ����
			if (fileExist(name)) {
				imgPlant[i][j] = new IMAGE;//�������ڴ�ᵼ�¼��ؿ�ָ�������
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
	srand(time(NULL));//�����������

	//������Ϸ��ͼ�δ���
	initgraph(WIN_WIDTH, WIN_HEIGHT);

}

void updateWindow() {

	BeginBatchDraw();//��ʼ����(˫����������ˢ�µ�˺�Ѹ�)

	//��ȾͼƬ
	putimage(0, 0, &imgBg);//ͼƬ���Ͻǵ�����(y�ᳯ�£�ԭ���Ǵ������Ͻ�)
	putimagePNG(260, 0, &imgBar);
	for (int i = 0;i < PLANT_COUNT;i++) {
		int x = 340 + i * 72;
		int y = 4;
		putimagePNG(x, y, &imgCard[i]);
	}


	//��Ⱦ��ֲ���ֲ��
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
	//��Ⱦ�϶����̵�ֲ��
	if (curPlant) {
		IMAGE* img = imgPlant[curPlant - 1][0];
		putimagePNG(curX - img->getwidth() / 2, curY - img->getheight() / 2, img);
		//ʹ�϶�ʱ������
	}

	EndBatchDraw();//����˫����
}

void userClick() {
	ExMessage msg;
	static int status = 0;
	if (peekmessage(&msg)) {
		if (msg.message == WM_LBUTTONDOWN) {//��갴��
			if (msg.x > 340 && msg.x < 340 + 72 * PLANT_COUNT && msg.y < 94) {
				int index = (msg.x - 340) / 72;
				status = 1;
				curPlant = index + 1;
			}
		}
		else if (msg.message == WM_MOUSEMOVE && status) {//����ƶ�
			curX = msg.x;
			curY = msg.y;
		}
		else if (msg.message = WM_LBUTTONUP && status) {//���̧��
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
	if (cnt >= fre) {//������������������
		fre = 200 + rand() % 200;
		cnt = 0;
		//���������ȡһ������ʹ�õ�
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
	int timer = 0;//����֡��
	bool flag = true;
	while (1) {//ѭ��ˢ����ʵ�ֽ���
		userClick();
		timer += getDelay();//���ϴ�ˢ�µ�ʱ���
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