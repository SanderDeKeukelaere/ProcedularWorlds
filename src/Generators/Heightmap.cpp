#include "Heightmap.h"

#include "../Renderer/SDLWrapper.h"
#include "../Perlin/Perlin.h"

#include <time.h>

that::gen::Heightmap::Heightmap(that::SDLWrapper& sdl)
{
	srand(static_cast<unsigned int>(time(nullptr)));

	const auto& windowSize{ sdl.GetWindowSize() };

	that::Perlin perlin{ m_Octaves, m_Zoom };
	for (int x{}; x < windowSize.x; ++x)
	{
		for (int y{}; y < windowSize.y; ++y)
		{
			const float perlinValue{ perlin.GetNoise(x, y) };

			const Color heightMapColor{ GetHeightColor(perlinValue) };

			sdl.DrawPixel({ x,y }, heightMapColor);
		}
	}
}

that::Color that::gen::Heightmap::GetHeightColor(float perlinValue) const
{
	if (perlinValue < m_SeaLevel)
	{
		return m_WaterColor;
	}

	// Calculate the percentage above sealevel
	const float aboveSeaHeight{ 1.0f - m_SeaLevel };
	const float perlinContinental{ (perlinValue - m_SeaLevel) / aboveSeaHeight };

	if (perlinContinental < m_LowLevel)
	{
		return m_LowLandColor;
	}
	else if (perlinContinental < m_MiddleLevel)
	{
		// Calculate the percentage where the perlin is between low level and middle level
		const float lowPerlin{ (perlinContinental - m_LowLevel) / (m_MiddleLevel - m_LowLevel) };

		// Lerp from low land color to middle land color
		return Color
		{
			static_cast<unsigned int>(m_LowLandColor.r + (m_MiddleLandColor.r - m_LowLandColor.r) * lowPerlin),
			static_cast<unsigned int>(m_LowLandColor.g + (m_MiddleLandColor.g - m_LowLandColor.g) * lowPerlin),
			static_cast<unsigned int>(m_LowLandColor.b + (m_MiddleLandColor.b - m_LowLandColor.b) * lowPerlin)
		};
	}
	else
	{
		// Calculate the percentage where the perlin is between middle and high level
		//		and clamp the percentage at 100%
		const float highPerlin{ std::min((perlinContinental - m_MiddleLevel) / (m_HighLevel - m_MiddleLevel), 1.0f) };

		// Lerp from middle land color to high land color
		return Color
		{
			static_cast<unsigned int>(m_MiddleLandColor.r + (m_HighLandColor.r - m_MiddleLandColor.r) * highPerlin),
			static_cast<unsigned int>(m_MiddleLandColor.g + (m_HighLandColor.g - m_MiddleLandColor.g) * highPerlin),
			static_cast<unsigned int>(m_MiddleLandColor.b + (m_HighLandColor.b - m_MiddleLandColor.b) * highPerlin)
		};
	}
}
