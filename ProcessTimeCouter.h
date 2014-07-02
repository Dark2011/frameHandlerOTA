#pragma once
//opencv
#include <opencv2/opencv.hpp>

class ProcessTimeCounter
{
public:
	ProcessTimeCounter() : m_duration(0.0) { ; }

	void start() { m_duration = (double)( cv::getTickCount() ); }
	void stop()
	{
		m_duration = (double)( cv::getTickCount() ) - m_duration;
		m_duration /= cv::getTickFrequency();
	}

	void reset() { m_duration = 0.0; }
	double getDuration() const { return m_duration; }

private:
	double m_duration;
};