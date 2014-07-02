#include "ShootPoint.h"
#include <math.h>

ShootPoint::ShootPoint() : m_x(-1), m_y(-1), m_brightness(0) { ; }
ShootPoint::ShootPoint(int x, int y, int brightness) : m_x(x), m_y(y), m_brightness(brightness) { ; }

ShootPoint::ShootPoint(const ShootPoint &shootPoint)
{
	m_x = shootPoint.getX();
	m_y = shootPoint.getY();
	m_brightness = shootPoint.getBrightness();
}

bool ShootPoint::isSame(const ShootPoint &prevPoint) const
{
	if( (abs( getX() - prevPoint.getX() ) <= 6) && (abs( getY() - prevPoint.getY() ) <= 6) )
		return true;
	else return false;
}

bool ShootPoint::isSame(const ShootPoint &prevPoint, int criterion) const
{
	if( (abs( getX() - prevPoint.getX() ) <= criterion) &&
		(abs( getY() - prevPoint.getY() ) <= criterion) )
		return true;
	else return false;
}

bool ShootPoint::isSameByX(const ShootPoint &prevPoint) const
{
	if( ( getY() == prevPoint.getY() ) && ( abs( getX() - prevPoint.getX() ) == 1 ) )
		return true;
	else return false;
}

/*test*/
bool ShootPoint::isEqualByY(const ShootPoint &prevPoint) const
{
	if( getY() == prevPoint.getY() ) return true;
	else return false;
}


void ShootPoint::setX(int x)
{
	m_x = x;
}

void ShootPoint::setY(int y)
{
	m_y = y;
}

void ShootPoint::setBrightness(int brightness)
{
	m_brightness = brightness;
}

int ShootPoint::getX() const
{
	return m_x;
}

int ShootPoint::getY() const
{
	return m_y;
}

int ShootPoint::getBrightness() const
{
	return m_brightness;
}