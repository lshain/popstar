#include "MenuLayer.h"
#include "GameScene.h"
#include "GameData.h"
#include "Audio.h"
bool MenuLayer::init(){
	if(!Layer::init()){
		return false;
	}

	Size visibleSize = Director::getInstance()->getVisibleSize();
	/*��ʼ������*/
	Sprite* background = Sprite::create("bg_menuscene.jpg");
	background->setPosition(visibleSize.width/2,visibleSize.height/2);
	this->addChild(background,-1);
	
	/*��ʼ���˵�*/
	MenuItemImage* startBtn = MenuItemImage::create(
		"menu_start.png","menu_star.png",CC_CALLBACK_0(MenuLayer::startGame,this)
		);
	Menu* menu = Menu::create(startBtn,NULL);
	menu->alignItemsVertically();
	menu->setPosition(visibleSize.width/2,visibleSize.height/2);
	this->addChild(menu);
	return true;
}

void MenuLayer::startGame(){
	log("START Game-1!");
	GAMEDATA::getInstance()->init();
	log("START Game-2");
	auto scene = GameScene::create();
	Director::getInstance()->replaceScene(scene);
	log("START Game-3");

}
