//----------------------------------------------------------------------------------------
//
//	siv::PerlinNoise
//	Perlin noise library for modern C++
//
//	Copyright (C) 2013-2018 Ryo Suzuki <reputeless@gmail.com>
//
//	Permission is hereby granted, free of charge, to any person obtaining a copy
//	of this software and associated documentation files(the "Software"), to deal
//	in the Software without restriction, including without limitation the rights
//	to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
//	copies of the Software, and to permit persons to whom the Software is
//	furnished to do so, subject to the following conditions :
//	
//	The above copyright notice and this permission notice shall be included in
//	all copies or substantial portions of the Software.
//	
//	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
//	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
//	THE SOFTWARE.
//
//----------------------------------------------------------------------------------------
//some parts have been slightly changed to accomodate for my needs


# pragma once
# include <cstdint>
# include <numeric>
# include <algorithm>
# include <random>
# include <type_traits>

namespace siv
{
	using precision = float;

	inline float floorType(float f) { return floorf(f); }
	inline double floorType(double f) { return floor(f); }

	class PerlinNoise
	{
	private:

		std::uint8_t p[512];

		static precision Fade(precision t) noexcept
		{
			return t * t * t * (t * (t * 6 - 15) + 10);
		}

		static precision Lerp(precision t, precision a, precision b) noexcept
		{
			return a + t * (b - a);
		}

		static precision Grad(std::uint8_t hash, precision x, precision y, precision z) noexcept
		{
			const std::uint8_t h = hash & 15;
			const precision u = h < 8 ? x : y;
			const precision v = h < 4 ? y : h == 12 || h == 14 ? x : z;
			return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
		}

	public:

		explicit PerlinNoise(std::uint32_t seed = std::default_random_engine::default_seed)
		{
			reseed(seed);
		}

		template <class URNG>
		explicit PerlinNoise(URNG& urng)
		{
			reseed(urng);
		}

		void reseed(std::uint32_t seed)
		{
			for (size_t i = 0; i < 256; ++i)
			{
				p[i] = static_cast<std::uint8_t>(i);
			}

			std::shuffle(std::begin(p), std::begin(p) + 256, std::default_random_engine(seed));

			for (size_t i = 0; i < 256; ++i)
			{
				p[256 + i] = p[i];
			}
		}

		template <class URNG>
		void reseed(URNG& urng)
		{
			for (size_t i = 0; i < 256; ++i)
			{
				p[i] = static_cast<std::uint8_t>(i);
			}

			std::shuffle(std::begin(p), std::begin(p) + 256, urng);

			for (size_t i = 0; i < 256; ++i)
			{
				p[256 + i] = p[i];
			}
		}

		precision noise(precision x) const
		{
			return noise(x, 0.0, 0.0);
		}

		precision noise(precision x, precision y) const
		{
			return noise(x, y, 0.0);
		}

		inline precision noise(precision x, precision y, precision z) const
		{
			//modified here
			precision florX = floorType(x);
			precision florY = floorType(y);
			precision florZ = floorType(z);

			const std::int32_t X = static_cast<std::int32_t>(florX) & 255;
			const std::int32_t Y = static_cast<std::int32_t>(florY) & 255;
			const std::int32_t Z = static_cast<std::int32_t>(florZ) & 255;

			x -= florX;
			y -= florY;
			z -= florZ;

			const precision u = Fade(x);
			const precision v = Fade(y);
			const precision w = Fade(z);

			const std::int32_t A = p[X] + Y, AA = p[A] + Z, AB = p[A + 1] + Z;
			const std::int32_t B = p[X + 1] + Y, BA = p[B] + Z, BB = p[B + 1] + Z;

			return Lerp(w, Lerp(v, Lerp(u, Grad(p[AA], x, y, z),
				Grad(p[BA], x - 1, y, z)),
				Lerp(u, Grad(p[AB], x, y - 1, z),
					Grad(p[BB], x - 1, y - 1, z))),
				Lerp(v, Lerp(u, Grad(p[AA + 1], x, y, z - 1),
					Grad(p[BA + 1], x - 1, y, z - 1)),
					Lerp(u, Grad(p[AB + 1], x, y - 1, z - 1),
						Grad(p[BB + 1], x - 1, y - 1, z - 1))));
		}

		precision octaveNoise(precision x, std::int32_t octaves) const
		{
			precision result = 0.0;
			precision amp = 1.0;

			for (std::int32_t i = 0; i < octaves; ++i)
			{
				result += noise(x) * amp;
				x *= 2.0;
				amp *= 0.5;
			}

			return result;
		}

		precision octaveNoise(precision x, precision y, std::int32_t octaves) const
		{
			precision result = 0.0;
			precision amp = 1.0;

			for (std::int32_t i = 0; i < octaves; ++i)
			{
				result += noise(x, y) * amp;
				x *= 2.0;
				y *= 2.0;
				amp *= 0.5;
			}

			return result;
		}

		precision octaveNoise(precision x, precision y, precision z, std::int32_t octaves) const
		{
			if(octaves > 8)
			{
				precision result = 0.0;
				precision amp = 1.0;

				for (std::int32_t i = 0; i < octaves; ++i)
				{
					result += noise(x, y, z) * amp;
					x *= 2.0;
					y *= 2.0;
					z *= 2.0;
					amp *= 0.5;
				}

				return result;
			}

			precision xData[8] = { x };
			precision yData[8] = { y };
			precision zData[8] = { z };
			precision rez[8] = { 0 };
			precision amp[8] = { 1 };

			for (std::int32_t i = 1; i < octaves; ++i)
			{
				xData[i] = xData[i - 1] * 2.0;
				yData[i] = yData[i - 1] * 2.0;
				zData[i] = zData[i - 1] * 2.0;
				amp[i] = amp[i-1] * 0.5;
			}

			for(int i=0; i<octaves; i++)
			{
				rez[i] = noise(xData[i], yData[i], zData[i]) * amp[i];
			}

			//rez[0] = noise(xData[0], yData[0], zData[0]) * amp[0];

			for (int i = 1; i < octaves; i++)
			{
				rez[0] += rez[i];
			}

			return rez[0];
		}

		precision noise0_1(precision x) const
		{
			return noise(x) * (precision)0.5 + (precision)0.5;
		}

		precision noise0_1(precision x, precision y) const
		{
			return noise(x, y) * (precision)0.5 + (precision)0.5;
		}

		precision noise0_1(precision x, precision y, precision z) const
		{
			return noise(x, y, z) * (precision)0.5 + (precision)0.5;
		}

		precision octaveNoise0_1(precision x, std::int32_t octaves) const
		{
			return octaveNoise(x, octaves) * (precision)0.5 + (precision)0.5;
		}

		precision octaveNoise0_1(precision x, precision y, std::int32_t octaves) const
		{
			return octaveNoise(x, y, octaves) * (precision)0.5 + (precision)0.5;
		}

		precision octaveNoise0_1(precision x, precision y, precision z, std::int32_t octaves) const
		{
			return octaveNoise(x, y, z, octaves) * (precision)0.5 + (precision)0.5;
		}
	};
}