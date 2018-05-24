#ifndef _crystalCell_
#define _crystalCell_


//----------------------------------------------------cell noise
#include <maya/MVector.h>

#include <maya/MPoint.h>

#include "noise.h"

#include "perlin.h"

using namespace crystalCG;

namespace crystalCG{
	
	class jCell{

		public:
			jCell(){}

			~jCell(){}

			static void setFrequency( double f ) { m_frequency = f; }

			static void setJitter( double j ) { m_jitter = j; }

			static void setSeed(int s){ m_seed = s;}

			static void setNearFar( double n ,double f ) { m_value[0] = n; m_value[1] = f;}

			static double GetValue (double x, double y, double z ,MVector &force );

			static double getFrequency(){ return m_frequency;}

			static double getJitter() { return m_jitter; }

			static void  getNearFar(double value[2]) { value[0] = m_value[0] ; value[1] = m_value[1] ; }

			static int GetSourceModuleCount ()
			{
			  return 1;
			}

		private:
			static double m_frequency;

			static double m_jitter;

			static double m_value[2];

			static int m_seed;
	};
}

#endif