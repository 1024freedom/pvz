#include<stdio.h>
#include<graphics.h>//easyxͼ�ο�
#include<time.h>
#include<math.h>
#include<mmsystem.h>
#pragma comment(lib,"winmm.lib")
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
	int timer;//��ʱ����������ڶ�ã�
	float xoff;
	float yoff;//������������bar��Ծʱ��ƫ����
};
struct sunshineBall balls[10];//�����
IMAGE imgSunshineBall[29];//����֡
int sunshine;

struct zm {
	int x, y;
	int frameIndex;
	bool used;
	int speed;
};
struct zm zms[10];//��ʬ��
IMAGE imgZM[22];

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
	sunshine = 75;//��ʼ������ֵ

	//������Ϸ��ͼ�δ���
	initgraph(WIN_WIDTH, WIN_HEIGHT);

	//��������
	LOGFONT f;
	gettextstyle(&f);
	f.lfHeight = 30;
	f.lfWeight = 15;
	strcpy(f.lfFaceName, "Segoe UI Black");
	f.lfQuality = ANTIALIASED_QUALITY;//�����Ч��
	settextstyle(&f);
	setbkmode(TRANSPARENT);
	setcolor(BLACK);

	//��ʼ����ʬ����
	memset(zms, 0, sizeof(zms));
	for (int i = 0;i < 22;i++) {
		sprintf_s(name, sizeof(name), "res/zm/%d.png", i + 1);
		loadimage(&imgZM[i], name);
	}
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

	//��Ⱦ������
	int ballMax = sizeof(balls) / sizeof(balls[0]);
	for (int i = 0;i < ballMax;i++) {
		if (balls[i].used||balls[i].xoff) {
			IMAGE* img = &imgSunshineBall[balls[i].frameIndex];
			putimagePNG(balls[i].x, balls[i].y, img);

		}
	}
	//��Ⱦ������ѡ���ķ�Ծ
	
	//��Ⱦ����ֵ
	char scoreText[8];
	sprintf_s(scoreText, sizeof(scoreText), "%d", sunshine);
	outtextxy(285, 67, scoreText);//�������ֵ

	//��Ⱦ��ʬ
	int zmcnt = sizeof(zms) / sizeof(zms[0]);
	for (int i = 0;i < zmcnt;i++) {
		if (zms[i].used) {
			IMAGE* img = &imgZM[zms[i].frameIndex];
			putimagePNG(zms[i].x, zms[i].y-img->getheight(), img);
		}
	}
	EndBatchDraw();//����˫����
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
				balls[i].used = false;//�ռ�����
				mciSendString("play res/sunshine.mp3",0,0,0);
				//���÷��Ƶ�ƫ����
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
		if (msg.message == WM_LBUTTONDOWN) {//��갴��
			if (msg.x > 340 && msg.x < 340 + 72 * PLANT_COUNT && msg.y < 94) {
				int index = (msg.x - 340) / 72;
				status = 1;
				curPlant = index + 1;
			}
			else {
				collectSunshine(&msg);
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
	static int fre = 200;
	cnt++;
	if (cnt >= fre) {//������������������
		fre = 100 + rand() % 300;
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
		balls[i].timer = 0;
		balls[i].xoff = 0;
		balls[i].yoff = 0;
	}
}
void updateSunshine() {
	int ballMax = sizeof(balls) / sizeof(balls[0]);
	for (int i = 0;i < ballMax;i++) {
		if (balls[i].used) {
			balls[i].frameIndex=(balls[i].frameIndex+1)%29;//29֡
			if (balls[i].timer == 0) {
				balls[i].y += 2;
			}//����Ŀ�꽵��λ��ǰ
			if (balls[i].y >= balls[i].destY) {
				balls[i].timer++;
				if (balls[i].timer > 300) {
					balls[i].used = false;balls[i].used = false;
				}//������ʧ

			}//����Ŀ�꽵��λ�ú�������ʱ��
		}
		else if (balls[i].xoff) {
			float destY = 0;
			float destX = 262;
			float angle = atan((balls[i].y - destY) / (balls[i].x - destX));
			balls[i].xoff = 8 * cos(angle);
			balls[i].yoff = 8 * sin(angle);//ÿ�ζ�����һ�£��������
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
	//����λ��
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
void updateGame() {//������Ϸ����
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
	createSunshine();//��������
	updateSunshine();//��������״̬
	createZM();//���ɽ�ʬ
	updateZM();//���½�ʬ״̬
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