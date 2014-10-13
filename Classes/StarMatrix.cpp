#include "StarMatrix.h"
#include "GameData.h"
#include "StarParticle.h"
#include "ComboEffect.h"
#include "Audio.h"
#include <ctime>

float StarMatrix::ONE_CLEAR_TIME = 0.05f;

StarMatrix* StarMatrix::create(GameLayer* layer){
	StarMatrix* ret = new StarMatrix();
	if(ret && ret->init(layer)){
		ret->autorelease();
		return ret;
	}
	CC_SAFE_DELETE(ret);
	return nullptr;
}

bool StarMatrix::init(GameLayer* layer){
	if(!Node::init()){
		return false;
	}
	m_layer = layer;
	needClear = false;
	clearSumTime = 0;
	//为stars数组赋值为0
	memset(stars, 0, sizeof(Star*) * ROW_NUM * COL_NUM);
	initMatrix();
	return true;
}

void StarMatrix::updateStar(float delta){
	//更新位置，每帧中来更新位置
	for(int i = 0;i < ROW_NUM;i++){
		for(int j = 0;j< COL_NUM;j++){
			if(stars[i][j]!=nullptr){
				stars[i][j]->updatePosition();
			}
		}
	}
	//看是否需要消除
	if(needClear){
		clearSumTime += delta;
		if(clearSumTime > ONE_CLEAR_TIME){
			clearMatrixOneByOne();
			clearSumTime = 0;
		}
	}
	
}

void StarMatrix::onTouch(const Point& p){
	Star* s = getStarByTouch(p);
	if(s){
		genSelectedList(s);
		log("SIZE = %d",selectedList.size());
		deleteSelectedList();
	}
}

void StarMatrix::setNeedClear(bool b){
	needClear = b;
}
//初始化矩阵，并设置位置
void StarMatrix::initMatrix(){
	srand(time(0));
	for(int i=0;i<ROW_NUM;i++){//行
		for(int j=0;j<COL_NUM;j++){//列
			int color = abs(rand()%Star::COLOR_MAX_NUM);
			if(color < 0)
				log("color i=%d,j=%d");
			Star* star = Star::create(color);
			stars[i][j] = star;
			star->setPosition(getPositionByIndex(i,j) + Point(0,100));
			star->setDesPosition(getPositionByIndex(i,j));
			star->setIndex_ij(i,j);
			this->addChild(star);
		}
	}
}

Point StarMatrix::getPositionByIndex(int i,int j){
	float x = j * Star::STAR_WIDTH + Star::STAR_WIDTH/2;//位置从左到右
	float y = (StarMatrix::COL_NUM - i)*Star::STAR_HEIGHT - Star::STAR_HEIGHT/2;//高度从上到下
	return Point(x,y);
}

Star* StarMatrix::getStarByTouch(const Point& p){
	int k = p.y/Star::STAR_HEIGHT;//����Ҫ��Kתһ��int ��Ȼ�ò�����ȷ���
	int i = ROW_NUM - 1 - k;
	int j = p.x/Star::STAR_WIDTH;
	if(i >= 0 && i < ROW_NUM && 
	   j >= 0 && j < COL_NUM &&
	   stars[i][j] != nullptr){
		log("i=%d,j=%d",i,j);
		return stars[i][j];
	}else{
		return nullptr;
	}
}
/**
 * 获取点击处的星星以及其周围的星星是否是相同的颜色，是否要被消除，
 * 并将所有要添加的星星添加到selectList中去
 */
void StarMatrix::genSelectedList(Star* s){
	selectedList.clear();
	deque<Star*> travelList;
	travelList.push_back(s);
	deque<Star*>::iterator it;
	for(it= travelList.begin();it != travelList.end();){
		Star* star = *it;
		Star* linkStar = nullptr;
		int index_i = star->getIndexI();
		int index_j = star->getIndexJ();
		//left link
		if(index_i-1 >= 0 && (linkStar = stars[index_i-1][index_j]) ){
			//上面相邻的没有没选中，且颜色和点击的星星相同，则将相邻的添加到遍历队列中
			if(!linkStar->isSelected() && linkStar->getColor() == star->getColor())
				travelList.push_back(stars[index_i-1][index_j]);
		}
		//right
		if(index_i+1 < ROW_NUM  && (linkStar = stars[index_i+1][index_j]) ){
			if(!linkStar->isSelected() && linkStar->getColor() == star->getColor())
				travelList.push_back(stars[index_i+1][index_j]);
		}
		//up
		if(index_j-1 >= 0 && (linkStar = stars[index_i][index_j-1]) ){
			if(!linkStar->isSelected() && linkStar->getColor() == star->getColor())
				travelList.push_back(stars[index_i][index_j-1]);
		}
		//down
		if(index_j+1 < COL_NUM && (linkStar = stars[index_i][index_j+1]) ){
			if(!linkStar->isSelected() && linkStar->getColor() == star->getColor())
				travelList.push_back(stars[index_i][index_j+1]);
		}
		//点击处的星星没有被选中，或者其相邻的星星没有被选中，则添加到选中的list中去
		if(!star->isSelected()){
			star->setSelected(true);
			selectedList.push_back(star);
		}
		travelList.pop_front();//删除队列头的星星
		it = travelList.begin();//遍历相邻的星星是否需要添加到选中的list中去
	}
}

/**
 * 将在selectList中的星星进行消除
 */
void StarMatrix::deleteSelectedList(){
	if(selectedList.size() <= 1){
		m_layer->hideLinkNum();
		selectedList.at(0)->setSelected(false);
		return;
	}

	for(auto it = selectedList.begin();it != selectedList.end();it++){
		Star* star = *it;
		//粒子特效
		showStarParticleEffect(star->getColor(),star->getPosition(),this);
		stars[star->getIndexI()][star->getIndexJ()] = nullptr;
		star->removeFromParentAndCleanup(true);
		Audio::getInstance()->playPop();
	}
	//combo特效
	showComboEffect(selectedList.size(),this);
	Audio::getInstance()->playCombo(selectedList.size());

	refreshScore();
	m_layer->showLinkNum(selectedList.size());
	//删除星星后需要修改矩阵中的星星位置
	adjustMatrix();
	//判断游戏是否结束
	if(isEnded()){
		m_layer->floatLeftStarMsg(getLeftStarNum());
		CCLOG("ENDED");
	}

}
/**
 * 修正星星矩阵
 */
void StarMatrix::adjustMatrix(){
	//up
	for(int i = ROW_NUM-1;i>=0;i--){
		for(int j = COL_NUM-1;j>=0;j--){
			if(stars[i][j] == nullptr){
				int up = i;
				int dis = 0;
				while(stars[up][j] == nullptr){
					dis++;
					up--;
					if(up<0){
						break;
					}
				}
				
				for(int begin_i = i - dis;begin_i >= 0;begin_i--){
					if(stars[begin_i][j] == nullptr)
						continue;
					Star* s = stars[begin_i + dis][j] = stars[begin_i][j];
					s->setIndex_ij(begin_i + dis,j);
					s->setDesPosition(getPositionByIndex(begin_i + dis,j));
					stars[begin_i][j] = nullptr;
				}
			}else{
				continue;
			}
		}
	}
	//down
	for(int j = 0;j < COL_NUM;j++){
		if(stars[ROW_NUM-1][j] == nullptr){
			int des = 0;
			int right = j;
			while(stars[ROW_NUM-1][right] == nullptr){
				des++;
				right++;
			}
			for(int begin_i = 0;begin_i<ROW_NUM;begin_i++){
				for(int begin_j = j + des;begin_j < COL_NUM;begin_j++){
					if(stars[begin_i][begin_j] == nullptr)
						continue;
					Star* s = stars[begin_i][begin_j - des] = stars[begin_i][begin_j];
					s->setIndex_ij(begin_i,begin_j - des);
					s->setDesPosition(getPositionByIndex(begin_i,begin_j - des));
					stars[begin_i][begin_j] = nullptr;
				}
			}
		}
	}

}


void StarMatrix::refreshScore(){
	GAMEDATA* data = GAMEDATA::getInstance();
	data->setCurScore(data->getCurScore() + selectedList.size()*selectedList.size()*5);
	if(data->getCurScore() > data->getHistoryScore()){
		data->setHistoryScore(data->getCurScore());
	}
	m_layer->refreshMenu();
}

/**
 * 判断游戏是否结束
 */
bool StarMatrix::isEnded(){
	bool bRet = true;
	/**
	 * 遍历所有的剩余的星星是否还有可以再消除的，
	 * 如果有的话则不结束
	 * 否则游戏结束
	 */
	for(int i=0;i<ROW_NUM;i++){
		for(int j=0;j<COL_NUM;j++){
			if(stars[i][j] == nullptr)
				continue;
			int curColor = stars[i][j]->getColor();
			//up
			if(i-1>=0 && stars[i-1][j]!=nullptr && stars[i-1][j]->getColor() ==  curColor)
				return false;
			//down
			if(i+1<ROW_NUM && stars[i+1][j]!=nullptr && stars[i+1][j]->getColor() == curColor)
				return false;
			//left
			if(j-1>=0 && stars[i][j-1]!=nullptr && stars[i][j-1]->getColor() == curColor)
				return false;
			//right
			if(j+1<COL_NUM && stars[i][j+1]!=nullptr && stars[i][j+1]->getColor() == curColor)
				return false;
		}
	}
	return bRet;
}

/**
 * 将剩余的星星逐个消除
 */
void StarMatrix::clearMatrixOneByOne(){
	for(int i=0;i<ROW_NUM;i++){
		for(int j=0;j<COL_NUM;j++){
			if(stars[i][j] == nullptr)
				continue;
			showStarParticleEffect(stars[i][j]->getColor(),stars[i][j]->getPosition(),this);
			stars[i][j]->removeFromParentAndCleanup(true);
			stars[i][j] = nullptr;
			return;
		}
	}
	needClear = false;
	if(GAMEDATA::getInstance()->getCurScore() >= GAMEDATA::getInstance()->getNextScore()){
		GAMEDATA::getInstance()->setCurLevel(GAMEDATA::getInstance()->getCurLevel() + 1);
		m_layer->gotoNextLevel();
	}else{
		m_layer->gotoGameOver();
		CCLOG("GAME OVER");
	}
}

/**
 * 获取剩下的星星数量
 */

int StarMatrix::getLeftStarNum(){
	int ret = 0;
	for(int i=0;i<ROW_NUM;i++){
		for(int j=0;j<COL_NUM;j++){
			if(stars[i][j] == nullptr)
				continue;
			ret++;
		}
	}
	return ret;
}
