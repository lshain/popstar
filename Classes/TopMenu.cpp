#include "TopMenu.h"
#include "GameData.h"

bool TopMenu::init(){
	if(!Node::init()){
		return false;
	}
	Size visibleSize = Director::getInstance()->getVisibleSize();
	log("TopMenu-1");
	//Verdana-Bold
	highestScore = Label::create(
			"历史最高分" + cocos2d::String::createWithFormat("%d",GAMEDATA::getInstance()->getHistoryScore())->_string,
		"Arial",30
	);
	highestScore->setPosition(visibleSize.width/2,visibleSize.height - 50);
	this->addChild(highestScore);
	log("TopMenu-2");
	level = Label::create(
		"关卡" + cocos2d::String::createWithFormat("%d",GAMEDATA::getInstance()->getNextLevel())->_string,
		"Arial",30
	);
	level->setPosition(200,visibleSize.height - 100);
	this->addChild(level);
	log("TopMenu-3");
	targetScore = Label::create(
		"目标" + cocos2d::String::createWithFormat("%d",GAMEDATA::getInstance()->getNextScore())->_string + "分",
		"Arial",30
	);
	targetScore->setPosition(400,visibleSize.height - 100);
	this->addChild(targetScore);
	log("TopMenu-4");
	curScore = Label::create(
	    cocos2d::String::createWithFormat("%d",GAMEDATA::getInstance()->getCurScore())->_string,
		"Arial",50
	);
	curScore->setPosition(visibleSize.width/2,visibleSize.height - 150);
	this->addChild(curScore);
	log("TopMenu-5");
	return true;
}

void TopMenu::refresh(){
	char buf[64];
	sprintf(buf,"%d",GAMEDATA::getInstance()->getCurScore());
	curScore->setString(buf);

//	string history = "历史最高分" + cocos2d::String::createWithFormat("%d",GAMEDATA::getInstance()->getHistoryScore())->_string;
	highestScore->setString("历史最高分" + cocos2d::String::createWithFormat("%d",GAMEDATA::getInstance()->getHistoryScore())->_string);

//	string guanqia = "关卡" + cocos2d::String::createWithFormat("%d",GAMEDATA::getInstance()->getNextLevel())->_string;
	level->setString("关卡" + cocos2d::String::createWithFormat("%d",GAMEDATA::getInstance()->getNextLevel())->_string);

//	string target = "目标" + cocos2d::String::createWithFormat("%d",GAMEDATA::getInstance()->getNextScore())->_string + "分";
	targetScore->setString("目标" + cocos2d::String::createWithFormat("%d",GAMEDATA::getInstance()->getNextScore())->_string + "分");
}
