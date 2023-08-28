# include "Common.hpp"
# include "Title.hpp"
# include "Game.hpp"

void Main()
{
	Window::SetTitle(U"ExceptionCatcher");
	Scene::Resize(BaseSceneSize);
	Scene::SetResizeMode(ResizeMode::Actual);
	
	FontAsset::Register(U"Title.TitleFont", FontMethod::SDF, 50, Typeface::Bold);
	FontAsset::Register(U"Menu", FontMethod::MSDF, 40, Typeface::Medium);
	FontAsset::Register(U"GameScore", 30, Typeface::Light);
	FontAsset::Register(U"Game.ExcName", 20, Typeface::Light);
	FontAsset::Register(U"Game.SymbolName", 20, Typeface::Light);
	FontAsset::Register(U"Game.Explanation", 28);
	FontAsset::Register(U"Game.Back", 32);
	AudioAsset::Register(U"Brick", GMInstrument::Woodblock, PianoKey::C5, 0.2s, 0.1s);
	
	exceptionConfig = JSON::Load(Resource(U"resource/config/exception_list.json"));
	symbolConfig = JSON::Load(Resource(U"resource/config/symbol_list.json"));
	
	for (const auto& object : exceptionConfig)
	{
		ExcTypeList.push_back(object.key);
	}

	App manager;
	manager.add<Title>(SceneState::Title);
	manager.add<Game>(SceneState::Game);
	
	while (System::Update()) {
		// 実際のシーンのサイズを取得する
		const Size currentSceneSize = Scene::Size();
		// 何倍に拡大すればいいかを計算する
		const double scale = Min(currentSceneSize.x / SceneWidth, currentSceneSize.y / SceneHeight);
		// 描画とマウス座標にスケーリングを適用する
		const Transformer2D scaling{ Mat3x2::Scale(scale), TransformCursor::Yes };
		if (not manager.update()) {
			break;
		}
	}
}

//
// = アドバイス =
// アプリケーションをビルドして実行するたびにファイルアクセス許可のダイアログが表示されるのを避けたい場合、
// プロジェクトのフォルダを ユーザ/(ユーザ名)/アプリケーション/ などに移動させてください。
// Web カメラ、マイク使用時の許可ダイアログを消すことはできません。
//
