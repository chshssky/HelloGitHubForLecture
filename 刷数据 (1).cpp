#include "Menu.h"
#include "BossLayer.h"
#include "GameOver.h"
#include "GameWin.h"

#include "SimpleAudioEngine.h"

using namespace CocosDenshion;

BossLayer::BossLayer()
{
}

BossLayer::~BossLayer()
{
}

bool BossLayer::init()
{
	bool bRet = false;
	do
	{
		CC_BREAK_IF(! CCLayer::init());

		SimpleAudioEngine::sharedEngine()->playBackgroundMusic("bgm2.wav", true); // 背景音乐

		player = new Player("plane.png");
		CC_BREAK_IF(! player);
		player->setPosition(ccp(200, 80));
		this->addChild(player->getsprite(), 2);

		boss = new Boss;
		deadBoss = new Boss;
		(deadBoss->getsprite())->initWithFile("Boss.png");
		CC_BREAK_IF(! boss);
		CC_BREAK_IF(! deadBoss);
		boss->setPosition(ccp(200, 500));
		deadBoss->setPosition(ccp(200, 800));
		(boss->getsprite())->setIsVisible(false);
		(deadBoss->getsprite())->setIsVisible(false);
		this->addChild(boss->getsprite(), 2);
		this->addChild(deadBoss->getsprite(), 2);

		// 加入背景
		m_bg1 = CCSprite::spriteWithFile("bossBg1.png");
		m_bg2 = CCSprite::spriteWithFile("bossBg2.png");
		CC_BREAK_IF(! m_bg1);
		CC_BREAK_IF(! m_bg2);
		CCSize size = CCDirector::sharedDirector()->getWinSize(); // 获取OpenGL View的大小，单位为点
		m_bg1->setAnchorPoint(CCPointZero);
		m_bg2->setAnchorPoint(CCPointZero);
		m_bg1->setPosition(ccp(0, 0));
		m_bg1->setPosition(ccp(0, 600));
		this->addChild(m_bg1, 0, 1);
		this->addChild(m_bg2, 0, 2);

		// 加入生命条
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
		// 初始化玩家生命、BOSS生命、BOSS状态
		playerLife = 3;
		bossLife = LIFE;
		bossLevel = 0;

		// 初始化三颗炸弹
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
		boomNum = 3;

		// 初始化炸弹爆炸粒子特效
		m_explore1 = new CCParticleSystemQuad();
		m_explore2 = new CCParticleSystemQuad();
		m_explore3 = new CCParticleSystemQuad();
		m_explore1->setIsVisible(false);
		m_explore2->setIsVisible(false);
		m_explore3->setIsVisible(false);
		this->addChild(m_explore1, 10);
		this->addChild(m_explore2, 11);
		this->addChild(m_explore3, 12);

		// 背景星系特效
		m_galaxy = new CCParticleSystemQuad();
		m_galaxy->setIsVisible(false);
		this->addChild(m_galaxy, 1);

		setIsTouchEnabled(true); // 开启触摸
		int stepindex = -1;
		scheduleUpdate(); // update()更新

		playerBullets = CCArray::arrayWithCapacity(10); // 创建玩家子弹，自动发射
		bossBullets = CCArray::arrayWithCapacity(10); // 创建Boss子弹，自动发射
		for (int i = 0; i < playerBullets->capacity(); i++)
		{
			Bullet* bullet = new Bullet("bullet.png");
			bullet->setType(0); // 设置子弹种类
			(bullet->getsprite())->setIsVisible(false);
			this->addChild(bullet->getsprite(), 200 + i);
			playerBullets->addObject(bullet);
		}
		for (int i = 0; i < bossBullets->capacity(); i++)
		{
			Bullet* bullet = new Bullet("BossBullet.png");
			bullet->setType(1); // 设置子弹种类
			(bullet->getsprite())->setIsVisible(false);
			this->addChild(bullet->getsprite(), 250 + i);
			bossBullets->addObject(bullet);
		}
		playerBullets->retain();
		bossBullets->retain();

		bossEffects = CCArray::arrayWithCapacity(20); // 死亡时的爆炸效果
		for (int i = 0; i < bossEffects->capacity(); i++)
		{
			CCSprite* effect = new CCSprite;
			effect = CCSprite::spriteWithFile("bossExplored.png");
			effect->setPosition(ccp(200, 800));
			effect->setIsVisible(false);
			this->addChild(effect, 100 + i);
			bossEffects->addObject(effect);
		}
		bossEffects->retain();

		playerEffect = new CCSprite;
		playerEffect = CCSprite::spriteWithFile("explored.png");
		CC_BREAK_IF(! playerEffect);
		playerEffect->setIsVisible(false);
		this->addChild(playerEffect, 99);

		// 加入虚拟摇杆
		CCSprite* jsPoint = CCSprite::spriteWithFile("point.png");
		CCSprite* jsBG = CCSprite::spriteWithFile("joystickbg.png");
		CC_BREAK_IF(! jsPoint);
		CC_BREAK_IF(! jsBG);
		joyStick = new JoyStick;
		joyStick = JoyStick::JoyStickWithCenter(ccp(70, 70), 48, jsPoint, jsBG);
		CC_BREAK_IF(! joyStick);
		this->addChild(joyStick, 1);
		joyStick->Active(); // 虚拟摇杆开启
		js = false;
		bulletSpeed = 1.3;
		rotateSpeed = 2;

		this->schedule(schedule_selector(BossLayer::updateBullets), 1.2); // 控制玩家子弹发射间隔
		this->schedule(schedule_selector(BossLayer::updateBossBullets), bulletSpeed); // 控制BOSS子弹发射间隔
		this->schedule(schedule_selector(BossLayer::updateFirst), 5);
		this->schedule(schedule_selector(BossLayer::updateFadeOut), 10);
		this->schedule(schedule_selector(BossLayer::updateRotate), rotateSpeed);

		this->schedule(schedule_selector(BossLayer::updateJS)); // 摇杆刷新

		bRet = true;
	} while (0);

	return bRet;
}

CCScene* BossLayer::scene()
{
	CCScene* scene = NULL;
	do
	{
		scene = CCScene::node();
		CC_BREAK_IF(! scene);

		BossLayer* layer = BossLayer::node();
		CC_BREAK_IF(! layer);

		scene->addChild(layer);
	} while (0);

	return scene;
}

// 判断精灵是否超出屏幕
bool BossLayer::outScreen(BaseSprite* sp)
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

// 玩家死亡逻辑
void BossLayer::playerDead()
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

	if (playerLife == 0) // 若玩家生命值为0，游戏失败
	{
		(player->getsprite())->stopAllActions();
		(player->getsprite())->setIsVisible(false);
		joyStick->Inactive(); // 停止摇杆触控
		SimpleAudioEngine::sharedEngine()->stopBackgroundMusic();

		// 转至游戏失败界面
		CCDirector::sharedDirector()->setDepthTest(true);
		CCDirector::sharedDirector()->replaceScene(CCTransitionFadeUp::transitionWithDuration(1.2, GameOver::scene()));
	}
}

// 爆炸效果
void BossLayer::boom()
{
	CCSize playersize = player->getSize();
	m_explore1->initWithFile("ExplodingRing.plist");
	m_explore1->setPosition(ccp(playerp.x, (playerp.y + playersize.height / 1.5) + 200));
	m_explore1->setIsVisible(true);
	SimpleAudioEngine::sharedEngine()->playEffect("boom.wav", false);
	for (int i = 0; i < bossBullets->capacity(); i++) // 使BOSS子弹消失
	{
		Bullet* bullet = (Bullet*)(bossBullets->objectAtIndex(i));
		if ((bullet->getsprite())->getIsVisible())
			(bullet->getsprite())->setIsVisible(false);
	}

	// BOSS被炸特效
	if ((boss->getsprite())->getIsVisible())
	{
		for (int i = 0; i < 5; i++)
		{
			CCActionInterval* action1 = CCFadeIn::actionWithDuration(0.3);
			CCActionInterval* action2 = CCFadeOut::actionWithDuration(0.3);
			CCSprite* effect = (CCSprite*)(bossEffects->objectAtIndex(i));
			CCSize bosssize = boss->getSize();
			effect->setPosition(ccp(bossp.x + CCRANDOM_MINUS1_1() * (bosssize.width / 2), bossp.y + CCRANDOM_MINUS1_1()* (bosssize.height / 2)));
			effect->setIsVisible(true);
			effect->runAction(CCSequence::actions(action1, action2, NULL));
		}
		bossLife -= 3;
	}

	if (bossLife > 2 * LIFE / 3)
		bossLevel = 0;
	else if (bossLife < LIFE / 3)
	{
		CCActionInterval* action = CCTintTo::actionWithDuration(1.5, 100, 75, 50);
		(boss->getsprite())->runAction(action);
		bossLevel = 2;
		bulletSpeed = 0.9;
		rotateSpeed = 0.6;
		for (int i = 0; i < playerBullets->capacity(); i++)
		{
			Bullet* bullet = (Bullet*)(playerBullets->objectAtIndex(i));
			(bullet->getsprite())->initWithFile("newnewBullet.png");
		}
		for (int i = 0; i < bossBullets->capacity(); i++)
		{
			Bullet* bullet = (Bullet*)(bossBullets->objectAtIndex(i));
			(bullet->getsprite())->initWithFile("newnewBossBullet.png");
		}
	}
	else
	{
		CCActionInterval* action = CCTintTo::actionWithDuration(1.5, 200, 150, 100);
		(boss->getsprite())->runAction(action);
		bossLevel = 1;
		bulletSpeed = 1.1;
		rotateSpeed = 1.3;
		for (int i = 0; i < playerBullets->capacity(); i++)
		{
			Bullet* bullet = (Bullet*)(playerBullets->objectAtIndex(i));
			if (!((bullet->getsprite())->getIsVisible()))
				(bullet->getsprite())->initWithFile("newBullet.png");
		}
		for (int i = 0; i < bossBullets->capacity(); i++)
		{
			Bullet* bullet = (Bullet*)(bossBullets->objectAtIndex(i));
			if (!((bullet->getsprite())->getIsVisible()))
				(bullet->getsprite())->initWithFile("newBossBullet.png");
		}
	}

	// 判断BOSS是否死亡
	if (bossLife <= 0)
	{
		// 停止所有更新，实现BOSS死亡特效
		unschedule(schedule_selector(BossLayer::updateBossBullets));
		unschedule(schedule_selector(BossLayer::updateFadeOut));
		unschedule(schedule_selector(BossLayer::updateFadeIn));
		CCActionInterval* fadeOut = CCFadeOut::actionWithDuration(4.5);
		deadBoss->setPosition(bossp);
		(deadBoss->getsprite())->setIsVisible(true);
		(deadBoss->getsprite())->runAction(fadeOut);
		boss->setDead();
		for (int i = 0; i < bossBullets->capacity(); i++)
		{
			Bullet* bullet = (Bullet*)(bossBullets->objectAtIndex(i));
			(bullet->getsprite())->setIsVisible(false);
		}
		SimpleAudioEngine::sharedEngine()->playEffect("bossdead.wav", false);
		for (int i = 0; i < bossEffects->capacity(); i++)
		{
			CCActionInterval* delay = CCDelayTime::actionWithDuration(CCRANDOM_0_1() * 5);
			CCActionInterval* action1 = CCFadeIn::actionWithDuration(0.3);
			CCActionInterval* action2 = CCFadeOut::actionWithDuration(0.3);
			CCSprite* effect = (CCSprite*)(bossEffects->objectAtIndex(i));
			CCSize bosssize = boss->getSize();
			effect->setPosition(ccp(bossp.x + CCRANDOM_MINUS1_1() * (bosssize.width / 2), bossp.y + CCRANDOM_MINUS1_1()* (bosssize.height / 2)));
			effect->setIsVisible(true);
			effect->runAction(CCSequence::actions(delay, action1, action2, NULL));
		}

		schedule(schedule_selector(BossLayer::updateWin), 7); // 7秒后转至胜利界面
	}
}

void BossLayer::aimAt(CCPoint boomp)
{
	CCActionInterval* action1 = CCFadeIn::actionWithDuration(0.3);
	CCActionInterval* action2 = CCFadeOut::actionWithDuration(0.3);
	CCSprite* effect = (CCSprite*)(bossEffects->objectAtIndex(0));
	effect->setPosition(boomp);
	effect->setIsVisible(true);
	effect->runAction(CCSequence::actions(action1, action2, NULL));
	bossLife--;

	if (bossLife > 2 * LIFE / 3)
		bossLevel = 0;
	else if (bossLife < LIFE / 3)
	{
		CCActionInterval* action = CCTintTo::actionWithDuration(1.5, 100, 75, 50);
		(boss->getsprite())->runAction(action);
		bossLevel = 2;
		bulletSpeed = 0.9;
		rotateSpeed = 0.6;
		for (int i = 0; i < playerBullets->capacity(); i++)
		{
			Bullet* bullet = (Bullet*)(playerBullets->objectAtIndex(i));
			(bullet->getsprite())->initWithFile("newnewBullet.png");
		}
		for (int i = 0; i < bossBullets->capacity(); i++)
		{
			Bullet* bullet = (Bullet*)(bossBullets->objectAtIndex(i));
			(bullet->getsprite())->initWithFile("newnewBossBullet.png");
		}
	}
	else
	{
		CCActionInterval* action = CCTintTo::actionWithDuration(1.5, 200, 150, 100);
		(boss->getsprite())->runAction(action);
		bossLevel = 1;
		bulletSpeed = 1.1;
		rotateSpeed = 1.3;
		for (int i = 0; i < playerBullets->capacity(); i++)
		{
			Bullet* bullet = (Bullet*)(playerBullets->objectAtIndex(i));
			if (!((bullet->getsprite())->getIsVisible()))
				(bullet->getsprite())->initWithFile("newBullet.png");
		}
		for (int i = 0; i < bossBullets->capacity(); i++)
		{
			Bullet* bullet = (Bullet*)(bossBullets->objectAtIndex(i));
			if (!((bullet->getsprite())->getIsVisible()))
				(bullet->getsprite())->initWithFile("newBossBullet.png");
		}
	}

	// BOSS被命中则判断是否死亡
	if (bossLife <= 0)
	{
		unschedule(schedule_selector(BossLayer::updateBossBullets));
		unschedule(schedule_selector(BossLayer::updateFadeOut));
		unschedule(schedule_selector(BossLayer::updateFadeIn));
		CCActionInterval* fadeOut = CCFadeOut::actionWithDuration(4.5);
		deadBoss->setPosition(bossp);
		(deadBoss->getsprite())->setIsVisible(true);
		(deadBoss->getsprite())->runAction(fadeOut);
		boss->setDead();
		for (int i = 0; i < bossBullets->capacity(); i++)
		{
			Bullet* bullet = (Bullet*)(bossBullets->objectAtIndex(i));
			(bullet->getsprite())->setIsVisible(false);
		}
		SimpleAudioEngine::sharedEngine()->playEffect("bossdead.wav", false);
		for (int i = 0; i < bossEffects->capacity(); i++)
		{
			CCActionInterval* delay = CCDelayTime::actionWithDuration(CCRANDOM_0_1() * 5);
			CCActionInterval* action1 = CCFadeIn::actionWithDuration(0.3);
			CCActionInterval* action2 = CCFadeOut::actionWithDuration(0.3);
			CCSprite* effect = (CCSprite*)(bossEffects->objectAtIndex(i));
			CCSize bosssize = boss->getSize();
			effect->setPosition(ccp(bossp.x + CCRANDOM_MINUS1_1() * (bosssize.width / 2), bossp.y + CCRANDOM_MINUS1_1()* (bosssize.height / 2)));
			effect->setIsVisible(true);
			effect->runAction(CCSequence::actions(delay, action1, action2, NULL));
		}
		schedule(schedule_selector(BossLayer::updateWin), 7);
	}
}

void BossLayer::update(ccTime dt)
{
	// 实现背景移动
	CCPoint m_bg1p = m_bg1->getPosition();
	CCPoint m_bg2p = m_bg2->getPosition();
	m_bg1->setPosition(ccp(m_bg1p.x, m_bg1p.y - 4));
	m_bg2->setPosition(ccp(m_bg2p.x, m_bg2p.y - 4));
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

	// 时时刻刻获得玩家与BOSS坐标
	playerp = (player->getsprite())->getPosition();
	bossp = (boss->getsprite())->getPosition();

	// 玩家子弹逻辑
	for (int i = 0; i < playerBullets->capacity(); i++)
	{
		Bullet* bullet = (Bullet*)(playerBullets->objectAtIndex(i));
		if ((bullet->getsprite())->getIsVisible())
		{
			bullet->tick();
			bullet->tick();
		}
	}
	// BOSS子弹逻辑
	for (int i = 0; i < bossBullets->capacity(); i++)
	{
		Bullet* bullet = (Bullet*)(bossBullets->objectAtIndex(i));
		if ((bullet->getsprite())->getIsVisible())
		{
			bullet->tick();
			bullet->tick();
		}
	}

	// 玩家命中BOSS
	for (int i = 0; i < playerBullets->capacity(); i++)
	{
		Bullet* bullet = (Bullet*)(playerBullets->objectAtIndex(i));
		if ((boss->getsprite())->getIsVisible() && ((bullet->getsprite())->getIsVisible()) && boss->collisionWithBullet(bullet))
		{
			aimAt((bullet->getsprite())->getPosition());
			bullet->setPosition(ccp(200, 800));
			(bullet->getsprite())->setIsVisible(false);
		}
	}
	// BOSS命中玩家
	for (int i = 0; i < bossBullets->capacity(); i++)
	{
		Bullet* bullet = (Bullet*)(bossBullets->objectAtIndex(i));
		if ((bullet->getsprite())->getIsVisible() && player->iscollision(bullet))
		{
			CCActionInterval* action1 = CCFadeIn::actionWithDuration(0.3);
			CCActionInterval* action2 = CCFadeOut::actionWithDuration(0.3);
			playerEffect->setPosition(playerp);
			playerEffect->setIsVisible(true);
			playerEffect->runAction(CCSequence::actions(action1, action2, NULL));
			playerDead();
			bullet->setPosition(ccp(200, 800));
			(bullet->getsprite())->setIsVisible(false);
		}
	}

	// BOSS与玩家碰撞，游戏结束
	if ((boss->getsprite())->getIsVisible() && boss->iscollision(player))
	{
		player->setPosition(ccp(200, 200));
		aimAt(playerp);
		playerDead();
	}

	// 星云效果更新
	if (!m_galaxy->getIsVisible())
	{
		m_galaxy->initWithFile("galaxy.plist");
		m_galaxy->setPosition(ccp(CCRANDOM_0_1() * 300 + 50, 600));
		m_galaxy->setIsVisible(true);
	}
	else
	{
		if (m_galaxy->getPosition().y < -800)
			m_galaxy->setIsVisible(false);
		else
			m_galaxy->setPosition(ccp(m_galaxy->getPosition().x + CCRANDOM_MINUS1_1() * 6, m_galaxy->getPosition().y - 4));
	}
}

void BossLayer::updateBullets(ccTime dt)
{
	for (int i = 0; i < playerBullets->capacity(); i++)
	{
		Bullet* bullet = (Bullet*)(playerBullets->objectAtIndex(i));
		if ((bullet->getsprite())->getIsVisible()) // 若子弹可见，判断是否飞出屏幕，是则重置位置
		{
			if ((bullet->getsprite())->getPosition().y  > 600)
				(bullet->getsprite())->setIsVisible(false);
		}
		else // 若子弹不可见，则使其可以见
		{
			CCSize playersize = player->getSize();
			bullet->setPosition(ccp(playerp.x, playerp.y + playersize.height / 1.5));
			SimpleAudioEngine::sharedEngine()->playEffect("pwl1.wav", false);   // 可能导致游戏卡
			(bullet->getsprite())->setIsVisible(true);
			break;
		}
	}
}

void BossLayer::updateBossBullets(ccTime dt)
{
	for (int i = 0; i < bossBullets->capacity(); i++)
	{
		if ((boss->getsprite())->getIsVisible())
		{
			Bullet* bullet = (Bullet*)(bossBullets->objectAtIndex(i));
			if ((bullet->getsprite())->getIsVisible()) // 若子弹可见，判断是否飞出屏幕，是则重置位置
			{
				if ((bullet->getsprite())->getPosition().y  < 0)
					(bullet->getsprite())->setIsVisible(false);
			}
			else // 若子弹不可见，则使其可以见
			{
				CCSize bosssize = boss->getSize();
				bullet->setPosition(ccp(bossp.x, bossp.y - bosssize.height / 1.5));
				(bullet->getsprite())->setIsVisible(true);
				break;
			}
		}
	}
}

// 摇杆实时更新
void BossLayer::updateJS(ccTime dt)
{
	CCPoint displace = joyStick->getDirection();
	CCSize playersize = player->getSize();
	if (outScreen(player)) // 玩家不得超出屏幕
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

// BOSS第一次出现
void BossLayer::updateFirst(ccTime dt)
{
	unschedule(schedule_selector(BossLayer::updateFirst));
	CCActionInterval* action = CCFadeIn::actionWithDuration(1);
	(boss->getsprite())->runAction(action);
	(boss->getsprite())->setIsVisible(true);
	schedule(schedule_selector(BossLayer::updateLive), 2);
}

// BOSS存在，执行逻辑
void BossLayer::updateLive(ccTime dt)
{
	unschedule(schedule_selector(BossLayer::updateLive));
	boss->tick(bossLevel);
}

// BOSS消失更新
void BossLayer::updateFadeOut(ccTime dt)
{
	CCActionInterval* action = CCFadeOut::actionWithDuration(1);
	if ((boss->getsprite())->getIsVisible())
		(boss->getsprite())->runAction(action);
	schedule(schedule_selector(BossLayer::updateMove), 1);
}

// BOSS出现更新
void BossLayer::updateFadeIn(ccTime dt)
{
	unschedule(schedule_selector(BossLayer::updateFadeIn));
	CCActionInterval* action = CCFadeIn::actionWithDuration(1);
	(boss->getsprite())->setIsVisible(true);
	(boss->getsprite())->runAction(action);
	boss->tick(bossLevel);
}

void BossLayer::updateRotate(ccTime dt)
{
	unschedule(schedule_selector(BossLayer::updateRotate));

	CCActionInterval* action = CCRotateBy::actionWithDuration(rotateSpeed, 360);
	(boss->getsprite())->runAction(action);

	schedule(schedule_selector(BossLayer::updateRotate), rotateSpeed);
}

// BOSS消失时位置更新
void BossLayer::updateMove(ccTime dt)
{
	unschedule(schedule_selector(BossLayer::updateMove));
	(boss->getsprite())->setIsVisible(false);
	boss->setPosition(ccp(CCRANDOM_0_1() * 300 + 50, CCRANDOM_0_1() * 300 + 200));
	schedule(schedule_selector(BossLayer::updateFadeIn), 1);
}

// 炸弹更新
void BossLayer::updateExplore1(ccTime dt)
{
	unschedule(schedule_selector(BossLayer::updateExplore1));
	boom();
}

void BossLayer::updateExplore2(ccTime dt)
{
	unschedule(schedule_selector(BossLayer::updateExplore2));
	boom();
}

void BossLayer::updateExplore3(ccTime dt)
{
	unschedule(schedule_selector(BossLayer::updateExplore3));
	boom();
}

// 游戏胜利
void BossLayer::updateWin(ccTime dt)
{
	SimpleAudioEngine::sharedEngine()->stopBackgroundMusic();

	unschedule(schedule_selector(BossLayer::updateWin));
	CCDirector::sharedDirector()->setDepthTest(true);
	CCDirector::sharedDirector()->replaceScene(CCTransitionFadeUp::transitionWithDuration(1.2, GameWin::scene()));
}

// 炸弹触摸
void BossLayer::ccTouchesBegan(CCSet* pTouches, CCEvent* pEvent)
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
				schedule(schedule_selector(BossLayer::updateExplore1), 1.5);
				break;
			case 2:
				boomNum--;
				boom2->setPosition(ccp(playerp.x, playerp.y + playersize.height / 1.5));
				(boom2->getsprite())->runAction(CCSequence::actions(action1, action2, NULL));
				schedule(schedule_selector(BossLayer::updateExplore2), 1.5);
				break;
			case 1:
				boomNum--;
				boom3->setPosition(ccp(playerp.x, playerp.y + playersize.height / 1.5));
				(boom3->getsprite())->runAction(CCSequence::actions(action1, action2, NULL));
				schedule(schedule_selector(BossLayer::updateExplore3), 1.5);
				break;
			}
		}
	}
}