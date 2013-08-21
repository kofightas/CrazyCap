#ifndef __CRAZYCAP
#define __CRAZYCAP

#include "cocos2d.h"
#include "tools.h"

using namespace cocos2d;

struct  BlockSprite
{
	CCSprite * pSprite;
	CCSprite * pShadowSprite;
	CCSpriteFrame * pDefaultSpriteFrame;
	BlockSprite():pSprite(NULL), pDefaultSpriteFrame(NULL){}
};

struct Block{
	int type;
	int x_move;
	int y_move;
	int alpha;
	int special;		//0 for normal , 1 for bomb , 2  for can't eat , 3 for can't move
	bool isDropping;
	bool isCanMove;
	BlockSprite * pBlockSprite;
	bool SelectAble(){

		if (isCanMove==false||
			special==2||
			special==3||
			alpha<255||
			isDropping==true||
			type==-1||
			x_move>0||
			y_move>0)
		{
			return false;
		}
		return true;
	}

	Block():pBlockSprite(NULL){}
};

struct Position{
	int x;
	int y;
};

struct Explode
{
	Position p;
	int startTime;
};

struct Paticle {
	int xSpeed;
	int ySpeed;
	int startX;
	int startY;
	int x;
	int y;
	int type;
	int rotateSpeed;
	BlockSprite * pBlockSprite;
};

struct PaticleSystem {
	Paticle p;
	int startTime;
	PaticleSystem *next;
};

class CrazyCapGame
{
public:
	CrazyCapGame();
	void Update();
	void Init();

	Position m_selectedPos[2];		//��¼���ε��ѡ��ķ���
	void LoadRes(int order);
	int m_blockBuffer[8][168];
	Block m_blocks[8][16];
	int m_progress;
	int m_gameState;				//0==�ȴ���ʼ,1==��Ϸ������,2==��Ϸ����
	bool m_bufferNull;
	bool m_isPlayExplodeSound;
	Position tempPos;
	int m_gameMode;					//0 == ��ͨ  �� 1==����  
	int m_items[3];					//���ӵ�еĵ��ߣ�-1�����޵���

	static const int gameAreaX=0;		//��Ϸ�������Ͻ�����
	static const int gameAreaY=100;		//��Ϸ�������Ͻ�����
	static const int gameAreaWidth=480;	//��Ϸ������
	static const int gameAreaHeight=510;//��Ϸ����߶�
	static const int BLOCKWIDTH=60;

	int m_windowHeight;

	cocos2d::CCLayer * m_curLayer;
public:
	int AddAItem(int itemType);		//Ϊ����������һ�����ߣ����ص���λ��
	bool Help(Position &position1,Position &position2);				//��������ҵ�һ�Կ����Ŀ�,�ҵ��Ļ�����true,���򷵻�false
	int ExchangeBlock(Position &position1,Position &position2);
	Block *GetBlock(Position &position);
	bool EatAllLine();			//������Ļ�����п����Ŀ飬������,�п����Ŀ�Ļ�����true,���򷵻�false
	bool DropDown();			//���з�����������һ������з������£�����true,���򷵻�false;	
	void MakeProgress(int comboCount);		//���к���������Progress�ı仯
	void CleanRoom();
	void StartGame();							//��Ϸ��ʼ
	void OnTouchBegin(int x, int y);
	void OnTouchEnd(int x, int y);
	void OnTouchMove(int x, int y);				//�ƶ�
protected:
	bool IsHaveLine(Position position);		//�ж�ĳ���Ƿ���һ�������ı�ʯ������
	void EatBlock(Position position);			//��������
	void BombEatBlock(Position position);
	bool EatLine(Position position);			//����position�漰�ļ�������һ��ķ���
	void ResortBlocks();		//������Ļ�ϵķ���
	void SystemExchangeBlock(int x1, int y1, int x2, int y2);	//����ʱϵͳ������������
	void PlaySound(int soundID);
	void TimerWork();
	void PlayAni(int x, int y, int dir, bool loop = false);
	bool CheckTwoPosLinked(Position position1, Position position2);
	void MoveBlockPair(Position position1, Position position2);		//�ƶ�����
	Position GetPosition(int x,int y){Position p;p.x=x;p.y=y;return p;}//���һ��Position
	Position GoTo(Position pos,int direction,int step);		//direction 0 for up,1 for up-right, 2 for down right, 3 for down, 4 for left down, 5 for left up

	void CallBackND(CCNode * sender, void * data);
protected:
	BlockSprite * GetFreeBlockSprite(int type);
	void MakeSpriteFree(int type, BlockSprite * pBlockSprite);
	void OnDropDownStart();

	void CreatePaticleSystem(int x, int y,int startTime);		//direction 0 for left,1 for topleft,2 for top,and so on...
	void DeleteANodeFormPaticleSystem(PaticleSystem *p);				//

	void SetSelectAni(int x, int y);

	std::map<int, std::list<BlockSprite *> >   m_availableSprite;				//������ſ��õ�Sprite
	std::map<int, std::string> m_soundEffectMap;

	unsigned int m_totalClickCount;		//�ܹ��������
	unsigned int m_workedClickCount;	//��Ч�������
	unsigned int m_maxComboCount;		//���combo��
	long m_lastClickTime;				//�ϴε��ʱ��
	long m_rhythmMark;				//����е÷�
	std::vector <long> m_perClickTakeTime;	//ÿ�ε������ʱ���¼
	unsigned int m_rpMark;				//��Ʒ�÷�
	unsigned int m_totalComboCount;		//combo�ܴ���
	int direction1,direction2;			//���������ƶ��Ŀ��ƶ��ķ���

	//��ʱ��
	Timer *timerMoveBlock;
	Timer *timerEatBlock;
	Timer *timerDropDown;
	Timer *timerCombo;
	Timer *timerComboCount;
	Timer *timerPaticles;
	Timer *timerGameEnd;
	Timer *m_timerGame;

	//���鶯��
	CCAnimation *m_blockMoveAni[6][6];			//���ַ��������������ƶ��Ķ���
	CCAnimate m_bombItem;
	CCAnimate m_getItemAni;
	CCAnimate m_progressBack1Ani;
	CCAnimate m_progressBack2Ani;
	CCSprite * m_selectAni;						//ѡ�ж���
	CCAnimate m_bubbleAni;

	int m_comboCount;				//��¼��ǰ������
	int m_comboCountTotal;			//��¼һ����������������
	int m_comboAlpha;				//������ʾͼƬ��alphaֵ
	int m_comboCountAlpha;			

	//״̬��־
	bool m_changeBack;		//�ڽ������鶯���б�־�Ƿ�Ϊ���ض���
	bool m_isLButtonDown;	//��־�������Ƿ���
	bool m_isGameOver;		//�Ƿ�����Ϸ����״̬

	PaticleSystem *paticleSystemHead;
	Paticle comboPaticle[6];
};

#endif // !__CRAZYCAP
