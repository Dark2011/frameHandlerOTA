#pragma once

class ShootPoint
{
public:
	ShootPoint();
	ShootPoint(int, int, int);
	ShootPoint(const ShootPoint &);
	
	bool isSame(const ShootPoint &) const;
	bool isSame(const ShootPoint &, int) const;

	bool isSameByX(const ShootPoint &) const;
	bool isEqualByY(const ShootPoint &) const;

	void setX(int);
	void setY(int);
	void setBrightness(int);

	int getX() const;
	int getY() const;
	int getBrightness() const;

private:
	int m_x;
	int m_y;
	int m_brightness;
};
