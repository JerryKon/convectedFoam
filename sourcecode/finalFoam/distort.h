#ifndef _distort_
#define _distort_

#include "perlin.h"

using namespace crystalCG;

class distort
{
	public:
		distort(){};
		~distort(){};

		static void setSeed(int s){ 
			m_xDistortModule.SetSeed (s + 1);
			m_yDistortModule.SetSeed (s + 2);
			m_zDistortModule.SetSeed (s + 3);

		}

		static void SetTurFrequency (double fx,double fy, double fz)
		{
		  // Set the frequency of each Perlin-noise module.
		  m_xDistortModule.SetFrequency (fx);
		  m_yDistortModule.SetFrequency (fy);
		  m_zDistortModule.SetFrequency (fz);
		}

		static void SetTurPower (double px,double py,double pz)
		{
		  m_powerX = px;
		  m_powerY = py;
		  m_powerZ = pz;
		}

		static void SetTurRoughness (int roughness)
		{
		  // Set the octave count for each Perlin-noise module.
		  m_xDistortModule.SetOctaveCount (roughness);
		  m_yDistortModule.SetOctaveCount (roughness);
		  m_zDistortModule.SetOctaveCount (roughness);
		}

		static void getValue( float& x, float& y, float& z ,float xx, float yy,float zz )
		{
			  float x0, y0, z0;
			  float x1, y1, z1;
			  float x2, y2, z2;

			  x0 = xx + (12414.0f / 65536.0f);
			  y0 = yy + (65124.0f / 65536.0f);
			  z0 = zz + (31337.0f / 65536.0f);
			  
			  x1 = xx + (26519.0f / 65536.0f);
			  y1 = yy + (18128.0f / 65536.0f);
			  z1 = zz + (60493.0f / 65536.0f);
			  
			  x2 = xx + (53820.0f / 65536.0f);
			  y2 = yy + (11213.0f / 65536.0f);
			  z2 = zz + (44845.0f / 65536.0f);

			  x += (float)(m_xDistortModule.GetValue (x0, y0, z0) * m_powerX);
			  y += (float)(m_yDistortModule.GetValue (x1, y1, z1) * m_powerY);
			  z += (float)(m_zDistortModule.GetValue (x2, y2, z2) * m_powerZ);
		}

		static void getValue( float& x, float& y, float& z )
		{
			  float x0, y0, z0;
			  float x1, y1, z1;
			  float x2, y2, z2;

			  x0 = x + (12414.0f / 65536.0f);
			  y0 = y + (65124.0f / 65536.0f);
			  z0 = z + (31337.0f / 65536.0f);
			  
			  x1 = x + (26519.0f / 65536.0f);
			  y1 = y + (18128.0f / 65536.0f);
			  z1 = z + (60493.0f / 65536.0f);
			  
			  x2 = x + (53820.0f / 65536.0f);
			  y2 = y + (11213.0f / 65536.0f);
			  z2 = z + (44845.0f / 65536.0f);

			  x += (float)(m_xDistortModule.GetValue (x0, y0, z0) * m_powerX);
			  y += (float)(m_yDistortModule.GetValue (x1, y1, z1) * m_powerY);
			  z += (float)(m_zDistortModule.GetValue (x2, y2, z2) * m_powerZ);
		}
	private:

		static double m_powerX;
		static double m_powerY;
		static double m_powerZ;

		static module::Perlin m_xDistortModule;

		static module::Perlin m_yDistortModule;

		static module::Perlin m_zDistortModule;
	
};

#endif