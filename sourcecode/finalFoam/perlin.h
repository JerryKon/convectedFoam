#ifndef _crystalPerlin_
#define _crystalPerlin_

#include "noise.h"

namespace crystalCG
{

  namespace module
  {
   
    class Perlin
    {

      public:

        Perlin ();

        double GetFrequency () const
        {
          return m_frequency;
        }

        double GetLacunarity () const
        {
          return m_lacunarity;
        }

        int GetNoiseQuality () const
        {
          return m_noiseQuality;
        }

        int GetOctaveCount () const
        {
          return m_octaveCount;
        }

        double GetPersistence () const
        {
          return m_persistence;
        }

        int GetSeed () const
        {
          return m_seed;
        }

        virtual int GetSourceModuleCount () const
        {
          return 0;
        }

        virtual double GetValue (double x, double y, double z) const;

        void SetFrequency (double frequency)
        {
          m_frequency = frequency;
        }

        void SetLacunarity (double lacunarity)
        {
          m_lacunarity = lacunarity;
        }

        void SetNoiseQuality (int q)
        {
          m_noiseQuality = q;
        }

        void SetOctaveCount (int octaveCount)
        {
          m_octaveCount = octaveCount;
        }

        void SetPersistence (double persistence)
        {
          m_persistence = persistence;
        }

        void SetSeed (int seed)
        {
          m_seed = seed;
        }

      protected:

        /// Frequency of the first octave.
        double m_frequency;

        /// Frequency multiplier between successive octaves.
        double m_lacunarity;

        /// Quality of the Perlin noise.
        int m_noiseQuality;

        /// Total number of octaves that generate the Perlin noise.
        int m_octaveCount;

        /// Persistence of the Perlin noise.
        double m_persistence;

        /// Seed value used by the Perlin-noise function.
        int m_seed;

    };

  }

}

#endif
