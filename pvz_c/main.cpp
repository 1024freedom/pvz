#include<stdio.h>
#include<graphics.h>//easyxͼ�ο�
#include"tools.h"
#define WIN_WIDTH 900
#define WIN_HEIGHT 600
IMAGE imgBg;//����ͼƬ
IMAGE imgBar;//������

enum{//ö�ٽ���ֲ�����
	SunFlower,
	PeaShooter,
	PLANT_COUNT
};
IMAGE imgCard[PLANT_COUNT];
void gameInit() {
	//������Ϸ����ͼƬ
	//���ַ����޸�Ϊ���ֽ��ַ���
	loadimage(&imgBg,"res/bg.jpg");
	loadimage(&imgBar, "res/bar5.png");

	char name[64];
	for (int i = 0;i < PLANT_COUNT;i++) {
		sprintf_s(name, sizeof(name), "res/Cards/card_%d.png", i + 1);
		//ֲ�￨�Ƶ��ļ�·��
		loadimage(&imgCard[i], name);
	}

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

	EndBatchDraw();//����˫����
}

void userClick() {
	ExMessage msg;
	if (peekmessage(&msg)) {
		if (msg.message == WM_LBUTTONDOWN) {//��갴��
			if (msg.x > 340 && msg.x < 340 + 72 * PLANT_COUNT && msg.y < 94) {
				int index = (msg.x - 340) / 72;
			}
		}
		else if (msg.message == WM_MOUSEMOVE) {//����ƶ�

		}
		else if (msg.message = WM_LBUTTONUP) {//���̧��

		}
	}
}


int main() {
	gameInit();

	while (1) {//ѭ��ˢ����ʵ�ֽ���
		userClick();
		updateWindow();
	}
	system("pause");
	return 0;
}