#include "CFrame.h"
int CFrame::getFramelength(int FrameType) {
	switch (FrameType)
	{
	case CTSTOSELF:
		return 8; //ʱ϶
	case BLOCKACK:
		return 6;
	// TODO:��Ҫ��ӷ��滷����ʹ�õ����е�֡����
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
֡��
���ԣ�֡����֡����
������
*/