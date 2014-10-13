#ifndef _STAR_H_
#define _STAR_H_

#include "cocos2d.h"
using namespace cocos2d;

class Star : public Sprite{
public:
	enum color{
		GREEN,
		RED,
		YELLOW,
		PURPLE,
		BLUE
	};
	static Star* create(int color);

	inline bool isSelected(){return selected;}
	inline void setSelected(bool b){selected = b;}
	inline int getColor(){return color;}
	inline void setDesPosition(const Point& p){desPosition = p;}
	inline Point getDesPosition(){return desPosition;}
	inline int getIndexI(){return index_i;}
	inline int getIndexJ(){return index_j;}
	inline void setIndex_ij(int i,int j){index_i = i;index_j = j;}

	void updatePosition();
private:
	char* getImage(int color);

public:
	const static int MOVE_SPEED = 4;
	static const int COLOR_MAX_NUM = 5;
	static const int STAR_WIDTH = 48;
	static const int STAR_HEIGHT = 48;

private:
	int color;
	Point desPosition;
	bool selected;
	int index_i;
	int index_j;
};
#endif
