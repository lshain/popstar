#include "GameScene.h"
#include "GameLayer.h"
#include "Audio.h"

bool GameScene::init(){
	if(!Scene::init()){
		return false;
	}
	Audio::getInstance()->playBGM();
	log("GameScene-1");
	this->addChild(GameLayer::create());
	log("GameScene-2");
	return true;
} 
