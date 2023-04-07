//
//  AdvancedGIF.hpp
//  ExceptionCatcher
//
//  Created by KKahiru on 2023/04/06.
//

# pragma once

# include <Siv3D.hpp> // OpenSiv3D v0.6.7

class AdvancedGIF {
	
	Array<Texture> m_textures;
	
	Array<int32> m_delays;
	
public:
	
	AdvancedGIF(FilePathView path, TextureDesc desc = TextureDesc::Unmipped);
	
	Texture getFrame(double t) const;
	
};
