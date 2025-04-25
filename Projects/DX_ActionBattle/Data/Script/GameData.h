#pragma once
#include "DxLib.h"
#include <vector>
#include "DrawingManager.h"
#include "SoundManager.h"
#include "PanelManager.h"
#include "EffectManager.h"
#include <cassert>
#include <psapi.h>



enum
{
	UP,
	DOWN,
	RIGHT,
	LEFT
};

/// <summary>
/// ゲーム全体で使う定数
/// </summary>
namespace GameData
{
	//////////////
	// 基本設定 //
	//////////////

	// 画面幅
	constexpr int kScreenWidth = 1280;      //画面の横幅
	constexpr int kScreenHeight = 720;     //画面の縦幅
	// 半分サイズ
	constexpr int kScreenHalfWidth = kScreenWidth / 2;      //画面の横幅
	constexpr int kScreenHalfHeight = kScreenHeight / 2;     //画面の縦幅

	// ゲーム色深度
	constexpr int kColorDepth = 32;        //16 or 32

	//////////////////////
	// ゲームごとの設定 //
	//////////////////////
	// 全体シーン数
	constexpr int kSceneNum = 3;

	// ステージデータ
	constexpr int kStageMaxNum = 3;
	constexpr int kStageMaxLine = 5;
	constexpr int kStageMaxRow = 3;
	constexpr int kStagePos[kStageMaxLine][kStageMaxRow] =
	{
		{0, 2, 0},
		{0, 1, 0},
		{1, 1, 1},
		{1, 1, 1},
		{1, 1, 1}
	};
	constexpr int kTutorialMaxLine = 6;

	constexpr int kTutorialStageMaxLine = 6;
	constexpr int kTutorialStageMaxRow = 1;
	constexpr int kTutorialStagePos[kTutorialStageMaxLine][kTutorialStageMaxRow] =
	{
		{2},
		{1},
		{1},
		{1},
		{1},
		{1}
	};

	// 使用ボタン数
	constexpr int kButtonNum = 12;

	// サウンド数
	constexpr int kBGMNum = 0;
	constexpr int kSENum = 0;

	// プレイヤー通常攻撃回数
	constexpr int kAttackCountMax = 3;

	constexpr float kGravity = 4.0f;

	// フォントハンドル
	constexpr int kFontHandle = 0;
}

struct Debug
{
private:
	// メモリ使用量記録用
	unsigned long long m_memoryUsage = 0;
	unsigned long long m_lastMemoryUsage = 0;
	// 仮想メモリ使用量
	unsigned long long m_memoryWorking = 0;
	unsigned long long m_lastMemoryWorking = 0;
	// 経過時間記録用
	unsigned long long m_time = 0;
	unsigned long long m_lastTime = 0;
	

public:

	static Debug& Instance()
	{
		static Debug instance;
		return instance;
	}

	/// <summary>
	/// 仮想メモリ使用量を取得、表示
	/// </summary>
	/// <returns>差分の表示</returns>
	void PrintMemoryUsage()
	{
		PROCESS_MEMORY_COUNTERS pmc;
		if (GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc)))
		{
			m_memoryWorking = pmc.WorkingSetSize / 1024;
			printf("Working Set Size(RAM使用量) : %d KB\n", m_memoryWorking);

			m_lastMemoryWorking = m_memoryWorking - m_lastMemoryWorking;

			/*auto memoryRangeWorking = (pmc.WorkingSetSize / 1024) - m_memoryWorking;
			if (memoryRangeWorking > 10000)
			{
				printf("仮想メモリ増加量  : %d KB\n かなり増加中!!!\n", memoryRangeWorking);
			}*/

			m_memoryUsage = pmc.PagefileUsage / 1024;
			printf("Pagefile Usage (仮想メモリ使用量) : %d KB\n", m_memoryUsage);

			m_lastMemoryUsage = m_memoryUsage - m_lastMemoryUsage;

			/*m_memoryWorking = pmc.WorkingSetSize / 1024;
			m_memoryUsage = pmc.PagefileUsage / 1024;*/
			// return m_lastMemoryUsage;

			m_time = GetNowHiPerformanceCount() - m_lastTime;
			printf("前回計測からの経過時間 : %d ms\n", m_time);
			m_lastTime = GetNowHiPerformanceCount();

		}
		else
		{
			printf("メモリ取得失敗\n");
		}
	}
};