#pragma once

#include <QtCore/QVector>
class FLSAlgorithm;

namespace FAC
{
	enum TypeOfAlgo { ONE_FRAME_TYPE1 = 0, ALL_FRAMES_TYPE1, ONE_FRAME_TYPE2,
				      ONE_FRAME_TYPE3, ALL_FRAMES_TYPE2 };
}

class FLSAlgorithmsCreator
{
public:
	FLSAlgorithmsCreator();
	FLSAlgorithm* create(FAC::TypeOfAlgo);

private:
	void init();

private:
	typedef FLSAlgorithm* (*createFUNC)();
	QVector<createFUNC>m_creator; 
};
