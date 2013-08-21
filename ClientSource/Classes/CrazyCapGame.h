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

	Position m_selectedPos[2];		//记录两次点击选择的方块
	void LoadRes(int order);
	int m_blockBuffer[8][168];
	Block m_blocks[8][16];
	int m_progress;
	int m_gameState;				//0==等待开始,1==游戏进行中,2==游戏结束
	bool m_bufferNull;
	bool m_isPlayExplodeSound;
	Position tempPos;
	int m_gameMode;					//0 == 普通  ， 1==连续  
	int m_items[3];					//玩家拥有的道具，-1代表无道具

	static const int gameAreaX=0;		//游戏区域左上角坐标
	static const int gameAreaY=100;		//游戏区域左上角坐标
	static const int gameAreaWidth=480;	//游戏区域宽度
	static const int gameAreaHeight=510;//游戏区域高度
	static const int BLOCKWIDTH=60;

	int m_windowHeight;

	cocos2d::CCLayer * m_curLayer;
public:
	int AddAItem(int itemType);		//为道具栏增加一个道具，返回道具位置
	bool Help(Position &position1,Position &position2);				//帮助玩家找到一对可消的块,找到的话返回true,否则返回false
	int ExchangeBlock(Position &position1,Position &position2);
	Block *GetBlock(Position &position);
	bool EatAllLine();			//搜索屏幕上所有可消的块，并消除,有可消的块的话返回true,否则返回false
	bool DropDown();			//所有方快自由下落一格，如果有方块落下，返回true,否则返回false;	
	void MakeProgress(int comboCount);		//消行后用来计算Progress的变化
	void CleanRoom();
	void StartGame();							//游戏开始
	void OnTouchBegin(int x, int y);
	void OnTouchEnd(int x, int y);
	void OnTouchMove(int x, int y);				//移动
protected:
	bool IsHaveLine(Position position);		//判断某点是否在一个能消的宝石连线中
	void EatBlock(Position position);			//消掉方块
	void BombEatBlock(Position position);
	bool EatLine(Position position);			//消掉position涉及的几个连在一起的方块
	void ResortBlocks();		//重排屏幕上的方块
	void SystemExchangeBlock(int x1, int y1, int x2, int y2);	//重排时系统用来交换方块
	void PlaySound(int soundID);
	void TimerWork();
	void PlayAni(int x, int y, int dir, bool loop = false);
	bool CheckTwoPosLinked(Position position1, Position position2);
	void MoveBlockPair(Position position1, Position position2);		//移动方块
	Position GetPosition(int x,int y){Position p;p.x=x;p.y=y;return p;}//获得一个Position
	Position GoTo(Position pos,int direction,int step);		//direction 0 for up,1 for up-right, 2 for down right, 3 for down, 4 for left down, 5 for left up

	void CallBackND(CCNode * sender, void * data);
protected:
	BlockSprite * GetFreeBlockSprite(int type);
	void MakeSpriteFree(int type, BlockSprite * pBlockSprite);
	void OnDropDownStart();

	void CreatePaticleSystem(int x, int y,int startTime);		//direction 0 for left,1 for topleft,2 for top,and so on...
	void DeleteANodeFormPaticleSystem(PaticleSystem *p);				//

	void SetSelectAni(int x, int y);

	std::map<int, std::list<BlockSprite *> >   m_availableSprite;				//用来存放可用的Sprite
	std::map<int, std::string> m_soundEffectMap;

	unsigned int m_totalClickCount;		//总共点击次数
	unsigned int m_workedClickCount;	//有效点击次数
	unsigned int m_maxComboCount;		//最大combo数
	long m_lastClickTime;				//上次点击时间
	long m_rhythmMark;				//节奏感得分
	std::vector <long> m_perClickTakeTime;	//每次点击所用时间记录
	unsigned int m_rpMark;				//人品得分
	unsigned int m_totalComboCount;		//combo总次数
	int direction1,direction2;			//两个正在移动的块移动的方向

	//计时器
	Timer *timerMoveBlock;
	Timer *timerEatBlock;
	Timer *timerDropDown;
	Timer *timerCombo;
	Timer *timerComboCount;
	Timer *timerPaticles;
	Timer *timerGameEnd;
	Timer *m_timerGame;

	//精灵动画
	CCAnimation *m_blockMoveAni[6][6];			//六种方块向六个方向移动的动画
	CCAnimate m_bombItem;
	CCAnimate m_getItemAni;
	CCAnimate m_progressBack1Ani;
	CCAnimate m_progressBack2Ani;
	CCSprite * m_selectAni;						//选中动画
	CCAnimate m_bubbleAni;

	int m_comboCount;				//记录当前连击数
	int m_comboCountTotal;			//纪录一次连击的总连击数
	int m_comboAlpha;				//连击显示图片的alpha值
	int m_comboCountAlpha;			

	//状态标志
	bool m_changeBack;		//在交换方块动画中标志是否为换回动画
	bool m_isLButtonDown;	//标志鼠标左键是否按下
	bool m_isGameOver;		//是否处于游戏结束状态

	PaticleSystem *paticleSystemHead;
	Paticle comboPaticle[6];
};

#endif // !__CRAZYCAP
