#include "Menu.h"
#include "GameLayer.h"
#include "BossLayer.h"
#include "GameOver.h"

#include "SimpleAudioEngine.h"

using namespace CocosDenshion;

GameLayer::GameLayer()
{
}

GameLayer::~GameLayer()
{
}

bool GameLayer::init()
{
	bool bRet = false;
	do
	{
		CC_BREAK_IF(! CCLayer::init());

		SimpleAudioEngine::sharedEngine()->playBackgroundMusic("bgm1.wav", true); // 开启背景音乐

		// 创建玩家
		player = new Player("plane.png");
		CC_BREAK_IF(! player);
		player->setPosition(ccp(200, 80));
		this->addChild(player->getsprite(), 2);

		// 创建背景
		m_bg1 = CCSprite::spriteWithFile("bg1.png");
		m_bg2 = CCSprite::spriteWithFile("bg2.png");
		CC_BREAK_IF(! m_bg1);
		CC_BREAK_IF(! m_bg2);
		CCSize size = CCDirector::sharedDirector()->getWinSize(); // 获取OpenGL View的大小，单位为点
		m_bg1->setAnchorPoint(CCPointZero);
		m_bg2->setAnchorPoint(CCPointZero);
		m_bg1->setPosition(ccp(0, 0));
		m_bg1->setPosition(ccp(0, 600));
		this->addChild(m_bg1, 0, 1);
		this->addChild(m_bg2, 0, 2);

		// 创建生命值精灵
		life1 = CCSprite::spriteWithFile("life.png");
		life2 = CCSprite::spriteWithFile("life.png");
		life3 = CCSprite::spriteWithFile("life.png");
		CC_BREAK_IF(! life1);
		CC_BREAK_IF(! life2);
		CC_BREAK_IF(! life3);
		life1->setPosition(ccp(340, 580));
		life2->setPosition(ccp(360, 580));
		life3->setPosition(ccp(380, 580));
		this->addChild(life1, 1);
		this->addChild(life2, 1);
		this->addChild(life3, 1);
		playerLife = 3; // 初始化生命值

		// 创建炸弹，共三个
		boom1 = new Bullet("boom.png");
		boom2 = new Bullet("boom.png");
		boom3 = new Bullet("boom.png");
		CC_BREAK_IF(! boom1);
		CC_BREAK_IF(! boom2);
		CC_BREAK_IF(! boom3);
		boom1->setPosition(ccp(360, 30));
		boom2->setPosition(ccp(370, 30));
		boom3->setPosition(ccp(380, 30));
		this->addChild(boom1->getsprite(), 1);
		this->addChild(boom2->getsprite(), 1);
		this->addChild(boom3->getsprite(), 1);
		boomNum = 3; // 初始化炸弹数量

		// 初始化炸弹特效
		m_explore1 = new CCParticleSystemQuad();
		m_explore2 = new CCParticleSystemQuad();
		m_explore3 = new CCParticleSystemQuad();
		m_explore1->setIsVisible(false);
		m_explore2->setIsVisible(false);
		m_explore3->setIsVisible(false);
		this->addChild(m_explore1, 10);
		this->addChild(m_explore2, 11);
		this->addChild(m_explore3, 12);

		setIsTouchEnabled(true); // 触屏开启
		int stepindex = -1;
		scheduleUpdate();

		enemyPlane1 = CCArray::arrayWithCapacity(10); // 创建五种敌人类型
		enemyPlane2 = CCArray::arrayWithCapacity(7);
		enemyPlane3 = CCArray::arrayWithCapacity(10);
		enemyPlane4 = CCArray::arrayWithCapacity(10);
		enemyPlane5 = CCArray::arrayWithCapacity(10);
		for (int i = 0; i < enemyPlane1->capacity(); i++)
		{
			Enemy* enemy = new Enemy("enemy2.png", 0);
			CC_BREAK_IF(! enemy);
			enemy->setPosition(ccp(CCRANDOM_0_1()*  400, 640));
			(enemy->getsprite())->setIsVisible(false);
			this->addChild(enemy->getsprite(), 200 + i);
			enemyPlane1->addObject(enemy);
		}
		for (int i = 0; i < enemyPlane2->capacity(); i++)
		{
			Enemy* enemy = new Enemy("enemy3.png", 1);
			CC_BREAK_IF(! enemy);
			enemy->setPosition(ccp(50 + i*  50, 640));
			(enemy->getsprite())->setIsVisible(false);
			this->addChild(enemy->getsprite(), 200 + i);
			enemyPlane2->addObject(enemy);
		}
		for (int i = 0; i < enemyPlane3->capacity(); i++)
		{
			Enemy* enemy = new Enemy("enemy4.png", 2);
			CC_BREAK_IF(! enemy);
			enemy->setPosition(ccp(CCRANDOM_0_1()*  400, 640));
			(enemy->getsprite())->setIsVisible(false);
			this->addChild(enemy->getsprite(), 200 + i);
			enemyPlane3->addObject(enemy);
		}
		for (int i = 0; i < enemyPlane4->capacity(); i++)
		{
			Enemy* enemy = new Enemy("enemy5.png", 3);
			CC_BREAK_IF(! enemy);
			// 对称排布
			if (i % 2 == 0)
				enemy->setPosition(ccp(20, 640));
			else
				enemy->setPosition(ccp(380, 640));
			(enemy->getsprite())->setIsVisible(false);
			this->addChild(enemy->getsprite(), 200 + i);
			enemyPlane4->addObject(enemy);
		}
		for (int i = 0; i < enemyPlane5->capacity(); i++)
		{
			Enemy* enemy = new Enemy("enemy6.png", 4);
			CC_BREAK_IF(! enemy);
			enemy->setPosition(ccp(200, 640));
			(enemy->getsprite())->setIsVisible(false);
			this->addChild(enemy->getsprite(), 200 + i);
			enemyPlane5->addObject(enemy);
		}
		enemyPlane1->retain();
		enemyPlane2->retain();
		enemyPlane3->retain();
		enemyPlane4->retain();
		enemyPlane5->retain();

		playerBullets = CCArray::arrayWithCapacity(10); // 创建玩家子弹，自动发射
		enemyBullets = CCArray::arrayWithCapacity(20); // 创建敌机子弹
		for (int i = 0; i < playerBullets->capacity(); i++)
		{
			Bullet* bullet = new Bullet("bullet.png");
			CC_BREAK_IF(! bullet);
			bullet->setType(0);
			(bullet->getsprite())->setIsVisible(false);
			this->addChild(bullet->getsprite(), 200 + i);
			playerBullets->addObject(bullet);
		}
		for (int i = 0; i < enemyBullets->capacity(); i++)
		{
			Bullet* bullet = new Bullet("enemybullet.png");
			CC_BREAK_IF(! bullet);
			bullet->setPosition(ccp(200, 800));
			bullet->setType(1);
			(bullet->getsprite())->setIsVisible(false);
			this->addChild(bullet->getsprite(), 200 + i);
			enemyBullets->addObject(bullet);
		}
		playerBullets->retain();
		enemyBullets->retain();

		plane1Effects = CCArray::arrayWithCapacity(10); // 死亡时的爆炸效果
		plane2Effects = CCArray::arrayWithCapacity(10);
		plane3Effects = CCArray::arrayWithCapacity(10);
		plane4Effects = CCArray::arrayWithCapacity(10);
		for (int i = 0; i < plane1Effects->capacity(); i++)
		{
			CCSprite* effect = new CCSprite;
			CC_BREAK_IF(! effect);
			effect = CCSprite::spriteWithFile("explored.png");
			effect->setPosition(ccp(200, 800));
			effect->setIsVisible(false);
			this->addChild(effect, 100 + i);
			plane1Effects->addObject(effect);
		}
		for (int i = 0; i < plane2Effects->capacity(); i++)
		{
			CCSprite* effect = new CCSprite;
			CC_BREAK_IF(! effect);
			effect = CCSprite::spriteWithFile("explored.png");
			effect->setPosition(ccp(200, 800));
			effect->setIsVisible(false);
			this->addChild(effect, 110 + i);
			plane2Effects->addObject(effect);
		}
		for (int i = 0; i < plane3Effects->capacity(); i++)
		{
			CCSprite* effect = new CCSprite;
			CC_BREAK_IF(! effect);
			effect = CCSprite::spriteWithFile("explored.png");
			effect->setPosition(ccp(200, 800));
			effect->setIsVisible(false);
			this->addChild(effect, 110 + i);
			plane3Effects->addObject(effect);
		}
		for (int i = 0; i < plane4Effects->capacity(); i++)
		{
			CCSprite* effect = new CCSprite;
			CC_BREAK_IF(! effect);
			effect = CCSprite::spriteWithFile("explored.png");
			effect->setPosition(ccp(200, 800));
			effect->setIsVisible(false);
			this->addChild(effect, 110 + i);
			plane4Effects->addObject(effect);
		}
		plane1Effects->retain();
		plane2Effects->retain();
		plane3Effects->retain();
		plane4Effects->retain();

		// 玩家特效
		playerEffect = new CCSprite;
		playerEffect = CCSprite::spriteWithFile("explored.png");
		CC_BREAK_IF(! playerEffect);
		playerEffect->setIsVisible(false);
		this->addChild(playerEffect, 99);

		// 创建摇杆
		CCSprite* jsPoint = CCSprite::spriteWithFile("point.png");
		CCSprite* jsBG = CCSprite::spriteWithFile("joystickbg.png");
		CC_BREAK_IF(! jsPoint);
		CC_BREAK_IF(! jsBG);
		joyStick = new JoyStick;
		joyStick = JoyStick::JoyStickWithCenter(ccp(70, 70), 48, jsPoint, jsBG);
		CC_BREAK_IF(! joyStick);
		this->addChild(joyStick, 1);
		joyStick->Active();
		js = false;
		score = 0;
		bulletSpeed = 1.2;

		this->schedule(schedule_selector(GameLayer::updateBullets), bulletSpeed); // 控制子弹发射间隔
		this->schedule(schedule_selector(GameLayer::updateJS));

		// 游戏逻辑更新
		this->schedule(schedule_selector(GameLayer::updateStep1), 5);
		this->schedule(schedule_selector(GameLayer::updateStep2), 10);
		this->schedule(schedule_selector(GameLayer::updateStep3), 13);
		this->schedule(schedule_selector(GameLayer::updateStep4), 16);
		this->schedule(schedule_selector(GameLayer::updateStep5), 23);
		this->schedule(schedule_selector(GameLayer::updateStep6), 40);

		bRet = true;
	} while (0);

	return bRet;
}

CCScene* GameLayer::scene()
{
	CCScene* scene = NULL;
	do
	{
		scene = CCScene::node();
		CC_BREAK_IF(! scene);

		GameLayer* layer = GameLayer::node();
		CC_BREAK_IF(! layer);

		scene->addChild(layer);
	} while (0);

	return scene;
}

bool GameLayer::outScreen(BaseSprite* sp)
{
	CCPoint spp = (sp->getsprite())->getPosition();
	CCSize spsize = (sp->getsprite())->getContentSize();
	if ((spp.x - spsize.width / 2.5) < 0 ||
		(spp.x + spsize.width / 2.5) > 400 ||
		(spp.y - spsize.height / 2) < 0 ||
		(spp.y + spsize.height / 2) > 600)
		return true;
	return false;
}

void GameLayer::playerDead()
{
	(player->getsprite())->stopAllActions();
	switch (playerLife)
	{
	case 3:
		life1->setIsVisible(false);
		playerLife--;
		break;
	case 2:
		life2->setIsVisible(false);
		playerLife--;
		break;
	case 1:
		life3->setIsVisible(false);
		playerLife--;
		break;
	}
	CCActionInterval* action1 = CCFadeIn::actionWithDuration(0.3);
	CCActionInterval* action2 = CCFadeOut::actionWithDuration(0.3);
	playerEffect->setPosition(playerp);
	playerEffect->setIsVisible(true);
	playerEffect->runAction(CCSequence::actions(action1, action2, NULL));
	CCActionInterval* action = CCBlink::actionWithDuration(3, 6);
	(player->getsprite())->runAction(action);

	if (playerLife == 0) // 游戏结束
	{
		(player->getsprite())->stopAllActions();
		(player->getsprite())->setIsVisible(false);
		joyStick->Inactive(); // 摇杆失效
		SimpleAudioEngine::sharedEngine()->stopBackgroundMusic(); // 停止背景音乐，切换场景

		CCDirector::sharedDirector()->setDepthTest(true);
		CCDirector::sharedDirector()->replaceScene(CCTransitionFadeUp::transitionWithDuration(1.2, GameOver::scene()));
	}
}

void GameLayer::allToDead()
{
	CCSize playersize = player->getSize();
	m_explore1->initWithFile("ExplodingRing.plist"); // 炸弹粒子特效
	m_explore1->setPosition(ccp(playerp.x, (playerp.y + playersize.height / 1.5) + 200));
	m_explore1->setIsVisible(true);
	SimpleAudioEngine::sharedEngine()->playEffect("boom.wav", false); // 炸弹音效
	for (int i = 0; i < enemyBullets->capacity(); i++)
	{
		Bullet* bullet = (Bullet*)(enemyBullets->objectAtIndex(i));
		if ((bullet->getsprite())->getIsVisible())
			(bullet->getsprite())->setIsVisible(false);
	}
	// 不同类型敌人相继死亡，玩家得分
	for (int i = 0; i < enemyPlane1->capacity(); i++)
	{
		Enemy* enemy = (Enemy*)(enemyPlane1->objectAtIndex(i));
		CCSprite* effect = (CCSprite*)(plane1Effects->objectAtIndex(i));
		CCActionInterval* action1 = CCFadeIn::actionWithDuration(0.3);
		CCActionInterval* action2 = CCFadeOut::actionWithDuration(0.3);
		CCPoint enemyp = (enemy->getsprite())->getPosition();
		if ((enemy->getsprite())->getIsVisible())
		{				
			enemy->setDead();
			effect->setPosition(enemyp);
			effect->setIsVisible(true);
			effect->runAction(CCSequence::actions(action1, action2, NULL));
			getScore();
		}
	}
	for	(int i = 0; i < enemyPlane2->capacity(); i++)
	{
		Enemy* enemy = (Enemy*)(enemyPlane2->objectAtIndex(i));
		CCSprite* effect = (CCSprite*)(plane2Effects->objectAtIndex(i));
		CCActionInterval* action1 = CCFadeIn::actionWithDuration(0.3);
		CCActionInterval* action2 = CCFadeOut::actionWithDuration(0.3);
		CCPoint enemyp = (enemy->getsprite())->getPosition();
		if ((enemy->getsprite())->getIsVisible())
		{
			enemy->setDead();
			effect->setPosition(enemyp);
			effect->setIsVisible(true);
			effect->runAction(CCSequence::actions(action1, action2, NULL));
			getScore();
		}
	}
	for	(int i = 0; i < enemyPlane3->capacity(); i++)
	{
		Enemy* enemy = (Enemy*)(enemyPlane3->objectAtIndex(i));
		CCSprite* effect = (CCSprite*)(plane3Effects->objectAtIndex(i));
		CCActionInterval* action1 = CCFadeIn::actionWithDuration(0.3);
		CCActionInterval* action2 = CCFadeOut::actionWithDuration(0.3);
		CCPoint enemyp = (enemy->getsprite())->getPosition();
		if ((enemy->getsprite())->getIsVisible())
		{
			enemy->setDead();
			effect->setPosition(enemyp);
			effect->setIsVisible(true);
			effect->runAction(CCSequence::actions(action1, action2, NULL));
			getScore();
		}
	}
	for	(int i = 0; i < enemyPlane4->capacity(); i++)
	{
		Enemy* enemy = (Enemy*)(enemyPlane4->objectAtIndex(i));
		CCSprite* effect = (CCSprite*)(plane4Effects->objectAtIndex(i));
		CCActionInterval* action1 = CCFadeIn::actionWithDuration(0.3);
		CCActionInterval* action2 = CCFadeOut::actionWithDuration(0.3);
		CCPoint enemyp = (enemy->getsprite())->getPosition();
		if ((enemy->getsprite())->getIsVisible())
		{
			enemy->setDead();
			effect->setPosition(enemyp);
			effect->setIsVisible(true);
			effect->runAction(CCSequence::actions(action1, action2, NULL));
			getScore();
		}
	}
}

void GameLayer::getScore()
{
	score++;
	if (score >= WIN) // WIN为过关条件
	{
		(player->getsprite())->stopAllActions();
		joyStick->Inactive();

		allToDead();
		unschedule(schedule_selector(GameLayer::update));
		schedule(schedule_selector(GameLayer::updateBoss), 3);
	}
}

void GameLayer::update(ccTime dt)
{
	// 背景更新
	CCPoint m_bg1p = m_bg1->getPosition();
	CCPoint m_bg2p = m_bg2->getPosition();
	m_bg1->setPosition(ccp(m_bg1p.x, m_bg1p.y - 2));
	m_bg2->setPosition(ccp(m_bg2p.x, m_bg2p.y - 2));
	if (m_bg2p.y < 0)
	{
		float temp = m_bg2p.y + 600;
		m_bg1->setPosition(ccp(m_bg1p.x, temp));
	}
	if (m_bg1p.y < 0)
	{
		float temp = m_bg1p.y + 600;
		m_bg2->setPosition(ccp(m_bg2p.x, temp));
	}

	playerp = (player->getsprite())->getPosition();

	for (int i = 0; i < playerBullets->capacity(); i++)
	{
		Bullet* bullet = (Bullet*)(playerBullets->objectAtIndex(i));
		if ((bullet->getsprite())->getIsVisible()) // 若子弹可以见，则执行逻辑
		{
			bullet->tick();
			bullet->tick();
		}
	}

	// 检测不同类型敌人的碰撞，包括子弹、玩家等，并伴有特效
	for (int i = 0; i < enemyPlane1->capacity(); i++)
	{
		Enemy* enemy = (Enemy*)(enemyPlane1->objectAtIndex(i));
		CCSprite* effect = (CCSprite*)(plane1Effects->objectAtIndex(i));
		CCPoint enemyp = (enemy->getsprite())->getPosition();
		CCActionInterval* action1 = CCFadeIn::actionWithDuration(0.3);
		CCActionInterval* action2 = CCFadeOut::actionWithDuration(0.3);
		for (int j = 0; j < playerBullets->capacity(); j++)
		{
			Bullet* bullet = (Bullet*)(playerBullets->objectAtIndex(j));
			if ((enemy->getsprite())->getIsVisible() && (bullet->getsprite())->getIsVisible() && enemy->collisionWithBullet(bullet))
			{				
				enemy->setDead();
				effect->setPosition(enemyp);
				effect->setIsVisible(true);
				effect->runAction(CCSequence::actions(action1, action2, NULL));
				bullet->setPosition(ccp(200, 800));
				(bullet->getsprite())->setIsVisible(false);
				getScore();
			}
			if ((enemy->getsprite())->getIsVisible() && enemy->iscollision(player)) // 与敌人相撞
			{
				enemy->setDead();
				effect->setPosition(enemyp);
				effect->setIsVisible(true);
				effect->runAction(CCSequence::actions(action1, action2, NULL));
				playerDead();
				getScore();
			}
		}
	}
	for (int i = 0; i < enemyPlane2->capacity(); i++)
	{
		Enemy* enemy = (Enemy*)(enemyPlane2->objectAtIndex(i));
		CCSprite* effect = (CCSprite*)(plane2Effects->objectAtIndex(i));
		CCPoint enemyp = (enemy->getsprite())->getPosition();
		CCActionInterval* action1 = CCFadeIn::actionWithDuration(0.3);
		CCActionInterval* action2 = CCFadeOut::actionWithDuration(0.3);
		for (int j = 0; j < playerBullets->capacity(); j++)
		{
			Bullet* bullet = (Bullet*)(playerBullets->objectAtIndex(j));
			if ((enemy->getsprite())->getIsVisible() && (bullet->getsprite())->getIsVisible() && enemy->collisionWithBullet(bullet))
			{
				enemy->setDead();
				effect->setPosition(enemyp);
				effect->setIsVisible(true);
				effect->runAction(CCSequence::actions(action1, action2, NULL));
				bullet->setPosition(ccp(200, 800));
				(bullet->getsprite())->setIsVisible(false);
				getScore();
			}
			if ((enemy->getsprite())->getIsVisible() && enemy->iscollision(player))
			{
				enemy->setDead();
				effect->setPosition(enemyp);
				effect->setIsVisible(true);
				effect->runAction(CCSequence::actions(action1, action2, NULL));
				playerDead();
				getScore();
			}
		}
	}
	for (int i = 0; i < enemyPlane3->capacity(); i++)
	{
		Enemy* enemy = (Enemy*)(enemyPlane3->objectAtIndex(i));
		CCSprite* effect = (CCSprite*)(plane3Effects->objectAtIndex(i));
		CCPoint enemyp = (enemy->getsprite())->getPosition();
		CCActionInterval* action1 = CCFadeIn::actionWithDuration(0.3);
		CCActionInterval* action2 = CCFadeOut::actionWithDuration(0.3);
		for (int j = 0; j < playerBullets->capacity(); j++)
		{
			Bullet* bullet = (Bullet*)(playerBullets->objectAtIndex(j));
			if ((enemy->getsprite())->getIsVisible() && (bullet->getsprite())->getIsVisible() && enemy->collisionWithBullet(bullet))
			{
				enemy->setDead();
				effect->setPosition(enemyp);
				effect->setIsVisible(true);
				effect->runAction(CCSequence::actions(action1, action2, NULL));
				bullet->setPosition(ccp(200, 800));
				(bullet->getsprite())->setIsVisible(false);
				getScore();
			}
			if ((enemy->getsprite())->getIsVisible() && enemy->iscollision(player))
			{
				enemy->setDead();
				effect->setPosition(enemyp);
				effect->setIsVisible(true);
				effect->runAction(CCSequence::actions(action1, action2, NULL));
				playerDead();
				getScore();
			}
		}
	}
	for (int i = 0; i < enemyPlane4->capacity(); i++)
	{
		Enemy* enemy = (Enemy*)(enemyPlane4->objectAtIndex(i));
		CCSprite* effect = (CCSprite*)(plane4Effects->objectAtIndex(i));
		CCPoint enemyp = (enemy->getsprite())->getPosition();
		CCActionInterval* action1 = CCFadeIn::actionWithDuration(0.3);
		CCActionInterval* action2 = CCFadeOut::actionWithDuration(0.3);
		for (int j = 0; j < playerBullets->capacity(); j++)
		{
			Bullet* bullet = (Bullet*)(playerBullets->objectAtIndex(j));
			if ((enemy->getsprite())->getIsVisible() && (bullet->getsprite())->getIsVisible() && enemy->collisionWithBullet(bullet))
			{
				enemy->setDead();
				effect->setPosition(enemyp);
				effect->setIsVisible(true);
				effect->runAction(CCSequence::actions(action1, action2, NULL));
				bullet->setPosition(ccp(200, 800));
				(bullet->getsprite())->setIsVisible(false);
				getScore();
			}
			if ((enemy->getsprite())->getIsVisible() && enemy->iscollision(player))
			{
				enemy->setDead();
				effect->setPosition(enemyp);
				effect->setIsVisible(true);
				effect->runAction(CCSequence::actions(action1, action2, NULL));
				playerDead();
				getScore();
			}
		}
	}

	// 判断玩家与敌人子弹的相撞
	for (int i = 0; i < enemyBullets->capacity(); i++)
	{
		Bullet* bullet = (Bullet*)(enemyBullets->objectAtIndex(i));
		if ((bullet->getsprite())->getIsVisible())
		{
			bullet->tick();
			if (player->iscollision(bullet))
			{
				(bullet->getsprite())->setIsVisible(false);
				playerDead();
			}
		}
	}
}

void GameLayer::updateBullets(ccTime dt)
{
	for (int i = 0; i < playerBullets->capacity(); i++)
	{
		Bullet* bullet = (Bullet*)(playerBullets->objectAtIndex(i));
		if ((bullet->getsprite())->getIsVisible()) // 若子弹可见，判断是否飞出屏幕，是则重置位置
		{
			if ((bullet->getsprite())->getPosition().y  > 600)
			{
				(bullet->getsprite())->setIsVisible(false);
			}
		}
		else // 若子弹不可见，则使其可以见
		{
			CCSize playersize = player->getSize();
			bullet->setPosition(ccp(playerp.x, playerp.y + playersize.height / 1.5));
			SimpleAudioEngine::sharedEngine()->playEffect("pwl1.wav", false);  // 可能导致游戏卡
			(bullet->getsprite())->setIsVisible(true);
			break;
		}
	}

	// 根据得分数量改变子弹效果
	if (score < 10)
		bulletSpeed = 1.2;
	else if (score > 30)
	{
		for (int i = 0; i < playerBullets->capacity(); i++)
		{
			Bullet* bullet = (Bullet*)(playerBullets->objectAtIndex(i));
			(bullet->getsprite())->initWithFile("newnewBullet.png");
		}
		bulletSpeed = 1.1;
	}
	else
	{
		for (int i = 0; i < playerBullets->capacity(); i++)
		{
			Bullet* bullet = (Bullet*)(playerBullets->objectAtIndex(i));
			(bullet->getsprite())->initWithFile("newBullet.png");
		}
		bulletSpeed = 1;
	}
}

// 转换到BOSS场景
void GameLayer::updateBoss(ccTime dt)
{
	unschedule(schedule_selector(GameLayer::updateBoss));
	unschedule(schedule_selector(GameLayer::update));
	joyStick->Inactive();
	SimpleAudioEngine::sharedEngine()->stopBackgroundMusic();

	CCDirector::sharedDirector()->setDepthTest(true);
	CCDirector::sharedDirector()->replaceScene(CCTransitionFadeUp::transitionWithDuration(1.5, BossLayer::scene()));
}

// 虚拟摇杆更新
void GameLayer::updateJS(ccTime dt)
{
	CCPoint displace = joyStick->getDirection();
	CCSize playersize = player->getSize();
	if (outScreen(player))
	{
		if ((playerp.x - playersize.width / 2.5) <= 0)
			player->setPosition(ccp(playerp.x + 1, playerp.y));
		else if ((playerp.x + playersize.width / 2.5) >= 400)
			player->setPosition(ccp(playerp.x - 1, playerp.y));
		else if ((playerp.y - playersize.height / 2) <= 0)
			player->setPosition(ccp(playerp.x, playerp.y + 1));
		else if ((playerp.y + playersize.height / 2) > 600)
			player->setPosition(ccp(playerp.x, playerp.y - 1));
	}
	else
		player->setPosition(ccpAdd(playerp, ccpMult(displace , -2.5)));
}

// 一类型敌机
void GameLayer::updateStep1(ccTime dt)
{
	unschedule(schedule_selector(GameLayer::updateStep1));
	for (int i = 0; i < enemyPlane1->capacity(); i++)
	{
		Enemy* enemy = (Enemy*)(enemyPlane1->objectAtIndex(i));
		(enemy->getsprite())->setIsVisible(true);
		enemy->tick();
	}
}

// 二类型敌机
void GameLayer::updateStep2(ccTime dt)
{
	unschedule(schedule_selector(GameLayer::updateStep2));
	for (int i = 0; i < enemyPlane2->capacity(); i++)
	{
		Enemy* enemy = (Enemy*)(enemyPlane2->objectAtIndex(i));
		(enemy->getsprite())->setIsVisible(true);
		CCActionInterval* action1 = CCDelayTime::actionWithDuration(1.5f);
		CCActionInterval* action2 = CCMoveTo::actionWithDuration(1.2f, ccp(50 + i*  50, 400));
		CCActionInterval* action3 = CCDelayTime::actionWithDuration(3);
		CCActionInterval* action4 = CCMoveBy::actionWithDuration(1.2f, ccp(0, -500));
		(enemy->getsprite())->runAction(CCSequence::actions(action1, action2, action3, action4, NULL));
	}
}

// 二类型敌机子弹
void GameLayer::updateStep3(ccTime dt)
{
	unschedule(schedule_selector(GameLayer::updateStep3));
	for (int i = 0; i < enemyPlane2->capacity(); i++)
	{
		Enemy* enemy = (Enemy*)(enemyPlane2->objectAtIndex(i));
		Bullet* bullet = (Bullet*)(enemyBullets->objectAtIndex(i));
		CCSize enemysize = enemy->getSize();
		CCPoint enemyp = (enemy->getsprite())->getPosition();
		if ((enemy->getsprite())->getIsVisible())
		{
			bullet->setPosition(ccp(enemyp.x, enemyp.y - enemysize.height / 1.5));
			(bullet->getsprite())->setIsVisible(true);
		}
	}
}

// 三类型敌机
void GameLayer::updateStep4(ccTime dt)
{
	unschedule(schedule_selector(GameLayer::updateStep4));
	for (int i = 0; i < enemyPlane3->capacity(); i++)
	{
		Enemy* enemy = (Enemy*)(enemyPlane3->objectAtIndex(i));
		(enemy->getsprite())->setIsVisible(true);
		enemy->tick();
	}
}

// 四类型敌机
void GameLayer::updateStep5(ccTime dt)
{
	unschedule(schedule_selector(GameLayer::updateStep5));
	for (int i = 0; i < enemyPlane4->capacity(); i++)
	{
		Enemy* enemy = (Enemy*)(enemyPlane4->objectAtIndex(i));
		(enemy->getsprite())->setIsVisible(true);
		enemy->tick();
	}
}

// 循环出现、随即敌机类型
void GameLayer::updateStep6(ccTime dt)
{
	unschedule(schedule_selector(GameLayer::updateStep6));

	int enemyNum[4];
	for (int i = 0; i < 4; i++)
		enemyNum[i] = CCRANDOM_0_1()*  6 + 1;

	for (int i = 0; i < enemyNum[0]; i++)
	{
		Enemy* enemy = (Enemy*)(enemyPlane1->objectAtIndex(i));
		(enemy->getsprite())->setIsVisible(true);
		enemy->tick();
	}

	for (int i = 0; i < enemyNum[1]; i++)
	{
		Enemy* enemy = (Enemy*)(enemyPlane2->objectAtIndex(i));
		(enemy->getsprite())->setIsVisible(true);
		enemy->tick();
	}

	for (int i = 0; i < enemyNum[2]; i++)
	{
		Enemy* enemy = (Enemy*)(enemyPlane3->objectAtIndex(i));
		(enemy->getsprite())->setIsVisible(true);
		enemy->tick();
	}

	for (int i = 0; i < enemyNum[3]; i++)
	{
		Enemy* enemy = (Enemy*)(enemyPlane4->objectAtIndex(i));
		(enemy->getsprite())->setIsVisible(true);
		enemy->tick();
	}

	schedule(schedule_selector(GameLayer::updateStep6), 15);
	schedule(schedule_selector(GameLayer::updateStep7), 3);
}

// 循环出现对应子弹
void GameLayer::updateStep7(ccTime dt)
{
	unschedule(schedule_selector(GameLayer::updateStep7));

	for (int i = 0; i < enemyPlane2->capacity(); i++)
	{
		Enemy* enemy = (Enemy*)(enemyPlane2->objectAtIndex(i));
		Bullet* bullet = (Bullet*)(enemyBullets->objectAtIndex(i));
		CCSize enemysize = enemy->getSize();
		CCPoint enemyp = (enemy->getsprite())->getPosition();
		if ((enemy->getsprite())->getIsVisible() && (enemy->getsprite())->getPosition().y > 0 && (enemy->getsprite())->getPosition().y < 600)
		{
			bullet->setPosition(ccp(enemyp.x, enemyp.y - enemysize.height / 1.5));
			(bullet->getsprite())->setIsVisible(true);
		}
	}
}

void GameLayer::updateExplore1(ccTime dt)
{
	unschedule(schedule_selector(GameLayer::updateExplore1));
	allToDead();
}

void GameLayer::updateExplore2(ccTime dt)
{
	unschedule(schedule_selector(GameLayer::updateExplore2));
	allToDead();
}

void GameLayer::updateExplore3(ccTime dt)
{
	unschedule(schedule_selector(GameLayer::updateExplore3));
	allToDead();
}

// 炸弹触摸
void GameLayer::ccTouchesBegan(CCSet* pTouches, CCEvent* pEvent)
{
	CCSetIterator it = pTouches->begin();
	CCTouch* touch = (CCTouch*)(*it);
	CCPoint m_tPos = touch->locationInView();
	m_tPos = CCDirector::sharedDirector()->convertToGL(m_tPos);
	if (boomNum != 0)
	{
		if (m_tPos.x > 300 && m_tPos.x < 400 && m_tPos.y > 0 && m_tPos.y < 100)
		{
			CCSize playersize = player->getSize();
			CCActionInterval* action1 = CCMoveBy::actionWithDuration(1.3, ccp(0, 200));
			CCActionInterval* action2 = CCFadeOut::actionWithDuration(0.2);
			SimpleAudioEngine::sharedEngine()->playEffect("launch.wav", false);
			switch (boomNum)
			{
			case 3:
				boomNum--;
				boom1->setPosition(ccp(playerp.x, playerp.y + playersize.height / 1.5));
				(boom1->getsprite())->runAction(CCSequence::actions(action1, action2, NULL));
				schedule(schedule_selector(GameLayer::updateExplore1), 1.5);
				break;
			case 2:
				boomNum--;
				boom2->setPosition(ccp(playerp.x, playerp.y + playersize.height / 1.5));
				(boom2->getsprite())->runAction(CCSequence::actions(action1, action2, NULL));
				schedule(schedule_selector(GameLayer::updateExplore2), 1.5);
				break;
			case 1:
				boomNum--;
				boom3->setPosition(ccp(playerp.x, playerp.y + playersize.height / 1.5));
				(boom3->getsprite())->runAction(CCSequence::actions(action1, action2, NULL));
				schedule(schedule_selector(GameLayer::updateExplore3), 1.5);
				break;
			}
		}
	}
}