# pragma once

# include "Common.hpp"

// タイトルシーン
class Title : public App::Scene
{
public:

	Title(const InitData& init);

	void update() override;

	void draw() const override;

private:

	Rect m_startButton{ Arg::center = CenterPos.movedBy(0, 50), 300, 60 };
	Transition m_startTransition{ 0.4s, 0.2s };

	Rect m_exitButton{ Arg::center = CenterPos.movedBy(0, 150), 300, 60 };
	Transition m_exitTransition{ 0.4s, 0.2s };

	Effect m_effect;

	double m_decorationDelta = 0;
};

class Decolation : public IEffect
{
	LineString line;
public:
	explicit Decolation();
	bool update(double t) override;
};
