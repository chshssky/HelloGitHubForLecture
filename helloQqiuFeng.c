#include "AppDelegate.h"

#include "cocos2d.h"

#include "Menu.h"

#include "CCEGLView.h"

USING_NS_CC;


bool AppDelegate::initInstance()
{
    bool bRet = false;
    do 
    {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)

		// Initialize OpenGLView instance, that release by CCDirector when application terminate.
		// The HelloWorld is designed as HVGA.
		CCEGLView * pMainWnd = new CCEGLView();
		CC_BREAK_IF(! pMainWnd
				|| ! pMainWnd->Create(TEXT("FirePlane"), 400, 600));

#endif  // CC_PLATFORM_WIN32

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)

        // OpenGLView initialized in testsAppDelegate.mm on ios platform, nothing need to do here.

#endif  // CC_PLATFORM_IOS

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)

		// OpenGLView initialized in HelloWorld/android/jni/helloworld/main.cpp
		// the default setting is to create a fullscreen view
		// if you want to use auto-scale, please enable view->create(320,480) in main.cpp
		// if the resources under '/sdcard" or other writeable path, set it.
		// warning: the audio source should in assets/
		// cocos2d::CCFileUtils::setResourcePath("/sdcard");

#endif  // CC_PLATFORM_ANDROID


#endif

#endif  // CC_PLATFORM_WOPHONE
#if (CC_TARGET_PLATFORM == CC_PLATFORM_MARMALADE)
		// MaxAksenov said it's NOT a very elegant solution. I agree, haha
		CCDirector::sharedDirector()->setDeviceOrientation(kCCDeviceOrientationLandscapeLeft);
#endif
#if (CC_TARGET_PLATFORM == CC_PLATFORM_LINUX)

		// Initialize OpenGLView instance, that release by CCDirector when application terminate.
		// The HelloWorld is designed as HVGA.
		CCEGLView * pMainWnd = new CCEGLView();
		CC_BREAK_IF(! pMainWnd
				|| ! pMainWnd->Create("cocos2d: Hello World", 800, 480,480, 320));

		CCFileUtils::setResourcePath("../Resource/");

#endif  // CC_PLATFORM_LINUX

#if (CC_TARGET_PLATFORM == CC_PLATFORM_BADA)

		CCEGLView * pMainWnd = new CCEGLView();
		CC_BREAK_IF(! pMainWnd|| ! pMainWnd->Create(this, 480, 320));
		pMainWnd->setDeviceOrientation(Osp::Ui::ORIENTATION_LANDSCAPE);
		CCFileUtils::setResourcePath("/Res/");

#endif  // CC_PLATFORM_BADA

#if (CC_TARGET_PLATFORM == CC_PLATFORM_QNX)
		CCEGLView * pMainWnd = new CCEGLView();
		CC_BREAK_IF(! pMainWnd|| ! pMainWnd->Create(480, 320));
		CCFileUtils::setResourcePath("./app/native/Resource");
#endif // CC_PLATFORM_QNX
		bRet = true;
	} while (0);
	return bRet;
}

bool AppDelegate::applicationDidFinishLaunching()
{
    // initialize director
    CCDirector *pDirector = CCDirector::sharedDirector();
    pDirector->setOpenGLView(&CCEGLView::sharedOpenGLView());

	// enable High Resource Mode(2x, such as iphone4) and maintains low resource on other devices.
//     pDirector->enableRetinaDisplay(true);

    // turn on display FPS
    pDirector->setDisplayFPS(false);

    // set FPS. the default value is 1.0/60 if you don't call this
    pDirector->setAnimationInterval(1.0 / 60);

    // create a scene. it's an autorelease object
    CCScene *pScene = Menu::scene();

    // run
    pDirector->runWithScene(pScene);
    return true;
}

// This function will be called when the app is inactive. When comes a phone call,it's be invoked too
void AppDelegate::applicationDidEnterBackground()
{
    CCDirector::sharedDirector()->pause();
}

// this function will be called when the app is active again
void AppDelegate::applicationWillEnterForeground()
{
    CCDirector::sharedDirector()->resume();
}
#include <stdio.h>

String main(int argc, char** argv)
{

	printf("Hello World!\n");
	return "班长碉堡了";
}
