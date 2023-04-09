# pragma once

# include <Siv3D.hpp> // OpenSiv3D v0.6.8

enum class SceneState
{
	Title,
	Game,
	Result
};

// 共有するデータ
struct GameData
{
	// 直前のゲームのスコア
	Optional<int32> lastGameScore;
	
};

inline String Format_uint32_Size(uint32 byte)
{
	const String unitList[] = {
		U"KB",
		U"MB",
		U"GB",
		U"TB"
	};
	int i = 0;
	
	if (byte < 1000)
	{
		return Format(byte) + U"バイト";
	}
	while (true)
	{
		if (byte < 1000000)
		{
			// double型のbyteを1000で割ってから、小数点第二位で丸めるために100をかけてから丸めて100で割る
			// 実際は簡略化して10で割ってから丸める
			return Format(round(double(byte) / 10) / 100) + unitList[i];
		}
		byte /= 1000;
		i++;
	}
}

template <typename T>
inline Optional<T> AccessArray(const Array<T>& array, size_t index)
{
	if (index < array.size())
	{
		return Optional<T>(array[index]);
	}
	else
	{
		return none;
	}
}

inline JSON exceptionConfig;

inline JSON symbolConfig;

inline Array<String> ExcTypeList;

using App = SceneManager<SceneState, GameData>;
