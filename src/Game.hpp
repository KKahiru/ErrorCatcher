# pragma once

# include "Common.hpp"
# include "AdvancedGIF.hpp"

class realizedExc
{
public:
	
	realizedExc(StringView type, uint16 lineNum);
	
	realizedExc();
	
	uint16 lineNum;
	
	double invasionDegree = 0;
	
	StringView DisplayName() const;
	
	float Speed() const;
	
	float Damage() const;
	
	s3d::Color Color() const;
	
	
private:
	
	String m_type;
	
	String m_display_name;
	
	float m_speed;
	
	float m_damage;
	
	s3d::Color m_color;
	
};

// ゲームシーン
class Game : public App::Scene
{
public:

	Game(const InitData& init);

	void update() override;

	void draw() const override;

private:
	
	Array<realizedExc> fallenExcList;
	
	Array<Array<uint8>> symbolDurability;
	
	Effect effect;
	
	bool isGamePaused = 1;
	
	uint8 explanationState = 0;
	
	float brokenness = 0;
	
	double generateTick = 0;
	
	// 実際のラインの数はmaxLine + 1になる
	const uint16 maxLine = 5;
	
	const uint16 lineWidth = 100;
	
	const uint16 leftMargin = (Scene::Width() - lineWidth * (maxLine + 1)) / (maxLine + 2);
	
	const uint16 topMargin = 10;
	
	const uint16 lineHeight = Scene::Height() - topMargin * 2 - lineWidth;
	
	const float symbol = 0.1;
	
	double symbolHeight;
	
	double symbolTopMargin;
	
	const uint16 symbolLeftMargin = 10;
	
	const AdvancedGIF explosionGIF{ Resource(U"resource/gif/explosion.gif"), TextureDesc::Mipped };
	
	const Audio explosionAudio{ Resource(U"resource/sound/explosion.mp3") };
	
};
