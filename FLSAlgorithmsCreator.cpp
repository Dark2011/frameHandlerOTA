#include "FLSAlgorithmsCreator.h"
#include "FLSAlgorithm.h"

FLSAlgorithm *create_OFT1()
{
	return new FLSAlgorithm_OF_Type1();
}

FLSAlgorithm *create_AFT1()
{
	return new FLSAlgorithm_AF_Type1();
}

FLSAlgorithm *create_OFT2()
{
	return new FLSAlgorithm_OF_Type2();
}

FLSAlgorithm *create_OFT3()
{
	return new FLSAlgorithm_OF_Type3();
}

FLSAlgorithm *create_AFT2()
{
	return new FLSAlgorithm_AF_Type2();
}


FLSAlgorithmsCreator::FLSAlgorithmsCreator() { init(); }

void FLSAlgorithmsCreator::init()
{
	m_creator.push_back(create_OFT1);
	m_creator.push_back(create_AFT1);
	m_creator.push_back(create_OFT2);
	m_creator.push_back(create_OFT3);
	m_creator.push_back(create_AFT2);
}

FLSAlgorithm* FLSAlgorithmsCreator::create(FAC::TypeOfAlgo type)
{
	return m_creator.at(type)();
}