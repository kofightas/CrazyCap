#include "CrazyCapGame.h"
#include "SimpleAudioEngine.h"

enum AudioType
{
	getready,
	go,
	win,
	lose,
	selected,
	eat,
	combo2,
	combo3,
	combo4,
	combo5,
	combo6,
	combo7,
	combo8,
	combo9,
	combo10,
	dropdown,
	capsmove,
	cantmove1,
	cantmove2,
	supercomboitem,
	nocomboitem,
	cantmoveitem,
	tips,
	counttime,
	itemattack,
	mousedown,
	useitem,
	explode,
};

int  CrazyCapGame::AddAItem(int itemType)
{
	for(int i=2;i>=0;i--)
	{
		if(m_items[i]==-1)
		{
			m_items[i]=itemType;
			return i;
		}
	}
	return -1;
}

void CrazyCapGame::StartGame()
{
	long time=millisecondNow();
	srand(time);
	for(int i=0;i<8;i++)
		for(int j=0;j<16;j++)
		{
			m_blocks[i][j].type=rand()%6;
			while(IsHaveLine(GetPosition(i,j)))m_blocks[i][j].type=(m_blocks[i][j].type + 1) % 6;		//若新生成瓶盖的造成消行，换一个颜色
			m_blocks[i][j].special=0;
			m_blocks[i][j].isDropping=false;
			m_blocks[i][j].x_move=0;
			m_blocks[i][j].y_move=0;
			m_blocks[i][j].alpha=255;
			m_blocks[i][j].isCanMove=true;
			
			if (j >= 8)
			{
				m_blocks[i][j].pBlockSprite = GetFreeBlockSprite(m_blocks[i][j].type);
			}
		}
}

void CrazyCapGame::CleanRoom()
{
	m_changeBack=false;					//置换回方块标记为false
	m_selectedPos[0].x=-1;				//用m_selectedPos[0].x=-1来表示当前第一个方块没有被选中
	m_selectedPos[1].x=-1;
	srand(1);
	for(int i=0;i<8;i++){
		for(int j=0;j<16;j++){
			m_blocks[i][j].type=-1;
			//while(IsHaveLine(GetPosition(i,j)))m_blocks[i][j].type=rand()%6;
			m_blocks[i][j].special=0;
			m_blocks[i][j].x_move=0;
			m_blocks[i][j].y_move=0;
			m_blocks[i][j].alpha=255;
			m_blocks[i][j].isDropping=false;
		}
	}
	m_isPlayExplodeSound=false;
	m_progress=0;
	m_gameState=0;
	m_rpMark = 0;
	m_bufferNull=false;
	for(int i=0;i<3;i++)
	{
		m_items[i]=-1;
	}
	for(int i=0;i<8;i++)
		for(int j=0;j<168;j++)
		{
			m_blockBuffer[i][j]=-1;
		}

	paticleSystemHead=NULL;
}

CrazyCapGame::CrazyCapGame()
{
	CleanRoom();
}


bool CrazyCapGame::IsHaveLine(Position position){
	int type=m_blocks[position.x][position.y].type;
	if(type==7)return false;
	if(GoTo(position,1,2).x<8 && GoTo(position,1,2).y>=8){
		if(m_blocks[GoTo(position,1,1).x][GoTo(position,1,1).y].type==type){
			if(m_blocks[GoTo(position,1,2).x][GoTo(position,1,2).y].type==type)return true;
		}
	}

	if(GoTo(position,2,2).x<8 && GoTo(position,2,2).y<16){
		if(m_blocks[GoTo(position,2,1).x][GoTo(position,2,1).y].type==type){
			if(m_blocks[GoTo(position,2,2).x][GoTo(position,2,2).y].type==type)return true;
		}
	}

	if(GoTo(position,4,2).x>=0 && GoTo(position,4,2).y>=8){
		if(m_blocks[GoTo(position,4,1).x][GoTo(position,4,1).y].type==type){
			if(m_blocks[GoTo(position,4,2).x][GoTo(position,4,2).y].type==type)return true;
		}
	}

	if(GoTo(position,5,2).x>=0 && GoTo(position,5,2).y>=8){
		if(m_blocks[GoTo(position,5,1).x][GoTo(position,5,1).y].type==type){
			if(m_blocks[GoTo(position,5,2).x][GoTo(position,5,2).y].type==type)return true;
		}
	}

	if(GoTo(position,5,1).x>=0 && GoTo(position,5,1).y>=8 && GoTo(position,2,1).x<8 && GoTo(position,2,1).y<16){
		if(m_blocks[GoTo(position,2,1).x][GoTo(position,2,1).y].type==type){
			if(m_blocks[GoTo(position,5,1).x][GoTo(position,5,1).y].type==type)return true;
		}
	}

	if(GoTo(position,4,1).x>=0 && GoTo(position,4,1).y<16 && GoTo(position,1,1).x<8 && GoTo(position,1,1).y>=8){
		if(m_blocks[GoTo(position,1,1).x][GoTo(position,1,1).y].type==type){
			if(m_blocks[GoTo(position,4,1).x][GoTo(position,4,1).y].type==type)return true;
		}
	}

	if(position.y-2>=8){
		if(m_blocks[position.x][position.y-2].type==type){
			if(m_blocks[position.x][position.y-1].type==type)return true;
		}
	}
	if(position.y+2<16){
		if(m_blocks[position.x][position.y+2].type==type){
			if(m_blocks[position.x][position.y+1].type==type)return true;
		}
	}
	if(position.y-1>=8&&position.y+1<16){
		if(m_blocks[position.x][position.y+1].type==type){
			if(m_blocks[position.x][position.y-1].type==type)return true;
		}
	}
	return false;
}


void CrazyCapGame::EatBlock(Position position)
{
	if(position.x>=8||position.y>=16 || position.x<0 || position.y<8)
		return;
	if(m_blocks[position.x][position.y].special==2)		//如果是石头
		return;
	if(m_blocks[position.x][position.y].x_move>0||m_blocks[position.x][position.y].y_move>0)
		return;
	for(int i=0;i<=position.y;i++)
	{
		m_blocks[position.x][i].isCanMove=false;
	}
	if(m_blocks[position.x][position.y].special==1)
	{
		Explode explode;
		m_blocks[position.x][position.y].special=0;
		explode.p.x=position.x;
		explode.p.y=position.y;
		explode.startTime=millisecondNow();
		//app.g_gameWindow->m_explodePosList.push_back(explode);
		m_isPlayExplodeSound=true;
		BombEatBlock(GoTo(position,0,1));
		BombEatBlock(GoTo(position,1,1));
		BombEatBlock(GoTo(position,2,1));
		BombEatBlock(GoTo(position,3,1));
		BombEatBlock(GoTo(position,4,1));
		BombEatBlock(GoTo(position,5,1));
	}
	m_blocks[position.x][position.y].alpha=254;			//置当前方块alpha值为254(用于客户端)
	m_blocks[position.x][position.y].special=0;

}

void CrazyCapGame::BombEatBlock(Position position)
{
	if(position.x>=8||position.y>=16 || position.x<0 || position.y<8)
		return;
	if(m_blocks[position.x][position.y].x_move>0||m_blocks[position.x][position.y].y_move>0)
		return;
	for(int i=0;i<=position.y;i++)
	{
		m_blocks[position.x][i].isCanMove=false;
	}
	if(m_blocks[position.x][position.y].special==1)
	{
		Explode explode;
		explode.p.x=position.x;
		explode.p.y=position.y;
		explode.startTime=millisecondNow();
		//app.g_gameWindow->m_explodePosList.push_back(explode);
		m_isPlayExplodeSound=true;
		m_blocks[position.x][position.y].special=0;
		BombEatBlock(GoTo(position,0,1));
		BombEatBlock(GoTo(position,1,1));
		BombEatBlock(GoTo(position,2,1));
		BombEatBlock(GoTo(position,3,1));
		BombEatBlock(GoTo(position,4,1));
		BombEatBlock(GoTo(position,5,1));
	}
	m_blocks[position.x][position.y].alpha=254;			//置当前方块alpha值为254(用于客户端)
	m_blocks[position.x][position.y].special=0;

}

void CrazyCapGame::MakeProgress(int comboCount){
	int sameTypeCount;

	for(int i=0;i<8;i++)
	{
		for(int j=8;(j+1)<16;j++)
		{
			sameTypeCount=0;
			while((j+1)<16&&m_blocks[i][j].type==m_blocks[i][j+1].type)
			{
				j++;
				sameTypeCount++;
			}
			if(sameTypeCount==2)
				m_progress+=10;
			if(sameTypeCount==3)
				m_progress+=15;
			if(sameTypeCount==4)
			{
				m_progress+=20;
			}
		}
	}



	Position pos=GetPosition(1,8);
	for(int k=0;k+1<8;k++)
	{
		sameTypeCount=0;
		while (GoTo(pos,2,k+1).x<8 && GoTo(pos,2,k+1).y<16 && m_blocks[GoTo(pos,2,k+1).x][GoTo(pos,2,k+1).y].type==m_blocks[GoTo(pos,2,k).x][GoTo(pos,2,k).y].type )
		{
			k++;
			sameTypeCount++;
		}
		if(sameTypeCount==2)
			m_progress+=10;
		if(sameTypeCount==3)
			m_progress+=15;
		if(sameTypeCount==4)
		{
			m_progress+=20;
		}
	}


	pos=GetPosition(3,8);
	for(int k=0;k+1<8;k++)
	{
		sameTypeCount=0;
		while (GoTo(pos,2,k+1).x<8 && GoTo(pos,2,k+1).y<16 && m_blocks[GoTo(pos,2,k+1).x][GoTo(pos,2,k+1).y].type==m_blocks[GoTo(pos,2,k).x][GoTo(pos,2,k).y].type )
		{
			k++;
			sameTypeCount++;
		}
		if(sameTypeCount==2)
			m_progress+=10;
		if(sameTypeCount==3)
			m_progress+=15;
		if(sameTypeCount==4)
		{
			m_progress+=20;
		}
	}


	pos=GetPosition(5,8);
	for(int k=0;k+1<8;k++)
	{
		sameTypeCount=0;
		while (GoTo(pos,2,k+1).x<8 && GoTo(pos,2,k+1).y<16 && m_blocks[GoTo(pos,2,k+1).x][GoTo(pos,2,k+1).y].type==m_blocks[GoTo(pos,2,k).x][GoTo(pos,2,k).y].type )
		{
			k++;
			sameTypeCount++;
		}
		if(sameTypeCount==2)
			m_progress+=10;
		if(sameTypeCount==3)
			m_progress+=15;
		if(sameTypeCount==4)
		{
			m_progress+=20;
		}
	}

	for(int j=8;j<16;j++)
	{

		pos=GetPosition(0,j);
		for(int k=0;k<=7;k++)
		{
			sameTypeCount=0;
			while (GoTo(pos,2,k+1).x<8 && GoTo(pos,2,k+1).y<16 && m_blocks[GoTo(pos,2,k+1).x][GoTo(pos,2,k+1).y].type==m_blocks[GoTo(pos,2,k).x][GoTo(pos,2,k).y].type )
			{
				k++;
				sameTypeCount++;
			}
			if(sameTypeCount==2)
				m_progress+=10;
			if(sameTypeCount==3)
				m_progress+=15;
			if(sameTypeCount==4)
			{
				m_progress+=20;
			}
		}
	}





	pos=GetPosition(3,8);
	for(int k=0;k+1<8;k++)
	{
		sameTypeCount=0;
		while (GoTo(pos,4,k+1).x>=0 && GoTo(pos,4,k+1).y<16 && m_blocks[GoTo(pos,4,k+1).x][GoTo(pos,4,k+1).y].type==m_blocks[GoTo(pos,4,k).x][GoTo(pos,4,k).y].type )
		{
			k++;
			sameTypeCount++;
		}
		if(sameTypeCount==2)
			m_progress+=10;
		if(sameTypeCount==3)
			m_progress+=15;
		if(sameTypeCount==4)
		{
			m_progress+=20;
		}
	}


	pos=GetPosition(5,8);
	for(int k=0;k+1<8;k++)
	{
		sameTypeCount=0;
		while (GoTo(pos,4,k+1).x>=0 && GoTo(pos,4,k+1).y<16 && m_blocks[GoTo(pos,4,k+1).x][GoTo(pos,4,k+1).y].type==m_blocks[GoTo(pos,4,k).x][GoTo(pos,4,k).y].type )
		{
			k++;
			sameTypeCount++;
		}
		if(sameTypeCount==2)
			m_progress+=10;
		if(sameTypeCount==3)
			m_progress+=15;
		if(sameTypeCount==4)
		{
			m_progress+=20;
		}
	}

	for(int j=8;j<16;j++)
	{

		pos=GetPosition(7,j);
		for(int k=0;k<=7;k++)
		{
			sameTypeCount=0;
			while (GoTo(pos,4,k+1).x>=0 && GoTo(pos,4,k+1).y<16 && m_blocks[GoTo(pos,4,k+1).x][GoTo(pos,4,k+1).y].type==m_blocks[GoTo(pos,4,k).x][GoTo(pos,4,k).y].type )
			{
				k++;
				sameTypeCount++;
			}
			if(sameTypeCount==2)
				m_progress+=10;
			if(sameTypeCount==3)
				m_progress+=15;
			if(sameTypeCount==4)
			{
				m_progress+=20;
			}
		}
	}

	if(comboCount>1)
		m_progress+=comboCount*5;
	m_rpMark+=comboCount*5;
}

bool CrazyCapGame::EatLine(Position position)
{
	int type=m_blocks[position.x][position.y].type;
	bool tag=false;
	if(type==-1)return false;
	if(type>6)return false;

	//向左上方向判断是否在连线中
	if(GoTo(position,5,2).y>=8&&GoTo(position,5,2).x>=0)			//如果当前方块左上方两格内都有方块
	{
		if(m_blocks[GoTo(position,5,2).x][GoTo(position,5,2).y].type==type&&m_blocks[GoTo(position,5,1).x][GoTo(position,5,1).y].type==type)		//如果左上方两块与当前块同类
		{			
			EatBlock(GoTo(position,5,2));			//消掉左上方第二个方块
			EatBlock(GoTo(position,5,1));			//消掉左上方第一个方块
			if(GoTo(position,2,1).x<8&&GoTo(position,2,1).y<16)
				if(m_blocks[GoTo(position,2,1).x][GoTo(position,2,1).y].type==type)		//如果右下方第一个方块与当前方块类型也相同
				{		
					EatBlock(GetPosition(GoTo(position,2,1).x,GoTo(position,2,1).y));		//消掉右下方第一个方块
					if(GoTo(position,2,2).x<8&&GoTo(position,2,1).y<16)
						if(m_blocks[GoTo(position,2,2).x][GoTo(position,2,2).y].type==type)		//如果右下方第二个方块也与当前方块类型相同	
						{	
							EatBlock(GoTo(position,2,2));	//消掉右下方第二个方块
						}
				}
				tag=true;
		}
	}

	//向右下方向判断是否在连线中

	if(GoTo(position,2,2).x<8&&GoTo(position,2,2).y<16)		//如果当前方块右下方两格内都有方块
	{												
		if(m_blocks[GoTo(position,2,2).x][GoTo(position,2,2).y].type==type&&m_blocks[GoTo(position,2,1).x][GoTo(position,2,1).y].type==type)
		{			//如果右边两块与当前块同类
			EatBlock(GoTo(position,2,2));			//消掉右下方第二个方块
			EatBlock(GoTo(position,2,1));			//消掉右下方第一个方块
			if(GoTo(position,5,1).x>=0 && GoTo(position,5,1).y>=8)
				if(m_blocks[GoTo(position,5,1).x][GoTo(position,5,1).y].type==type)	//如果左边第一个方块与当前方块类型也相同
				{		
					EatBlock(GoTo(position,5,1));		//消掉左边第一个方块
				}
				tag=true;
		}
	}




	//向右上方向判断是否在连线中
	if(GoTo(position,1,2).x<8&&GoTo(position,1,2).y>=8)			//如果当前方块左上方两格内都有方块
	{											
		if(m_blocks[GoTo(position,1,2).x][GoTo(position,1,2).y].type==type&&m_blocks[GoTo(position,1,1).x][GoTo(position,1,1).y].type==type)		//如果左上方两块与当前块同类
		{			
			EatBlock(GoTo(position,1,2));			//消掉左上方第二个方块
			EatBlock(GoTo(position,1,1));			//消掉左上方第一个方块
			if(GoTo(position,4,1).x>=0&&GoTo(position,4,1).y<16)
				if(m_blocks[GoTo(position,4,1).x][GoTo(position,4,1).y].type==type)		//如果右下方第一个方块与当前方块类型也相同
				{		
					EatBlock(GetPosition(GoTo(position,4,1).x,GoTo(position,4,1).y));		//消掉右下方第一个方块
					if(GoTo(position,4,2).x>=0&&GoTo(position,4,1).y<16)
						if(m_blocks[GoTo(position,4,2).x][GoTo(position,4,2).y].type==type)		//如果右下方第二个方块也与当前方块类型相同	
						{	
							EatBlock(GoTo(position,4,2));	//消掉右下方第二个方块
						}
				}
				tag=true;
		}
	}

	//向左下方向判断是否在连线中

	if(GoTo(position,4,2).x>=0&&GoTo(position,4,2).y<16)		//如果当前方块右下方两格内都有方块
	{												
		if(m_blocks[GoTo(position,4,2).x][GoTo(position,4,2).y].type==type&&m_blocks[GoTo(position,4,1).x][GoTo(position,4,1).y].type==type)
		{			//如果右边两块与当前块同类
			EatBlock(GoTo(position,4,2));			//消掉右下方第二个方块
			EatBlock(GoTo(position,4,1));			//消掉右下方第一个方块
			if(GoTo(position,1,1).x<8 && GoTo(position,1,1).y>=8)
				if(m_blocks[GoTo(position,1,1).x][GoTo(position,1,1).y].type==type)	//如果左边第一个方块与当前方块类型也相同
				{		
					EatBlock(GoTo(position,1,1));		//消掉左边第一个方块
				}
				tag=true;
		}
	}



	//向上方向判断是否在连线中
	if(position.y-2>=8){
		if(m_blocks[position.x][position.y-2].type==type&&m_blocks[position.x][position.y-1].type==type){			//如果上边两块与当前块同类
			EatBlock(GetPosition(position.x,position.y-2));			//消掉上边第二个方块
			EatBlock(GetPosition(position.x,position.y-1));			//消掉上边第一个方块
			if(position.y+1<16)
				if(m_blocks[position.x][position.y+1].type==type){		//如果下边第一个方块与当前方块类型也相同
					EatBlock(GetPosition(position.x,position.y+1));		//消掉下边第一个方块
					if(position.y+2<16)
						if(m_blocks[position.x][position.y+2].type==type){	//如果下边第二个方块也与当前方块类型相同	
							EatBlock(GetPosition(position.x,position.y+2));	//消掉下边第二个方块
						}
				}
				tag=true;
		}
	}
	//向下方向判断是否在连线中
	if(position.y+2<16){
		if(m_blocks[position.x][position.y+2].type==type&&m_blocks[position.x][position.y+1].type==type){			//如果下边两块与当前块同类
			EatBlock(GetPosition(position.x,position.y+2));			//消掉上边第二个方块
			EatBlock(GetPosition(position.x,position.y+1));			//消掉上边第一个方块
			if(position.y-1>=8)
				if(m_blocks[position.x][position.y-1].type==type){		//如果下边第一个方块与当前方块类型也相同
					EatBlock(GetPosition(position.x,position.y-1));		//消掉下边第一个方块
				}
				tag=true;
		}
	}

	if(GoTo(position,4,1).x>=0 && GoTo(position,4,1).y<16 && GoTo(position,1,1).x<8 && GoTo(position,1,1).y>=8)
	{
		if(m_blocks[GoTo(position,4,1).x][GoTo(position,4,1).y].type==type&&m_blocks[GoTo(position,1,1).x][GoTo(position,1,1).y].type==type)
		{			//如果左下方和右上方两块与当前块同类
			EatBlock(GoTo(position,1,1));			//消掉右上方第一个方块
			EatBlock(GoTo(position,4,1));			//消掉左下方第一个方块
			tag=true;
		}
	}

	if(GoTo(position,5,1).x>=0 && GoTo(position,5,1).y>=8 && GoTo(position,2,1).x<8 && GoTo(position,2,1).y<16)
	{
		if(m_blocks[GoTo(position,5,1).x][GoTo(position,5,1).y].type==type&&m_blocks[GoTo(position,2,1).x][GoTo(position,2,1).y].type==type)
		{			//如果左下方和右上方两块与当前块同类
			EatBlock(GoTo(position,5,1));			//消掉右上方第一个方块
			EatBlock(GoTo(position,2,1));			//消掉左下方第一个方块
			tag=true;
		}
	}

	if(position.y-1>=8&&position.y+1<16)
	{
		if(m_blocks[position.x][position.y-1].type==type&&m_blocks[position.x][position.y+1].type==type)
		{			//如果左右两边两块与当前块同类
			EatBlock(GetPosition(position.x,position.y-1));			//消掉上边第一个方块
			EatBlock(GetPosition(position.x,position.y+1));			//消掉下边第一个方块
			tag=true;
		}
	}

	if(tag){

		EatBlock(GetPosition(position.x,position.y));			//消除当前块
		return true;
	}
	return false;
}



int CrazyCapGame::ExchangeBlock(Position &position1,Position &position2){
	bool tag1,tag2;
	Block tempBlock=m_blocks[position1.x][position1.y];
	m_blocks[position1.x][position1.y]=m_blocks[position2.x][position2.y];
	m_blocks[position2.x][position2.y]=tempBlock;
	tag2=false;
	tag1=EatLine(position1);
	tag2=EatLine(position2);
	if(tag1||tag2){
		return 1;
	}
	return 0;
}



bool CrazyCapGame::DropDown(){
	bool tag=false;
	for(int i=0;i<8;i++){
		for(int j=15;j>=0;j--){
			if(m_blocks[i][j].type==-1){			//若发现空格子
				tag=true;
				for(int k=j;k>0;k--){				//空格子上方全体掉落一行
					m_blocks[i][k]=m_blocks[i][k-1];
					if (k == 8)
					{
						m_blocks[i][k].pBlockSprite = GetFreeBlockSprite(m_blocks[i][k].type);
					}
				}
				m_blocks[i][0].type=rand()%6;		//最上方获取新的方块
				if(m_blocks[i][0].type==-1)						//如果最上方已经没有新的方块
				{
					//m_bufferNull=true;
					tag=false;
					break;
				}
				m_blocks[i][0].x_move=0;
				m_blocks[i][0].y_move=0;
				m_blocks[i][0].alpha=255;
				m_blocks[i][0].isDropping=false;
				break;
			}
		}
	}
	return tag;			//返回是否发生了掉落
}



bool CrazyCapGame::EatAllLine(){
	bool tag=false;
	for(int i=0;i<8;i++)
	{
		for(int j=0;j<8;j++)
		{
			if(EatLine(GetPosition(i,j+8)))
			{
				tag=true;
			}
		}
	}
	return tag;
}



bool CrazyCapGame::Help(Position &position1,Position &position2){
	Block tempBlock;
	for(int i=0;i<8;i++)
		for(int j=8;j<15;j++){
			tempBlock=m_blocks[i][j];
			m_blocks[i][j]=m_blocks[i][j+1];
			m_blocks[i][j+1]=tempBlock;
			if(IsHaveLine(GetPosition(i,j))||IsHaveLine(GetPosition(i,j+1))){
				position1.x=i;
				position1.y=j;
				position2.x=i;
				position2.y=j+1;
				tempBlock=m_blocks[i][j];
				m_blocks[i][j]=m_blocks[i][j+1];
				m_blocks[i][j+1]=tempBlock;
				return true;
			}
			tempBlock=m_blocks[i][j];
			m_blocks[i][j]=m_blocks[i][j+1];
			m_blocks[i][j+1]=tempBlock;
		}
		for(int i=0;i<7;i++)
			for(int j=8;j<16;j++){
				tempBlock=m_blocks[i][j];
				m_blocks[i][j]=m_blocks[i+1][j];
				m_blocks[i+1][j]=tempBlock;
				if(IsHaveLine(GetPosition(i,j))||IsHaveLine(GetPosition(i+1,j))){
					position1.x=i;
					position1.y=j;
					position2.x=i+1;
					position2.y=j;
					tempBlock=m_blocks[i][j];
					m_blocks[i][j]=m_blocks[i+1][j];
					m_blocks[i+1][j]=tempBlock;
					return true;
				}
				tempBlock=m_blocks[i][j];
				m_blocks[i][j]=m_blocks[i+1][j];
				m_blocks[i+1][j]=tempBlock;
			}
			return false;
}

void CrazyCapGame::ResortBlocks()
{
	for(int i=0;i<64;i++)
	{
		Position pos1 = GetPosition(i%6,i%7+8);
		Position pos2 = GetPosition(i%7,i%6+8);
		SystemExchangeBlock(pos1.x, pos1.y, pos2.x, pos2.y);
	}
}

Position CrazyCapGame::GoTo(Position pos,int direction,int step)
{
	if(step==0)return pos;
	int i=pos.x;
	int j=pos.y;
	Position p;
	if((i+10)%2==0)
	{
		switch(direction) {
		case 0:
			{
				j--;
			}
			break;
		case 1:
			{
				i++;
			}
			break;
		case 2:
			{
				i++;
				j++;
			}
			break;
		case 3:
			{
				j++;
			}
			break;
		case 4:
			{
				i--;
				j++;
			}
			break;
		case 5:
			{
				i--;
			}
			break;
		default:
			break;
		}
	}
	else
	{
		switch(direction) {
		case 0:
			{
				j--;
			}
			break;
		case 1:
			{
				i++;
				j--;
			}
			break;
		case 2:
			{
				i++;
			}
			break;
		case 3:
			{
				j++;
			}
			break;
		case 4:
			{
				i--;
			}
			break;
		case 5:
			{
				j--;
				i--;
			}
			break;
		default:
			break;
		}
	}
	p.x=i;
	p.y=j;

	if(step-1>0)
	{
		return GoTo(p,direction,step-1);
	}
	else
	{	
		return p;
	}
}

void CrazyCapGame::SystemExchangeBlock(int x1, int y1, int x2, int y2)
{
	Block temp=m_blocks[x1][y1];
	m_blocks[x1][y1]=m_blocks[x2][y2];
	m_blocks[x2][y2]=temp;
}

void CrazyCapGame::OnDropDownStart()
{
	for (unsigned int i=0; i<8;++i)
	{
		if (m_blocks[i][7].pBlockSprite == NULL)
		{
			//m_blocks[i][7].pSprite = GetFreeBlockSprite(m_blocks[i][7].type);
		}
	}
}

void CrazyCapGame::TimerWork()
{
	/*-------------------------处理timerComboCount-----------------------*/
	if(timerComboCount->GetState()==Timer::ERunning){
		if(timerComboCount->GetTime()>5000){
			timerComboCount->Stop();
		}
		else{
			m_comboCountAlpha=255-timerComboCount->GetTime()*255/5000;
		}
	}

	/*-------------------------处理timerCombo-----------------------*/

	if(timerCombo->GetState()==Timer::ERunning){
		if(timerCombo->GetTime()>900){
			timerCombo->Stop();
		}
		else
		{
			int timeNow=timerCombo->GetTime();
			for(int i=0;i<5;i++)
			{
				comboPaticle[i].x=comboPaticle[i].startX+timeNow*comboPaticle[i].xSpeed/100;
				comboPaticle[i].y=comboPaticle[i].startY+timeNow*comboPaticle[i].ySpeed/100+6*timeNow*timeNow/20000;;
			}
			m_comboAlpha=255-timerCombo->GetTime()*255/900;
		}
	}

	/*------------------处理timerDropDown------------------*/
	if(timerDropDown->GetState()==Timer::ERunning){
		if(timerDropDown->GetTime()>120)	//如果下落计时器到120毫秒
		{
			timerDropDown->Stop();
			DropDown();


			//清空方块的下落标志和偏移量
			for(int i=0;i<8;i++)
				for(int j=0;j<16;j++){
					m_blocks[i][j].isDropping=false;
					m_blocks[i][j].x_move=0;
					m_blocks[i][j].y_move=0;
				}
				bool isNeedDrop=false;
				for(int i=0;i<8;i++)
					for(int j=7;j>=0;j--){
						if(m_blocks[i][j+8].type==-1){
							isNeedDrop=true;
							for(int k=j;k>=-1;k--){
								m_blocks[i][k+8].isDropping=true;
							}
							break;
						}
					}		
					if(isNeedDrop){							//如果仍有空格，继续下落
						timerDropDown->Play();
						OnDropDownStart();
					}
					else{									//否则，更新m_game里的数据，消所有能消行
						PlaySound(dropdown);
						for(int i=0;i<8;i++)
							for(int j=0;j<16;j++)
							{
								m_blocks[i][j].isCanMove=true;
							}
							if(!EatAllLine())
							{

								m_comboCountTotal=m_comboCount;
								//if(!m_isSinglePlay)
								//{
								//	if(m_comboCountTotal>=7)
								//		m_getItemPos=m_game->AddAItem(2);
								//	else if(m_comboCountTotal>=5)
								//		m_getItemPos=m_game->AddAItem(1);
								//	else if(m_comboCountTotal>=3)
								//		m_getItemPos=m_game->AddAItem(0);
								//	if(m_comboCountTotal>=3)
								//		m_getItemAni.Play(false);
								//}
								m_comboCount=1;
								if(timerComboCount->GetState()==Timer::ERunning)
								{
									timerComboCount->Stop();
								}
								timerComboCount->Play();
							}
							else {

								timerEatBlock->Play();
								timerEatBlock->Adjust(50);		//Bug,不知道为什么这里必须停一下，才能消所有行
								//if(m_isNoCombo)
								//{

								//}
								//else if(m_isSuperCombo)
								//{
								//	m_comboCount+=2;
								//}
								//else
								//{
									m_comboCount++;
									m_totalComboCount++;
								//}
								if( m_comboCount > m_maxComboCount )
								{
									m_maxComboCount = m_comboCount;
								}
								if(m_comboCount>=2)
								{
									//m_bubbleAni.Play(false);
								}
								if(m_comboCount>2 && m_comboCount <=10)
								{
									PlaySound(combo2 + m_comboCount - 2);
								}
								if(m_comboCount>=10)
								{
									PlaySound(combo10);
								}
								//if(!m_gameEnd)
								//	m_game->MakeProgress(m_comboCount);			//计算分数变化

								//if(m_game->m_progress>m_progressToWin[0])
								//{
								//	m_game->m_progress=m_progressToWin[0];
								//	if(m_isSinglePlay)
								//	{
								//		SinglePlayGameOver();
								//	}
								//}

								//m_userInfo[0].progress=m_game->m_progress;

								if(timerCombo->GetState()==Timer::ERunning)
								{
									timerCombo->Stop();
								}
								timerCombo->Play();
								for(int i=0;i<6;i++)
								{
									comboPaticle[i].xSpeed=(i-4)*20+rand()%40;
									comboPaticle[i].ySpeed=0-rand()%50;
									comboPaticle[i].frame=0;
								}
								comboPaticle[0].startX=194;
								comboPaticle[0].startY=174;
								comboPaticle[1].startX=265;
								comboPaticle[1].startY=167;
								comboPaticle[2].startX=336;
								comboPaticle[2].startY=165;
								comboPaticle[3].startX=424;
								comboPaticle[3].startY=170;
								comboPaticle[4].startX=492;
								comboPaticle[4].startY=176;
								comboPaticle[5].x=34;
								comboPaticle[5].y=15;
							}
					}
		}
		else
		{											//如果未到120毫秒，更新各方快的位置
			for(int i=0;i<8;i++)
				for(int j=-1;j<8;j++)
				{
					if(m_blocks[i][j+8].isDropping)
					{
						m_blocks[i][j+8].y_move=(int)timerDropDown->GetTime()*BLOCKWIDTH/120;
					}
				}
		}
	}

	/*------------------处理timerEatBlock------------------*/
	if(timerEatBlock->GetState()==Timer::ERunning){			//如果消块计时器状态为开启
		if(timerEatBlock->GetTime()>500){		//消块计时器到400毫秒
			timerEatBlock->Stop();
			for(int i=0;i<8;i++)
				for(int j=0;j<8;j++){
					if(m_blocks[i][j+8].alpha<255)
					{
						int startTime;
						if(timerPaticles->GetState()==Timer::ERunning)
							startTime=timerPaticles->GetTime();
						else
						{
							startTime=0;
							timerPaticles->Play();
						}
						PlayAni(i, j+8, 0, true);
						CreatePaticleSystem(i, j+8, startTime);
						//清空block信息
						m_blocks[i][j+8].type = -1;
						m_blocks[i][j+8].pBlockSprite = NULL;
						m_blocks[i][j+8].alpha = 255;

					}
				}

				for(int i=0;i<8;i++)
					for(int j=7;j>=0;j--){
						if(m_blocks[i][j+8].type==-1){
							for(int k=j-1;k>=-1;k--){
								m_blocks[i][k+8].isDropping=true;
							}
							break;
						}
					}
					timerDropDown->Play();								//开启自由下落计时器
					OnDropDownStart();
					return;
		}

		for(int i=0;i<8;i++)
			for(int j=0;j<8;j++){
				if(m_blocks[i][j+8].alpha<255)
				{
					m_blocks[i][j+8].alpha=255-timerEatBlock->GetTime()*255/500;		//根据当前时间计算要消的块的alpha值
				}
			}
	}

	/*------------------处理timerMoveBlock------------------*/
	if(timerMoveBlock->GetState()==Timer::ERunning){			//如果交换方块计时器状态为开启
		bool tag=false;
		int passTime = timerMoveBlock->GetTime();
		if(passTime>500)	//交换方块计时器到了500毫秒
		{		
			timerMoveBlock->Stop();				//停止计时器
			//清空方块的偏移值
			m_blocks[m_selectedPos[0].x][m_selectedPos[0].y].x_move=0;
			m_blocks[m_selectedPos[0].x][m_selectedPos[0].y].y_move=0;
			m_blocks[m_selectedPos[1].x][m_selectedPos[1].y].x_move=0;
			m_blocks[m_selectedPos[1].x][m_selectedPos[1].y].y_move=0;
			CCLOGERROR("Clear y_move\n");
			if(m_changeBack)//如果处于换回状态
			{		
				m_changeBack=false;	//清除换回标志
				if(m_selectedPos[0].x==-1||m_selectedPos[1].x==-1)return;

				if(ExchangeBlock(m_selectedPos[0],m_selectedPos[1]))
				{
					PlaySound(eat);
					//if(!m_gameEnd)
					//	MakeProgress(m_comboCount);			//计算分数变化

					//处理游戏结束
					//if(m_progress>m_progressToWin[0])
					//{
					//	m_progress=m_progressToWin[0];
					//	if(m_isSinglePlay)
					//	{
					//		SinglePlayGameOver();
					//	}
					//}
					//m_userInfo[0].progress=m_game->m_progress;
					if(m_selectedPos[0].x==-1||m_selectedPos[1].x==-1)return;			//若什么都没选这里返回

					m_selectedPos[0].x=-1;												//清除选块数据
					m_selectedPos[1].x=-1;
					timerEatBlock->Play();												//开启消块计时器
					tag=true;
				}

				//清空选择的方块
				m_selectedPos[0].x=-1;
				m_selectedPos[1].x=-1;
			}
			else 
			{
				if(ExchangeBlock(m_selectedPos[0],m_selectedPos[1])==0)//如果交换不成功,播放交换回来的动画
				{		
					if(m_gameMode==0)
					{
						timerMoveBlock->Play();
						if(m_blocks[m_selectedPos[1].x][m_selectedPos[1].y].type>=0 && m_blocks[m_selectedPos[1].x][m_selectedPos[1].y].type<6)
						{
							PlayAni(m_selectedPos[1].x, m_selectedPos[1].y,direction2);
						}
						if(m_blocks[m_selectedPos[0].x][m_selectedPos[0].y].type>=0 && m_blocks[m_selectedPos[1].x][m_selectedPos[1].y].type<6)
						{
							PlayAni(m_selectedPos[0].x, m_selectedPos[0].y,direction1);
						}
						m_changeBack=true;
					}
				}
				else
				{					//如果交换成功
					m_workedClickCount++;
					PlaySound(eat);
					//if(!m_gameEnd)
					//	m_game->MakeProgress(m_comboCount);			//计算分数变化
					//if(m_game->m_progress>m_progressToWin[0])
					//{
					//	m_game->m_progress=m_progressToWin[0];
					//	if(m_isSinglePlay)
					//	{
					//		SinglePlayGameOver();
					//	}
					//}
					//m_userInfo[0].progress=m_game->m_progress;
					if(m_selectedPos[0].x==-1||m_selectedPos[1].x==-1)return;

					m_selectedPos[0].x=-1;
					m_selectedPos[1].x=-1;
					timerEatBlock->Play();												//开启消块计时器
					tag=true;
				}
			}
		}
		passTime = timerMoveBlock->GetTime();		//重新获取一次passtime，因为前面有可能被刷新过
		if(m_selectedPos[0].x==-1||m_selectedPos[1].x==-1)return;
		if(m_selectedPos[0].x!=m_selectedPos[1].x&&!tag)		//若x方向上的值不一样，就有x方向上的移动
		{
			m_blocks[m_selectedPos[0].x][m_selectedPos[0].y].x_move=(m_selectedPos[1].x-m_selectedPos[0].x)*passTime*BLOCKWIDTH/500;
			m_blocks[m_selectedPos[1].x][m_selectedPos[1].y].x_move=(m_selectedPos[0].x-m_selectedPos[1].x)*passTime*BLOCKWIDTH/500;
		}
		if(m_selectedPos[0].x-m_selectedPos[1].x==0)
		{
			m_blocks[m_selectedPos[0].x][m_selectedPos[0].y].y_move=(m_selectedPos[1].y-m_selectedPos[0].y)*passTime*BLOCKWIDTH/500;
			m_blocks[m_selectedPos[1].x][m_selectedPos[1].y].y_move=(m_selectedPos[0].y-m_selectedPos[1].y)*passTime*BLOCKWIDTH/500;
			CCLOGERROR("y_move1 = %d, y_move2 = %d\n", m_blocks[m_selectedPos[0].x][m_selectedPos[0].y].y_move, m_blocks[m_selectedPos[1].x][m_selectedPos[1].y].y_move);
		}
		else
		{
			if(m_selectedPos[0].y!=m_selectedPos[1].y)
			{
				m_blocks[m_selectedPos[0].x][m_selectedPos[0].y].y_move=(m_selectedPos[1].y-m_selectedPos[0].y)*passTime*(BLOCKWIDTH/2)/500;
				m_blocks[m_selectedPos[1].x][m_selectedPos[1].y].y_move=(m_selectedPos[0].y-m_selectedPos[1].y)*passTime*(BLOCKWIDTH/2)/500;
			}
			else
			{
				if(m_selectedPos[0].x%2==0)
				{
					m_blocks[m_selectedPos[0].x][m_selectedPos[0].y].y_move=0-passTime*(BLOCKWIDTH/2)/500;
					m_blocks[m_selectedPos[1].x][m_selectedPos[1].y].y_move=passTime*(BLOCKWIDTH/2)/500;
				}
				else
				{
					m_blocks[m_selectedPos[0].x][m_selectedPos[0].y].y_move=passTime*(BLOCKWIDTH/2)/500;
					m_blocks[m_selectedPos[1].x][m_selectedPos[1].y].y_move=0-passTime*(BLOCKWIDTH/2)/500;
				}
			}
		}

	}

	//-------------------------处理timerPaticles--------------------------//
	if(timerPaticles->GetState()==Timer::ERunning){
		if(paticleSystemHead==NULL)		//若粒子已经结束
		{
			timerPaticles->Stop();
			//if (this->m_gameEnd == true
			//	&& m_game->m_gameState == 1
			//	&& m_timerEnd == true)
			//{
			//	m_isGameOver=true;
			//	for(int i=0;i<4;i++)
			//	{
			//		if(m_userInfo[i].userID>-1)
			//			for(int j=0;j<4;j++)
			//			{
			//				if(m_userInfo[i].userID == m_gameOverMsg.UserID[j])
			//				{
			//					m_userInfo[i].ScoreThisTime = m_gameOverMsg.UserScore[j];
			//					m_userInfo[i].progress = m_gameOverMsg.UserProgress[j];
			//					//一局完成后的成绩记录
			//					m_lastScore.ID[i] = m_gameOverMsg.UserID[j];
			//					m_lastScore.Score[i] = m_gameOverMsg.UserScore[j];
			//					m_lastScore.nickname[i] = m_userInfo[i].nickName;
			//					m_lastScore.ProgressToWin[i] = m_progressToWin[i];
			//					m_lastScore.WinMoney[i]=m_gameOverMsg.WinMoney[j];

			//					if(m_userInfo[i].progress >= m_progressToWin[i])
			//						m_userInfo[i].progress = m_progressToWin[i];

			//					m_lastScore.Progress[i] = m_userInfo[i].progress;

			//					m_userInfo[i].isWinner = m_gameOverMsg.isWinner[j];

			//				}
			//			}
			//	}
			//if(m_isSinglePlay)
			//{
			//	((PictureBox*)GetChild("SingleScoreWindow"))->SetValue("Visible","1");
			//	//计算节奏感得分
			//	int totalTime = 0;
			//	for(int i=0; i<m_perClickTakeTime.size(); i++)
			//	{
			//		totalTime += m_perClickTakeTime[i];
			//	}
			//	int averageTime = totalTime/m_perClickTakeTime.size();
			//	int totalMark = 0;
			//	for(int i=0; i<m_perClickTakeTime.size(); i++)
			//	{
			//		totalMark += (((int)m_perClickTakeTime[i] - averageTime)) * (((int)m_perClickTakeTime[i] - averageTime));
			//	}
			//	m_rhythmMark = totalMark / m_perClickTakeTime.size()/1000;

			//}
			//else
			//{
			//	((PictureBox*)GetChild("ShowScorePic"))->SetValue("Visible","1");
			//}
			//for(int i=0;i<4;i++)
			//{
			//	if(m_userInfo[i].userID>0)
			//		app.g_hallWindow->UpdateUserInfo(m_userInfo[i].userID,m_userInfo[i]);
			//}
			//app.g_hallWindow->UpdateUserInfo(g_userInformation.userID,g_userInformation);
		}// end if
		else
		{
			PaticleSystem * pt=paticleSystemHead;
			PaticleSystem * oldpt;
			while(pt!=NULL)
			{
				int minY=600;
				float timeNow;
				if(pt->startTime>0)
					timeNow=(float)(timerPaticles->GetTime()-pt->startTime);
				else
					timeNow=(float)(timerPaticles->GetTime());
				pt->p.x=(int)(timeNow/100*pt->p.xSpeed+pt->p.startX);
				pt->p.y=(int)(timeNow/100*pt->p.ySpeed+pt->p.startY+15*timeNow*timeNow/15000);

				int shadowX = (int)(timeNow/100 * (pt->p.xSpeed / 2) + pt->p.startX);
				int shadowY = (int)(timeNow/100 * (pt->p.ySpeed - 1) + pt->p.startY + 15*timeNow*timeNow / 15000);
				if(pt->p.y<minY)minY=pt->p.y;

				pt->p.pBlockSprite->pSprite->setPosition(CCPointMake(pt->p.x, m_windowHeight - pt->p.y));
				pt->p.pBlockSprite->pShadowSprite->setPosition(CCPointMake(shadowX, m_windowHeight - shadowY));

				oldpt=pt;
				pt=pt->next;
				if(minY==600)
				{
					DeleteANodeFormPaticleSystem(oldpt);
				}
			}
		}
	}
}

void CrazyCapGame::Update()
{
	TimerWork();

	int shadowOffSet = 5;

	//根据数据绘制Sprite
	for(int i=0;i<8;i++)
	{
		for(int j=0;j<8;j++)
		{
			//当前不是正在移动的瓶盖
			//if(! ( ((m_selectedPos[0].x==i&&m_selectedPos[0].y==j+8) ||  (m_selectedPos[1].x==i&&m_selectedPos[1].y==j+8))&&timerMoveBlock->GetState()==Timer::ERunning ) )
			{
				int eatBlockFrame=0;

				if(m_blocks[i][j+8].alpha<255  )		//如果正在消失
				{
					if(m_blocks[i][j+8].alpha>100)
						eatBlockFrame=(m_blocks[i][j+8].alpha-98)/13;
					else
						eatBlockFrame=0;
					int type = m_blocks[i][j+8].type;
					if(type >=0 && type<7 && eatBlockFrame>=0 && eatBlockFrame<13)		//播放消失动画
					{
						m_blocks[i][j+8].pBlockSprite->pSprite->setPosition(ccp(gameAreaX+i*BLOCKWIDTH+m_blocks[i][j+8].x_move + BLOCKWIDTH/2, m_windowHeight - (gameAreaY+j*BLOCKWIDTH+m_blocks[i][j+8].y_move + (i + 1)%2 * BLOCKWIDTH/2 + BLOCKWIDTH/2)));
						m_blocks[i][j+8].pBlockSprite->pShadowSprite->setPosition(ccp(gameAreaX+i*BLOCKWIDTH+m_blocks[i][j+8].x_move + BLOCKWIDTH/2 + shadowOffSet, m_windowHeight - (gameAreaY+j*BLOCKWIDTH+m_blocks[i][j+8].y_move + (i + 1)%2 * BLOCKWIDTH/2 + BLOCKWIDTH/2 + shadowOffSet)));
					}
				}
				else     //若没在消失中
				{
					if (m_blocks[i][j+8].type > -1 && m_blocks[i][j+8].type <= 7)
					{
						m_blocks[i][j+8].pBlockSprite->pSprite->setPosition(ccp(gameAreaX+i*BLOCKWIDTH+m_blocks[i][j+8].x_move + BLOCKWIDTH/2, m_windowHeight - (gameAreaY +j*BLOCKWIDTH+m_blocks[i][j+8].y_move + (i + 1)%2 * BLOCKWIDTH/2 + BLOCKWIDTH/2)));
						m_blocks[i][j+8].pBlockSprite->pShadowSprite->setPosition(ccp(gameAreaX+i*BLOCKWIDTH+m_blocks[i][j+8].x_move + BLOCKWIDTH/2 + shadowOffSet, m_windowHeight - (gameAreaY +j*BLOCKWIDTH+m_blocks[i][j+8].y_move + (i + 1)%2 * BLOCKWIDTH/2 + BLOCKWIDTH/2 + shadowOffSet)));
					}
				}

			}
		}
	}
}

void CrazyCapGame::OnTouchMove(int x, int y)
{
	//不在游戏区，先不处理
	if(x < gameAreaX || y < gameAreaY || x > gameAreaX+gameAreaWidth || y > gameAreaY+gameAreaHeight)
	{
		return;
	}

	if(timerMoveBlock->GetState()==Timer::ERunning)return;

	if(m_selectedPos[0].x==-1)		//若没选好第一个块，不处理
	{
		return;
	}

	//计算位置
	Position p;
	p.x=(x-gameAreaX)/BLOCKWIDTH;
	if(p.x%2==0)
		p.y=(y-gameAreaY-BLOCKWIDTH/2)/BLOCKWIDTH+8;
	else
		p.y=(y-gameAreaY)/BLOCKWIDTH+8;
	if(p.y>15)p.y=15;

	if (m_selectedPos[0].x == p.x && m_selectedPos[0].y == p.y)		//若没移出第一个方块
	{
		return;
	}

	//如果移到了连接方块
	if(!CheckTwoPosLinked(p, m_selectedPos[0]))
	{
		return;
	}

	//如果在选第二个块时，第一次选中的块已经变为不可移动，清空选中状态，返回
	if( ! m_blocks[m_selectedPos[0].x][m_selectedPos[0].y].SelectAble() )
	{
		m_selectedPos[0].x=-1;
		m_selectedPos[1].x=-1;
		return;
	}

	//选中第二个
	m_selectedPos[1]=p;

	MoveBlockPair(m_selectedPos[0], m_selectedPos[1]);
}

bool CrazyCapGame::CheckTwoPosLinked(Position position1, Position position2)
{
	if(abs(position2.x-position1.x)+abs(position2.y-position1.y)==1  
		||  ( position1.x%2==1&& position1.y-position2.y==1&&abs(position2.x-position1.x)==1 )  
		||  ( position1.x%2==0&& position2.y-position1.y==1&&abs(position2.x-position1.x)==1 )    )
		return true;
	return false;
}

void CrazyCapGame::SetSelectAni(int x, int y)
{
	if (x != -1)		//隐藏
	{
		m_selectAni->setPosition(CCPointMake(x - 15, y - 15));
	}
	else				//显示
	{
		m_selectAni->setPosition(CCPointMake(-100, -100));
	}
}

void CrazyCapGame::OnTouchBegin(int x, int y)
{
	//不在游戏区，先不处理
	if(x < gameAreaX || y < gameAreaY || x > gameAreaX+gameAreaWidth || y > gameAreaY+gameAreaHeight)
	{
		return;
	}

	if(timerMoveBlock->GetState()==Timer::ERunning)return;

	Position p;
	p.x=(x-gameAreaX)/BLOCKWIDTH;
	if(p.x%2==0)
		p.y=(y-gameAreaY-BLOCKWIDTH/2)/BLOCKWIDTH+8;
	else
		p.y=(y-gameAreaY)/BLOCKWIDTH+8;
	if(p.y>15)p.y=15;

	if (m_selectedPos[0].x == p.x && m_selectedPos[0].y == p.y)		//若没移出第一个方块
	{
		return;
	}

	//如果选择正在下落中或者正在消减中的瓶盖，不处理
	if(m_blocks[p.x][p.y].isCanMove==false || m_blocks[p.x][p.y].alpha <255 || m_blocks[p.x][p.y].isDropping==true || m_blocks[p.x][p.y].x_move>0 ||  m_blocks[p.x][p.y].y_move>0)
		return;

	//如果选中一个状态处于不可移动的块，或者一个特殊块，置选中标志为空，返回
	if(	! m_blocks[p.x][p.y].SelectAble() )
	{
		if(m_blocks[p.x][p.y].special==2)
		{
			PlaySound(cantmove1);
		}
		else if(m_blocks[p.x][p.y].special==3)
		{
			PlaySound(cantmove1);
		}
		m_selectedPos[0].x=-1;
		m_selectedPos[1].x=-1;
		return;
	}

	PlaySound(selected);

	//当第一个方块没选的情况选中第一个方块
	if(m_selectedPos[0].x==-1)
	{				
		m_selectedPos[0]=p;
		if(m_selectedPos[0].x==-1)return;
		m_totalClickCount++;
		m_selectedPos[1].x=-1;
		long clickTime = millisecondNow();
		m_perClickTakeTime.push_back(clickTime - m_lastClickTime);
		m_lastClickTime = clickTime;
		SetSelectAni(p.x, p.y);
		return;
	}

	//如果第二次点击不贴着第一个方块，选中第一个
	if(!CheckTwoPosLinked(p, m_selectedPos[0]))
	{
		m_selectedPos[0]=p;
		SetSelectAni(p.x, p.y);
		return;
	}

	//如果在选第二个块时，第一次选中的块已经变为不可移动，清空选中状态，返回
	if( ! m_blocks[m_selectedPos[0].x][m_selectedPos[0].y].SelectAble() )
	{
		m_selectedPos[0].x=-1;
		m_selectedPos[1].x=-1;
		SetSelectAni(-1, -1);
		return;
	}

	//选中第二个
	m_selectedPos[1]=p;

	MoveBlockPair(m_selectedPos[0], m_selectedPos[1]);
}

void CrazyCapGame::MoveBlockPair(Position position1, Position position2)
{
	//移动
	PlaySound(capsmove);
	timerMoveBlock->Play();

	//计算方向
	if(position1.y>position2.y)
	{
		if(position1.x==position2.x)
		{
			direction1=0;
			direction2=3;
		}
		else if(position1.x>position2.x)
		{
			direction1=2;
			direction2=5;
		}
		else
		{
			direction1=1;
			direction2=4;
		}
	}
	else if(position1.y<position2.y)
	{
		if(position1.x==position2.x)
		{
			direction1=3;
			direction2=0;
		}
		else if(position1.x>position2.x)
		{
			direction1=4;
			direction2=1;
		}
		else
		{
			direction1=5;
			direction2=2;
		}
	}
	else
	{
		if(position1.x>position2.x)
		{
			if(position1.x%2==0)
			{
				direction1=2;
				direction2=5;
			}
			else
			{
				direction1=4;
				direction2=1;
			}
		}
		else
		{
			if(position1.x%2==0)
			{
				direction1=1;
				direction2=4;
			}
			else
			{
				direction1=5;
				direction2=2;
			}
		}
	}
	//播放动画
	if(m_blocks[position1.x][position1.y].type>=0 && m_blocks[position2.x][position2.y].type>=0)
	{
		PlayAni(position1.x, position1.y,direction1);
		PlayAni(position2.x, position2.y,direction2);
	}

	SetSelectAni(-1, -1);
}

void CrazyCapGame::OnTouchEnd(int x, int y)
{

}

void CrazyCapGame::Init()
{
	m_windowHeight = CCDirector::sharedDirector()->getWinSize().height;
	//计时器
	timerMoveBlock = new Timer();
	timerEatBlock = new Timer();
	timerDropDown = new Timer();
	timerCombo = new Timer();
	timerComboCount = new Timer();
	timerPaticles = new Timer();
	timerGameEnd = new Timer();
	m_timerGame = new Timer();

	m_gameMode = 0;

	//初始化瓶盖图片池
	CCSpriteFrameCache *frameCache = CCSpriteFrameCache::sharedSpriteFrameCache();  
	frameCache->addSpriteFramesWithFile("111.plist");
	char * name = new char[20];
	for (int i=0; i<6; ++i)
	{
		sprintf(name, "Item_%d.png", i+1);
		for (unsigned int j=0; j<40; ++j)
		{
			BlockSprite * pSprite = new BlockSprite();
			pSprite->pSprite = new CCSprite();
			pSprite->pSprite->init();
			pSprite->pShadowSprite = new CCSprite();
			pSprite->pShadowSprite->init();
			pSprite->pDefaultSpriteFrame = frameCache->spriteFrameByName(name);
			pSprite->pSprite->setDisplayFrame(pSprite->pDefaultSpriteFrame);
			pSprite->pShadowSprite->setDisplayFrame(pSprite->pDefaultSpriteFrame);
			pSprite->pShadowSprite->setColor(ccBLACK);
			m_availableSprite[i].push_back(pSprite);
		}
	}

	//初始化各方向动画
	for (unsigned int type=0; type<6; ++type)
	{
		for (unsigned int dir=0; dir<6; ++dir)
		{
			//sprintf(name, "Block%d-%d.png", type + 1, dir % 3 + 1);
			sprintf(name, "block%d-%d.png", type + 1, dir % 3 + 1);
			CCTexture2D * pTexture = CCTextureCache::sharedTextureCache()->addImage(name);
			CCArray * pArray = new CCArray(25);
			for (unsigned int i=0; i<25; ++i)
			{
				CCSpriteFrame * pSpriteFrame = NULL;
				if (dir >= 3)
				{
					unsigned int num = (i + 24) % 25;
					pSpriteFrame = CCSpriteFrame::createWithTexture(pTexture, CCRectMake((4 - (num % 5)) * 60, (4 - (num / 5)) * 60, 60, 60));
				}
				else
				{
					pSpriteFrame = CCSpriteFrame::createWithTexture(pTexture, CCRectMake((i % 5) * 60, (i / 5) * 60, 60, 60));
				}
				pArray->addObject(pSpriteFrame);
			}
			m_blockMoveAni[type][dir] = new CCAnimation();
			m_blockMoveAni[type][dir]->initWithSpriteFrames(pArray, 0.02f);
		}
	}
	

	//初始化选中动画
	m_selectAni = new CCSprite();
	m_selectAni->init();
	CCAnimation * selectAni = new CCAnimation();
	CCTexture2D * pTexture = CCTextureCache::sharedTextureCache()->addImage("selectAni.png");
	CCArray * pArray = new CCArray(25);
	for (unsigned int i=0; i<25; ++i)
	{
		CCSpriteFrame * pSpriteFrame = CCSpriteFrame::createWithTexture(pTexture, CCRectMake((i % 5) * 90, (i / 5) * 90, 90, 90));
		pArray->addObject(pSpriteFrame);
	}
	selectAni->initWithSpriteFrames(pArray, 0.02f);
	CCAnimate * pAnimate = CCAnimate::create(selectAni);
	m_selectAni->runAction(CCRepeatForever::create(pAnimate));
	m_curLayer->addChild(m_selectAni, 1);

	delete [] name;

	m_soundEffectMap[selected] = "pgsound/selected.wav";
	m_soundEffectMap[eat] = "pgsound/eat.wav";
	m_soundEffectMap[combo2] = "pgsound/combo2.wav";
	m_soundEffectMap[combo3] = "pgsound/combo3.wav";
	m_soundEffectMap[combo4] = "pgsound/combo4.wav";
	m_soundEffectMap[combo5] = "pgsound/combo5.wav";
	m_soundEffectMap[combo6] = "pgsound/combo6.wav";
	m_soundEffectMap[combo7] = "pgsound/combo7.wav";
	m_soundEffectMap[combo8] = "pgsound/combo8.wav";
	m_soundEffectMap[combo9] = "pgsound/combo9.wav";
	m_soundEffectMap[combo10] = "pgsound/combo10.wav";
	m_soundEffectMap[dropdown] = "pgsound/dropdown.wav";
	m_soundEffectMap[capsmove] = "pgsound/capsmove.wav";
	m_soundEffectMap[cantmove1] = "pgsound/cantmove1.wav";
	m_soundEffectMap[cantmove2] = "pgsound/cantmove2.wav";
	//m_soundEffectMap[supercomboitem] = "pgsound/supercomboitem.wav";
	//m_soundEffectMap[nocomboitem] = "pgsound/nocomboitem.wav";
	//m_soundEffectMap[cantmoveitem] = "pgsound/cantmoveitem.wav";
	//m_soundEffectMap[tips] = "pgsound/tips.wav";
	//m_soundEffectMap[itemattack] = "pgsound/itemattack.wav";
	//m_soundEffectMap[mousedown] = "pgsound/mousedown.wav";
	//m_soundEffectMap[useitem] = "pgsound/useitem.wav";
	//m_soundEffectMap[explode] = "pgsound/explode.wav";
	//预装载音效
	std::map<int, std::string>::iterator iter = m_soundEffectMap.begin();
	std::map<int, std::string>::iterator iterEnd = m_soundEffectMap.end();
	for (; iter != iterEnd; ++iter)
	{
		CocosDenshion::SimpleAudioEngine::sharedEngine()->preloadEffect( iter->second.c_str() );
	}
	
	CocosDenshion::SimpleAudioEngine::sharedEngine()->preloadBackgroundMusic("pgsound/music.mp3");
	CocosDenshion::SimpleAudioEngine::sharedEngine()->playBackgroundMusic("pgsound/music.mp3", true);

	StartGame();
}

void CrazyCapGame::PlayAni(int x, int y, int dir, bool loop)
{
	if (loop)		//循环播放
	{
		CCAnimate * pAnimate = CCAnimate::create(m_blockMoveAni[m_blocks[x][y].type][dir]);
		CCAction * pAction = CCRepeatForever::create(pAnimate);
		m_blocks[x][y].pBlockSprite->pSprite->runAction(pAction);

		pAnimate = CCAnimate::create(m_blockMoveAni[m_blocks[x][y].type][dir]);
		pAction = CCRepeatForever::create(pAnimate);
		m_blocks[x][y].pBlockSprite->pShadowSprite->runAction(pAction);
	}
	else			//播放一次
	{
		CCAnimate * pAnimate = CCAnimate::create(m_blockMoveAni[m_blocks[x][y].type][dir]);
		CCCallFuncND * callFunc = CCCallFuncND::create(m_blocks[x][y].pBlockSprite->pSprite, callfuncND_selector(CrazyCapGame::CallBackND), m_blocks[x][y].pBlockSprite);
		CCAction * pAction = CCSequence::create(pAnimate, callFunc, NULL);
		m_blocks[x][y].pBlockSprite->pSprite->runAction(pAction);

		pAnimate = CCAnimate::create(m_blockMoveAni[m_blocks[x][y].type][dir]);
		m_blocks[x][y].pBlockSprite->pShadowSprite->runAction(pAnimate);
	}
}

void CrazyCapGame::CallBackND(CCNode * sender, void * data)
{
	BlockSprite * pSprite = (BlockSprite *)data;
	pSprite->pSprite->setDisplayFrame(pSprite->pDefaultSpriteFrame);
	pSprite->pShadowSprite->setDisplayFrame(pSprite->pDefaultSpriteFrame);
}

BlockSprite * CrazyCapGame::GetFreeBlockSprite(int type)
{
	std::list<BlockSprite *> & list = m_availableSprite[type];
	BlockSprite * pSprite = *list.begin();

	m_availableSprite[type].erase(m_availableSprite[type].begin());
	m_curLayer->addChild(pSprite->pSprite, 1);
	m_curLayer->addChild(pSprite->pShadowSprite, 0);
	return pSprite;
}

void CrazyCapGame::MakeSpriteFree(int type, BlockSprite * pBlockSprite)
{
	m_curLayer->removeChild(pBlockSprite->pSprite, false);
	m_curLayer->removeChild(pBlockSprite->pShadowSprite, false);
	pBlockSprite->pSprite->stopAllActions();
	pBlockSprite->pShadowSprite->stopAllActions();
	pBlockSprite->pSprite->setDisplayFrame(pBlockSprite->pDefaultSpriteFrame);
	pBlockSprite->pShadowSprite->setDisplayFrame(pBlockSprite->pDefaultSpriteFrame);
	m_availableSprite[type].push_back(pBlockSprite);
}

void CrazyCapGame::PlaySound(int soundID)
{
	if (m_soundEffectMap.find(soundID) == m_soundEffectMap.end())
	{
		return;
	}
	CocosDenshion::SimpleAudioEngine::sharedEngine()->playEffect(m_soundEffectMap[soundID].c_str());
}


void CrazyCapGame::CreatePaticleSystem(int x, int y, int startTime)
{
	int startX=gameAreaX+x*BLOCKWIDTH;
	int startY=gameAreaY+(y - 8)*BLOCKWIDTH;
	if(x%2==0) startY+=BLOCKWIDTH/2;

	PaticleSystem* pt=paticleSystemHead;
	if(paticleSystemHead==NULL)
	{
		paticleSystemHead=new PaticleSystem();
		paticleSystemHead->next=NULL;
		pt=paticleSystemHead;
	}
	else
	{
		//---------------查找链表结尾-------------//
		while(pt->next!=NULL)
		{
			pt=pt->next;

		}
		pt->next=new PaticleSystem();
		pt=pt->next;
		pt->next=NULL;
	}

	int xSpeed=rand()%100-50;
	int ySpeed=rand()%20-50;
	int rotateSpeed=2+rand()%3;

	pt->p.xSpeed=xSpeed;
	pt->p.ySpeed=ySpeed;
	pt->p.startX=startX;
	pt->p.startY=startY;
	pt->p.rotateSpeed=rotateSpeed;
	pt->p.type=m_blocks[x][y].type;
	pt->p.x=startX;
	pt->p.y=startY;
	pt->startTime=startTime;
	pt->p.pBlockSprite = m_blocks[x][y].pBlockSprite;
	m_curLayer->reorderChild(pt->p.pBlockSprite->pSprite, 4);
	m_curLayer->reorderChild(pt->p.pBlockSprite->pShadowSprite, 3);

	////------------------添加影子------------//

	//pt->next=new PaticleSystem();
	//pt=pt->next;
	//pt->next=NULL;

	//pt->p.xSpeed=xSpeed/2;
	//pt->p.ySpeed=ySpeed-1;
	//pt->p.startX=startX;
	//pt->p.startY=startY;
	//pt->p.rotateSpeed=rotateSpeed;
	//pt->p.type=7;
	//pt->p.x=startX;
	//pt->p.y=startY;
	//pt->startTime=startTime;
	//pt->p.pBlockSprite = m_blocks[x][y].pBlockSprite;

	//m_curLayer->reorderChild(pt->p.pBlockSprite->pShadowSprite, 1);
}

void CrazyCapGame::DeleteANodeFormPaticleSystem(PaticleSystem *p)
{
	MakeSpriteFree(p->p.type, p->p.pBlockSprite);
	PaticleSystem * ptOld;
	if(paticleSystemHead==p)
	{
		ptOld=paticleSystemHead;
		paticleSystemHead=paticleSystemHead->next;
		delete ptOld;
		return;
	}
	PaticleSystem * pt=paticleSystemHead;


	while(pt!=p)		//找到p指向的节点
	{
		ptOld=pt;
		pt=pt->next;
	}

	//--------------删除结点------------//
	ptOld->next=pt->next;
	delete pt;

}