#include "HelloWorldScene.h"
#include "CrazyCapGame.h"

using namespace cocos2d;

CCScene* HelloWorld::scene()
{
    CCScene * scene = NULL;
    do 
    {
        // 'scene' is an autorelease object
        scene = CCScene::create();
        CC_BREAK_IF(! scene);

        // 'layer' is an autorelease object
        HelloWorld *layer = HelloWorld::create();
        CC_BREAK_IF(! layer);

        // add layer as a child to scene
        scene->addChild(layer);

		layer->DoMyInit();
    } while (0);

    // return the scene
    return scene;
}

void HelloWorld::DoMyInit()
{
	m_pCrazyCapGame = new CrazyCapGame();
	m_pCrazyCapGame->m_curLayer = this;
	m_pCrazyCapGame->Init();
}

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
    bool bRet = false;
    do 
    {
        //////////////////////////////////////////////////////////////////////////
        // super init first
        //////////////////////////////////////////////////////////////////////////

        CC_BREAK_IF(! CCLayer::init());

        //////////////////////////////////////////////////////////////////////////
        // add your codes below...
        //////////////////////////////////////////////////////////////////////////

        // 1. Add a menu item with "X" image, which is clicked to quit the program.

        // Create a "close" menu item with close icon, it's an auto release object.
        CCMenuItemImage *pCloseItem = CCMenuItemImage::create(
            "CloseNormal.png",
            "CloseSelected.png",
            this,
            menu_selector(HelloWorld::menuCloseCallback));
        CC_BREAK_IF(! pCloseItem);

        // Place the menu item bottom-right conner.
        pCloseItem->setPosition(ccp(CCDirector::sharedDirector()->getWinSize().width - 20, 20));

        // Create a menu with the "close" menu item, it's an auto release object.
        CCMenu* pMenu = CCMenu::create(pCloseItem, NULL);
        pMenu->setPosition(CCPointZero);
        CC_BREAK_IF(! pMenu);

        // Add the menu to HelloWorld layer as a child layer.
        this->addChild(pMenu, 1);

		CCLayerColor::initWithColor(ccc4(150,150,150,150));

        // 2. Add a label shows "Hello World".

        //// Create a label and initialize with string "Hello World".
        //CCLabelTTF* pLabel = CCLabelTTF::create("Hello World", "Arial", 24);
        //CC_BREAK_IF(! pLabel);

        //// Get window size and place the label upper. 
        //CCSize size = CCDirector::sharedDirector()->getWinSize();
        //pLabel->setPosition(ccp(size.width / 2, size.height - 50));

        //// Add the label to HelloWorld layer as a child layer.
        //this->addChild(pLabel, 1);

        //// 3. Add add a splash screen, show the cocos2d splash image.
        //CCSprite* pSprite = CCSprite::create("block1-1.png");
        //CC_BREAK_IF(! pSprite);

        //// Place the sprite on the center of the screen
        //pSprite->setPosition(ccp(size.width/2, size.height/2));

        //// Add the sprite to HelloWorld layer as a child layer.
        //this->addChild(pSprite, 0);

		this->schedule(schedule_selector(HelloWorld::Update));
		this->setTouchEnabled(true);
        bRet = true;
    } while (0);

    return bRet;
}

void HelloWorld::Update(float dt)
{
	m_pCrazyCapGame->Update();
	Timer::s_currentTime = millisecondNow();
}

void HelloWorld::menuCloseCallback(CCObject* pSender)
{
    // "close" menu item clicked
    CCDirector::sharedDirector()->end();
}

void HelloWorld::ccTouchesMoved(CCSet *pTouches, CCEvent *pEvent)
{
	CCTouch* touch = (CCTouch*)( pTouches->anyObject() );
	CCPoint location = touch->getLocationInView();
	location = CCDirector::sharedDirector()->convertToGL(location);
	m_pCrazyCapGame->OnTouchMove(location.x, m_pCrazyCapGame->m_windowHeight - location.y);
}

void HelloWorld::ccTouchesBegan(cocos2d::CCSet* touches, cocos2d::CCEvent* event)
{
	CCTouch* touch = (CCTouch*)( touches->anyObject() );
	CCPoint location = touch->getLocationInView();
	location = CCDirector::sharedDirector()->convertToGL(location);
	m_pCrazyCapGame->OnTouchBegin(location.x, m_pCrazyCapGame->m_windowHeight - location.y);
}

void HelloWorld::ccTouchesEnded(cocos2d::CCSet* touches, cocos2d::CCEvent* event)
{
	CCTouch* touch = (CCTouch*)( touches->anyObject() );
	CCPoint location = touch->getLocationInView();
	location = CCDirector::sharedDirector()->convertToGL(location);
	m_pCrazyCapGame->OnTouchEnd(location.x, m_pCrazyCapGame->m_windowHeight - location.y);
}


