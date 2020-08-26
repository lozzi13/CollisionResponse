#ifndef _MATHS_H_
#define _MATHS_H_

#define _USE_MATH_DEFINES
#include <math.h>
#include <vector>

#define RAD2DEG(x) ((x)*(180.0f/(float)M_PI))
#define DEG2RAD(x) ((x)*((float)M_PI/180.0f))

template<typename T>
T Select(bool condition, T a, T b)
{
	return ((T)condition) * a + (1 - ((T)condition)) * b;
}

template<typename T>
T Min(T a, T b)
{
	return Select(a < b, a, b);
}

template<typename T>
T Max(T a, T b)
{
	return Select(a > b, a, b);
}

template<typename T>
T Clamp(T val, T min, T max)
{
	return Min(Max(val, min), max);
}

float Sign(float a);

float Random(float from, float to);

struct Vec2
{
	float x, y;

	Vec2() : x(0.0f), y(0.0f) {}

	Vec2(float _x, float _y) : x(_x), y(_y) {}

	Vec2 operator+(const Vec2& rhs) const
	{
		return Vec2(x + rhs.x, y + rhs.y);
	}

	Vec2& operator+=(const Vec2& rhs)
	{
		x += rhs.x;
		y += rhs.y;
		return *this;
	}

	Vec2 operator-(const Vec2& rhs) const
	{
		return Vec2(x - rhs.x, y - rhs.y);
	}

	Vec2& operator-=(const Vec2& rhs)
	{
		x -= rhs.x;
		y -= rhs.y;
		return *this;
	}

	Vec2 operator*(float factor) const
	{
		return Vec2(x * factor, y * factor);
	}

	Vec2 operator*(Vec2 factor) const
	{
		return Vec2(x * factor.x, y * factor.y);
	}

	Vec2& operator*=(float factor)
	{
		*this = Vec2(x * factor, y * factor);
		return *this;
	}

	Vec2 operator/(float factor) const
	{
		return Vec2(x / factor, y / factor);
	}

	Vec2& operator/=(float factor)
	{
		*this = Vec2(x / factor, y / factor);
		return *this;
	}

	float& operator [](int value)
	{
		switch (value)
		{
		case 0:
			return x;
			break;
		case 1:
			return y;
			break;
		default:
			break;
		}
	}

	float operator|(const Vec2& rhs) const
	{
		return x * rhs.x + y * rhs.y;
	}

	float operator^(const Vec2& rhs) const
	{
		return x * rhs.y - y * rhs.x;
	}

	float GetLength() const
	{
		return sqrtf(x*x + y * y);
	}

	float GetSqrLength() const
	{
		return x * x + y * y;
	}

	void	Normalize()
	{
		float length = GetLength();
		x /= length;
		y /= length;
	}

	Vec2	Normalized() const
	{
		Vec2 res = *this;
		res.Normalize();
		return res;
	}

	void Reflect(Vec2 normal, float elasticity = 1.0f)
	{
		*this = *this - normal * (1.0f + elasticity) * (*this | normal);
	}

	Vec2 GetNormal() const
	{
		return Vec2(-y, x);
	}

	float Angle(const Vec2& to)
	{
		float cosAngle = Clamp(Normalized() | to.Normalized(), -1.0f, 1.0f);
		float angle = RAD2DEG(acosf(cosAngle)) * Sign(*this ^ to);
		return angle;
	}
};

struct Mat2
{
	Vec2 X, Y;

	Mat2() : X(1.0f, 0.0f), Y(0.0f, 1.0f) {}

	Mat2(float a, float b, float c, float d) : X(a, c), Y(b, d) {}

	Mat2	GetInverse() const
	{
		return Mat2(X.x, X.y, Y.x, Y.y);
	}

	float	GetAngle() const
	{
		return Vec2(1.0f, 0.0f).Angle(X);
	}

	void	SetAngle(float angle)
	{
		float c = cosf(angle * ((float)M_PI / 180.0f));
		float s = sinf(angle * ((float)M_PI / 180.0f));

		X.x = c; X.y = s;
		Y.x = -s; Y.y = c;
	}

	void Rotate(float angle)
	{
		Mat2 matRot;
		matRot.SetAngle(angle);
		*this = *this * matRot;
	}

	Mat2 operator*(const Mat2& rhs) const
	{
		return Mat2(X.x*rhs.X.x + Y.x*rhs.X.y, X.x*rhs.Y.x + Y.x*rhs.Y.y, X.y*rhs.X.x + Y.y*rhs.X.y, X.y*rhs.Y.x + Y.y*rhs.Y.y);
	}

	Vec2 operator*(const Vec2& vec) const
	{
		return Vec2(X.x*vec.x + Y.x*vec.y, X.y*vec.x + Y.y*vec.y);
	}
};

struct Line
{
	Vec2 point, dir;
	float length;
	Line() = default;
	Line(Vec2 _point, Vec2 _dir, float _length) : point(_point), dir(_dir), length(_length) {}

	Vec2	GetNormal() const
	{
		return dir.GetNormal();
	}

	// positive value means point above line, negative means point is under line
	float	GetPointDist(const Vec2& pt) const
	{
		return (pt - point) | GetNormal();
	}

	Line	Transform(const Mat2& rotation, const Vec2& position) const
	{
		return Line(position + rotation * point, rotation * dir, length);
	}

	Vec2	Project(const Vec2& pt) const
	{
		return point + dir * ((pt - point) | dir);
	}

	Vec2 GetEndPoint()const
	{
		return point + dir * length;
	}
	Line Inverse()const
	{
		return Line(point + dir * length, dir * -1, length);
	}

	Vec2 static Clip(const Line& Plane, const Line& incident)
	{
		Vec2 normal = Plane.GetNormal();
		float value = normal | incident.dir;
		if (abs(value) <= FLT_EPSILON)
			return incident.point;

		Vec2 W = (incident.point - Plane.point);
		float project = ((W | normal) * -1) / value;

		return incident.point + incident.dir * Clamp(project, 0.0f, incident.length);
	}
};

struct Transform
{
private:
	Vec2 Position;
	Mat2 Rotation;
	bool isDirty = false;
public:

	Transform() = default;
	~Transform() = default;

	void setPosition(const Vec2& position)
	{
		isDirty = true;
		Position = position;
	}

	void updatePosition(const Vec2& position)
	{
		isDirty = true;
		Position += position;
	}

	const Vec2& GetPosition() const
	{
		return Position;
	}

	void setRotation(const Mat2& rotation)
	{
		isDirty = true;
		Rotation = rotation;
	}
	void SetAngle(float angle)
	{
		isDirty = true;
		Rotation.SetAngle(angle);
	}
	float GetAngle()
	{
		return Rotation.GetAngle();
	}


	const Mat2& GetRotation() const
	{
		return Rotation;
	}

	bool  HasChanged() const
	{
		return isDirty;
	}
	void  HasChanged(bool value)
	{
		isDirty = value;
	}
};

#endif
