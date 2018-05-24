#include "perlin.h"


using namespace crystalCG::module;

Perlin::Perlin ():
  m_frequency    (1.0   ),
  m_lacunarity   (2.0  ),
  m_noiseQuality (1),
  m_octaveCount  (6),
  m_persistence  (0.5 ),
  m_seed         (0)
{
}

double Perlin::GetValue (double x, double y, double z) const
{
  double value = 0.0;
  double signal = 0.0;
  double curPersistence = 1.0;
  double nx, ny, nz;
  int seed;

  x *= m_frequency;
  y *= m_frequency;
  z *= m_frequency;

  for (int curOctave = 0; curOctave < m_octaveCount; curOctave++) {

    // Make sure that these floating-point values have the same range as a 32-
    // bit integer so that we can pass them to the coherent-noise functions.
    nx = MakeInt32Range (x);
    ny = MakeInt32Range (y);
    nz = MakeInt32Range (z);

    // Get the coherent-noise value from the input value and add it to the
    // final result.
    seed = (m_seed + curOctave) & 0xffffffff;
    signal = GradientCoherentNoise3D (nx, ny, nz, seed, m_noiseQuality);
    value += signal * curPersistence;

    // Prepare the next octave.
    x *= m_lacunarity;
    y *= m_lacunarity;
    z *= m_lacunarity;
    curPersistence *= m_persistence;
  }

  return value;
}
