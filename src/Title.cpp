# include "Title.hpp"

Decolation::Decolation()
{
	line.push_back(Vec2{ RandomPoint(Scene::Rect().stretched(-50)) });
	bool b = RandomBool();
	const int8 m1 = RandomBool() ? 1 : -1;
	const int8 m2 = RandomBool() ? 1 : -1;
	for (int i = 0; i < Random(2, 5); i++) {
		if (b)
		{
			line.push_back(line.back().movedBy(m1 * Random(10, 100), 0));
		}
		else
		{
			line.push_back(line.back().movedBy(0, m2 * Random(10, 100)));
		}
		b = !b;
	}
}

bool Decolation::update(double t)
{
	line.draw(ColorF{1, Periodic::Triangle0_1(1s, t)});
	Circle{ line.calculatePointFromOrigin(t * line.calculateLength()), 10 }.draw(ColorF{1, Periodic::Triangle0_1(1s, t) + 0.1});
	return t < 1;
}


Title::Title(const InitData& init)
	: IScene{ init } {
		Scene::SetBackground(Color{ 11, 22, 33 });
		// リザルト画面に移設する
		ClearPrint();
	}

void Title::update()
{
	m_startTransition.update(m_startButton.mouseOver());
	m_exitTransition.update(m_exitButton.mouseOver());

	if (m_startButton.mouseOver() or m_exitButton.mouseOver())
	{
		Cursor::RequestStyle(CursorStyle::Hand);
	}

	if (m_startButton.leftClicked())
	{
		// ゲームシーンへ
		changeScene(SceneState::Game);
	}
	else if (m_exitButton.leftClicked())
	{
		// 終了
		System::Exit();
	}
	m_decorationDelta += Scene::DeltaTime() / 0.3;
	while (m_decorationDelta > 1) {
		m_effect.add<Decolation>();
		m_decorationDelta --;
	}
	
}

void Title::draw() const
{
	//Scene::SetBackground(Color{U"#004599"});
	m_effect.update();
	FontAsset(U"Title.TitleFont")(U"例外キャッチャー")
	.drawAt(Vec2{ 400, 100 }, ColorF{ 0.75 });

	m_startButton.draw(Scene::GetBackground()).draw(ColorF{ 1.0, m_startTransition.value() }).drawFrame(2);
	m_exitButton.draw(Scene::GetBackground()).draw(ColorF{ 1.0, m_exitTransition.value() }).drawFrame(2);

	FontAsset(U"Menu")(U"PLAY").drawAt(m_startButton.center(), ColorF{ 0.75 });
	FontAsset(U"Menu")(U"EXIT").drawAt(m_exitButton.center(), ColorF{ 0.75 });
}
