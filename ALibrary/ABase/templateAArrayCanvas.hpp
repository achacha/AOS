#ifndef INCLUDED_templateAArrayCanvas_HPP_
#define INCLUDED_templateAArrayCanvas_HPP_

#include "apiABase.hpp"
#include "ADebugDumpable.hpp"
#include "templateAutoPtr.hpp"
#include <cassert>
#include <limits>

#ifndef min
#define min(a, b)  (((a) < (b)) ? (a) : (b))
#endif

template<class T>
class AArrayCanvas : public ADebugDumpable
{
  T *m_data;
	u2 m_sizeX;
	u2 m_sizeY;
	u4 m_length;

	AArrayCanvas() {}  // No default object, needs dimensions

public:
	// Creates a linear array that gets then partitioned into 'x' by 'y' canvas and filled with 'c'
	AArrayCanvas(u2 x, u2 y, T c = 0x0) : m_sizeX(x), m_sizeY(y)
	{
		m_length = x*y;
    m_data = new T[m_length];
		clear(c);
	}
	~AArrayCanvas() {}

	// Attributes
	u2 getSizeX() const { return m_sizeX; }
	u2 getSizeY() const { return m_sizeY; }
  T *useData() { return m_data; }  //a_Use at your own risk, this is the raw data

	// Primitives
	T& at(u4 u)		// Access element which is x+m_sizeX*y in a linearized array
	{ 
		AASSERT(this, u < m_length);
		return m_data[u];
	}
	
  T at(u4 u) const		// Access copy of element at L which is x+m_sizeX*y in a linearized array
	{ 
		AASSERT(this, u < m_length);
		return m_data[u];
	}

	T& at(u2 x, u2 y)  // Access for modification element at (x,y)
	{ 
		AASSERT(this, u4(x) + m_sizeX*y < m_length);
		return m_data[x + m_sizeX*y];
	}
	
  T at(u2 x, u2 y) const
	{
		AASSERT(this, u4(x) + m_sizeX*y < m_length);
    const T* p = m_data.get();
    return p[x + m_sizeX*y];
	}

	void drawPoint(u2 x, u2 y, T c)   // Set element of type T at (x,y)
	{
		AASSERT(this, u4(x) + m_sizeX*y < m_length);
		m_data[x + m_sizeX*y]=c;
	}

	void clear(T c)
	{
    memset(m_data, c, m_length);
	}

	// Draws a line between 2 points using parametric equation (direction independent)
	// Uses thick lines by default to generate lines which look better when angles cause large aliasing
	void drawLine(u2 px0, u2 py0, u2 px1, u2 py1, T c, bool boolThick=true)
	{
		u2 x0 = min(px0, m_sizeX-1);
		u2 x1 = min(px1, m_sizeX-1);
		u2 y0 = min(py0, m_sizeY-1);
		u2 y1 = min(py1, m_sizeY-1);
		double rounding = 0.0;
		if (boolThick)
			rounding = 0.5;

		int dY = y1 - y0, dX = x1 - x0;
		if (dX == 0) {
			// Vertical line
			int dir = (y0 > y1 ? -1 : 1);
			for (int i = y0; i <= y1; i += dir)
				drawPoint(x0, i, c);
		}
		else if (dY == 0) {
			// Horizontal line
			int dir = (x0 > x1 ? -1 : 1);
			for (int i = x0; i <= x1; i += dir)
				drawPoint(i, y0, c);
		}
		else
		{
			// Finite slope
			double radius = double(dX) * double(dX) + double(dY) * double(dY);
			radius = sqrt(radius);
			double xp = x0, yp = y0, dxp = double(dX) / radius, dyp = double(dY) / radius;
			u2 rlimit = u2(radius + 0.5);
			for (u2 i = 0; i < rlimit; ++i)
			{
				drawPoint(u2(xp+rounding), u2(yp+rounding), c);
				xp += dxp;
				yp += dyp;
			}
			drawPoint(x1, y1, c);
		}
	}

	// Draws a rectangle (p0 and p1 are the vertices)
	void drawRect(u2 px0, u2 py0, u2 px1, u2 py1, T c)
	{
		u2 x0 = min(px0, m_sizeX-1);
		u2 x1 = min(px1, m_sizeX-1);
		u2 y0 = min(py0, m_sizeY-1);
		u2 y1 = min(py1, m_sizeY-1);

		if (x0 > x1) {
			for (u2 x=x1; x<=x0; ++x) {
				drawPoint(x, y0, c);
				drawPoint(x, y1, c);
			}
		}
		else {
			for (u2 x=x0; x<=x1; ++x) {
				drawPoint(x, y0, c);
				drawPoint(x, y1, c);
			}
		}

		if (y0 > y1) {
			for (u2 y=y1+1; y<=y0-1; ++y) {
				drawPoint(x0, y, c);
				drawPoint(x1, y, c);
			}
		}
		else {
			for (u2 y=y0+1; y<=y1-1; ++y) {
				drawPoint(x0, y, c);
				drawPoint(x1, y, c);
			}
		}
	}

public:
#ifdef __DEBUG_DUMP__
  virtual void debugDump(std::ostream& os = std::cerr, int indent = 0x0) const;
#endif
};

#ifdef __DEBUG_DUMP__
template<class T>
void AArrayCanvas<T>::debugDump(std::ostream& os, int indent) const
{
  ADebugDumpable::indent(os, indent) << "(AArrayCanvas @ " << std::hex << this << std::dec << ") {" << std::endl;
  ADebugDumpable::indent(os, indent+1) << "m_sizeX=" << m_sizeX << std::endl;
  ADebugDumpable::indent(os, indent+1) << "m_sizeY=" << m_sizeY << std::endl;
  ADebugDumpable::indent(os, indent+1) << "m_length=" << m_length << std::endl;

  ADebugDumpable::indent(os, indent+1) << "m_data={" << std::endl;
  ADebugDumpable::dumpMemory_Hex(os, m_data, m_length, indent+2);
  ADebugDumpable::indent(os, indent+1) << "}" << m_sizeY << std::endl;

  ADebugDumpable::indent(os, indent) << "}" << std::endl;
}
#endif

// Sample usage
/*
	AArrayCanvas<u1> canvas(10,10,' ');
	canvas.drawRect(9,9,0,0,'#');
	canvas.drawLine(8,2,8,5,'.');
	canvas.drawLine(2,7,6,4,'*');

	for (u2 y=0;	y<canvas.getSizeY(); ++y) {
		for (u2 x=0; x<canvas.getSizeX(); ++x) {
			std::cout << (char)canvas.at(x,y);
		}
		std::cout << std::endl;
	}
*/

#endif // INCLUDED_templateAArrayCanvas_HPP_
