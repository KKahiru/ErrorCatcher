# include "Game.hpp"

realizedExc::realizedExc(StringView type, uint16 lineNum)
{
	JSON config = exceptionConfig[type];
	this->lineNum = lineNum;
	m_type = type;
	m_display_name = config[U"display_name"].get<String>();
	m_speed = config[U"speed"].get<float>();
	m_damage = config[U"speed"].get<float>();
	m_color = s3d::Color(config[U"color"].get<String>());
}

realizedExc::realizedExc()
	:realizedExc(U"NPE", 0) {}

StringView realizedExc::DisplayName() const
{
	return m_display_name;
}

float realizedExc::Speed() const
{
	return m_speed;
}

float realizedExc::Damage() const
{
	return m_damage;
}

Color realizedExc::Color() const
{
	return m_color;
}

Game::Game(const InitData& init)
	: IScene{ init }
{
	fallenExcList.push_back(realizedExc(U"NPE", 0));
	const JSON symbols = symbolConfig[0];
	for (auto& lineObject : symbols)
	{
		symbolDurability.push_back(Array<uint8>());
		for (size_t i = 0; i < lineObject.value.size(); i++)
		{
			symbolDurability[Parse<uint16>(lineObject.key)].push_back(3);
		}
	}
	symbolHeight = lineHeight * symbol * 0.9;
	symbolTopMargin = lineHeight * symbol - symbolHeight;
}

void Game::update()
{
	// 戻るボタンの処理
	if (backButton.leftReleased()) changeScene(SceneState::Title);
	// カーソルがはみ出ないようにする処理
	{
		if (Cursor::Pos().x < 5)
		{
			Cursor::SetPos(5, Cursor::Pos().y);
		}
		else if (Cursor::Pos().x > SceneWidth - 5)
		{
			Cursor::SetPos(SceneWidth - 5, Cursor::Pos().y);
		}
		if (Cursor::Pos().y < 5)
		{
			Cursor::SetPos(Cursor::Pos().x, 5);
		}
		else if (Cursor::Pos().y > SceneHeight - 5)
		{
			Cursor::SetPos(Cursor::Pos().x, SceneHeight - 5);
		}
	}
	
	if (not isExplaining)
	{
		// 0.1秒間隔で例外の生成処理を行う
		generateTick += Scene::DeltaTime();
		while (generateTick >= 0.1)
		{
			generateTick -= 0.1;
			if (RandomBool( 0.15 + brokenness / 60))
			{
				const String type = ExcTypeList[Random(ExcTypeList.size() - 1)];
				fallenExcList.push_back(realizedExc(type, Random(maxLine)));
			}
		}
		
		//
		// 例外の更新作業
		//
		for (long int i = fallenExcList.size() - 1; i >= 0; i--)
		{
			realizedExc& item = fallenExcList[i];
			item.invasionDegree += Scene::DeltaTime() * item.Speed();
			const uint16 leftX = leftMargin * (item.lineNum + 1) + lineWidth * item.lineNum;
			const uint16 topY = topMargin  + lineHeight * item.invasionDegree;
			const size_t yIndex = (1 - item.invasionDegree) / symbol;
			const auto optionalDurabilityLine = AccessArray(symbolDurability, yIndex);
			
			size_t xIndex = 0;
			
			Optional<uint8> optionalDurability = none;
			
			// 破壊確認
			if (optionalDurabilityLine)
			{
				// 0 以上 optionalDurabilityLine->size() *未満*の整数
				xIndex = size_t(double(item.lineNum) / maxLine * optionalDurabilityLine->size());
				// optionalDurabilityLine->size()以上になった場合の処理
				if (xIndex >= optionalDurabilityLine->size())
				{
					xIndex = optionalDurabilityLine->size() - 1;
				}
				optionalDurability = AccessArray<uint8>( *optionalDurabilityLine, xIndex);
			}
			
			if (Circle{ Arg::topLeft(leftX, topY), lineWidth / 2 }.leftClicked())
			{
				// クリック時の処理
				brokenness += item.Damage();
				fallenExcList.remove_at(i);
			} else if (optionalDurability and *optionalDurability != 0)
			{
				//
				// シンボルに当たった時の処理
				//
				symbolDurability[yIndex][xIndex] --;
				
				brokenness += item.Damage() * 2;
				// 爆発エフェクトの追加
				effect.add([item, leftX, topY, lineWidth = this->lineWidth, explosionGIF = this->explosionGIF](double t)
				{
					explosionGIF.getFrame(t).resized(lineWidth).draw(leftX, topY);
					return (t < 1.0);
				});
				const double pan = double(item.lineNum * 2) / maxLine - 1;
				explosionAudio.playOneShot(0.5, pan);
				fallenExcList.remove_at(i);
				
				if (yIndex == 0 and symbolDurability[0][0] == 0)
				{
					//
					// ジ・エンド…
					//
					for (long int j = fallenExcList.size() - 1; j >= 0; j--)
					{
						// 爆発エフェクトの追加
						realizedExc& item = fallenExcList[j];
						item.invasionDegree += Scene::DeltaTime() * item.Speed();
						const uint16 leftX = leftMargin * (item.lineNum + 1) + lineWidth * item.lineNum;
						const uint16 topY = topMargin  + lineHeight * item.invasionDegree;
						effect.add([item, leftX, topY, lineWidth = this->lineWidth, explosionGIF = this->explosionGIF](double t)
						{
							explosionGIF.getFrame(t).resized(lineWidth).draw(leftX, topY);
							return (t < 1.0);
						});
						const double pan = double(item.lineNum * 2) / maxLine - 1;
						// 小音量で再生する
						explosionAudio.playOneShot(0.1, pan);
						fallenExcList.remove_at(j);
					}
					effect.add([this](double t)
					{
						if (t > 1.5)
						{
							this->changeScene(SceneState::Title, 0.0s);
							return false;
						}
						return true;
					});
					isExplaining = true;
				}
				
			}
		}
	}
	//
	// 説明の遷移処理
	//
	switch (explanationState) {
		case 0:
			if (Circle{ Arg::topLeft(leftMargin, topMargin), lineWidth / 2 }.leftClicked())
			{
				explanationState ++;
				fallenExcList.pop_back();
			}
			break;
		case 1:
			if (MouseL.down())
			{
				explanationState ++;
				isExplaining = 0;
			}
		default:
			break;
	}
}

void Game::draw() const
{
	
	//
	// シンボルの描画
	//
	const JSON symbols = symbolConfig[0];
	for (const auto& lineObject : symbols)
	{
		const double symbolWidth = (SceneWidth - symbolLeftMargin) / lineObject.value.size() - symbolLeftMargin;
		const double y = SceneHeight - ( topMargin + (symbolHeight + symbolTopMargin) * (Parse<uint16>(lineObject.key) + 1) );
		for (const auto& itemObject : lineObject.value)
		{
			const JSON itemValue = itemObject.value;
			const double x = symbolLeftMargin + (symbolWidth + symbolLeftMargin) * Parse<uint16>(itemObject.key);
			const uint8 durability = symbolDurability[Parse<uint16>(lineObject.key)][Parse<uint16>(itemObject.key)];
			
			if (durability)
			{
				RectF box{x, y, symbolWidth, symbolHeight};
				ColorF color;
				switch (durability) {
					case 3:
						color = Palette::Green;
						break;
					case 2:
						color = Palette::Orange;
						box.moveBy(Periodic::Sine1_1(1) * 5, Periodic::Sine1_1(1.5, Scene::Time()) * 5);
						break;
					case 1:
						color = Palette::Red;
						box.moveBy(Periodic::Sine1_1(0.5) * 7, Periodic::Sine1_1(1, Scene::Time()) * 7);
						break;
					default:
						color = Palette::Gray;
						break;
				}
				box.draw(color);
				FontAsset(U"Game.SymbolName")(itemValue.get<String>()).drawAt(box.center());
			}
		}
	}
	
	//
	// 例外の描画
	//
	for (auto i : step(fallenExcList.size())) {
		const realizedExc& item = fallenExcList[i];
		const uint16 leftX = leftMargin * (item.lineNum + 1) + lineWidth * item.lineNum;
		const uint16 topY = topMargin  + lineHeight * item.invasionDegree;
		
		Circle{ Arg::topLeft(leftX, topY), lineWidth / 2 }.draw(item.Color());
		FontAsset(U"Game.ExcName")(item.DisplayName())
			.drawAt(leftX + lineWidth / 2, topY + lineWidth / 2, Palette::Black);
	}
	
	//
	// 説明の描画
	//
	{
		uint16 x, y, dx, dy, w, h;
		switch (explanationState) {
			case 0:
				x = lineWidth + leftMargin * 2;
				y = topMargin * 2;
				dx = SceneWidth - leftMargin;
				dy = SceneHeight - topMargin;
				w = dx - x;
				h = dy - y;
				
				FontAsset(U"Game.Explanation")
					(U"←はソフトウェアに発生したエラー\n（専門用語では「例外」）であり、\n上から降ってきます。\nクリックすると取り除く事ができます。\n（エラーをクリックしてください）")
					.draw(Rect{ x, y, w, h });
				break;
			case 1:
				dx = SceneWidth - leftMargin;
				x = leftMargin;
				w = SceneWidth - leftMargin - x;
				dy = SceneHeight * 0.8 - topMargin;
				h = FontAsset(U"Game.Explanation").fontSize() * 8;
				y = dy - h;
				
				FontAsset(U"Game.Explanation")
				(U"↓の四角と先程のエラーが当たると爆発し、ダメージが入ります。\n特に一番下の「サーバー」は最優先で守る必要があり\nサーバーを破壊されるとゲームオーバーになります。\n（クリックしてゲーム開始！）")
				.draw(Rect{ x, y, w, h });
			default:
				break;
		}
	}
	effect.update();
	if (not isExplaining)
	{
		backButton.rounded(5).draw(ColorF{ 0, 0.5 }).drawFrame();
		FontAsset(U"Game.Back")(U"戻る").drawAt(backButton.center(), Palette::White);
	}
}

