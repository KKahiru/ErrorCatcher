//
//  AdvancedGIF.cpp
//  ExceptionCatcher
//
//  Created by KKahiru on 2023/04/06.
//

# include "AdvancedGIF.hpp"

AdvancedGIF::AdvancedGIF(FilePathView path, TextureDesc desc)
{
	const AnimatedGIFReader gifReader{ path };
	Array<Image> images;
	if (gifReader)
	{
		gifReader.read(images, m_delays);
		for (const auto& image : images)
		{
			m_textures << Texture{ image, desc };
		}
	}
	else
	{
		m_textures << Texture();
	}
}


Texture AdvancedGIF::getFrame(double t) const
{
	const size_t index = AnimatedGIFReader::GetFrameIndex(t, m_delays);
	return m_textures[index];
}
