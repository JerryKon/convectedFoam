#include "cell.h"

using namespace crystalCG;

double jCell::m_frequency;
double jCell::m_jitter;
double jCell::m_value[2];
int  jCell::m_seed;

double jCell::GetValue (double x, double y, double z ,MVector &force )
{
	  x *= m_frequency;
	  y *= m_frequency;
	  z *= m_frequency;

	  int xInt = (x > 0.0? (int)x: (int)x - 1);
	  int yInt = (y > 0.0? (int)y: (int)y - 1);
	  int zInt = (z > 0.0? (int)z: (int)z - 1);

	  double minDist = 2147483647.0;
	  double minDist2 = 2147483648.0;

	  MPoint nearPoint,farPoint;
	  for (int zCur = zInt - 2; zCur <= zInt + 2; zCur++) {
		for (int yCur = yInt - 2; yCur <= yInt + 2; yCur++) {
		  for (int xCur = xInt - 2; xCur <= xInt + 2; xCur++) {

			double xPos = xCur + m_jitter * ( abs( ValueNoise3D (xCur, yCur, zCur, m_seed    ) )- 0.5 ) ;
			double yPos = yCur + m_jitter * ( abs( ValueNoise3D (xCur, yCur, zCur, m_seed + 1) )- 0.5 );
			double zPos = zCur + m_jitter * ( abs( ValueNoise3D (xCur, yCur, zCur, m_seed + 2) )- 0.5 );

			double xDist = xPos - x;
			double yDist = yPos - y;
			double zDist = zPos - z;
			double dist = sqrt( xDist * xDist + yDist * yDist + zDist * zDist );

			if (dist < minDist) {

			  minDist2 = minDist;
			  minDist  = dist;
			  nearPoint = MPoint( xPos,yPos,zPos );

			}
			else if(dist < minDist2)
			{
				minDist2 = dist;
				farPoint = MPoint( xPos,yPos,zPos );

			}
		  }
		}
	  }

	  double result =  minDist * m_value[0] + minDist2 * m_value[1];
	  
	  force = farPoint - nearPoint;
	  
	  return result ;
}

