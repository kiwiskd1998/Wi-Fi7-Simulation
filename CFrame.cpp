#include "CFrame.h"
int CFrame::getFramelength(int FrameType) {
	switch (FrameType)
	{
	case CTSTOSELF:
		return 8; //时隙
	case BLOCKACK:
		return 6;
	// TODO:需要添加仿真环境中使用的所有的帧类型
	case MURTS:
		return 6;
	case DATA:
		return 80;
	default:
		break;
	}


	return 0;
}
CFrame::CFrame()
{
	this->type = DATA;

}
CFrame::~CFrame()
{


}

/*
帧类
属性：帧长、帧类型
函数：
*/