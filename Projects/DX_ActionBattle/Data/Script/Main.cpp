#include <DxLib.h>
#include <EffekseerForDXLib.h>
#include "SceneManager.h"
#include "GameData.h"
#include "HandleManager.h"
#include <memory>

// プログラムは WinMain から始まります
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{

	// システムバータイトル
	SetWindowTextA("Tactical Power");

#ifdef _DEBUG
	// コンソール表示設定
	AllocConsole();                                        // コンソール
	FILE* out = 0; freopen_s(&out, "CON", "w", stdout); // stdout
	FILE* in = 0; freopen_s(&in, "CON", "r", stdin);   // stdin

#endif // _DEBUG
	// ログ出力設定
	SetOutApplicationLogValidFlag(false);

	// ウインドウモード設定
	ChangeWindowMode(true);

	// 画面サイズ設定
	SetGraphMode(GameData::kScreenWidth, GameData::kScreenHeight, 32);
	

	// DirectX11を使用するようにする。(DirectX9も可、一部機能不可とのこと)
	// Effekseerを使用するには必ず設定する。
	SetUseDirect3DVersion(DX_DIRECT3D_11);


	if (DxLib_Init() == -1)		// ＤＸライブラリ初期化処理
	{
		return -1;			// エラーが起きたら直ちに終了
	}

	// 裏画面に描画を行う
	SetDrawScreen(DX_SCREEN_BACK);
	
	// Effekseerを初期化する。
	// 引数には画面に表示する最大パーティクル数を設定する。
	if (Effekseer_Init(8000) == -1)
	{
		DxLib::DxLib_End();
		return -1;
	}

	// フルスクリーンウインドウの切り替えでリソースが消えるのを防ぐ。
	// Effekseerを使用する場合は必ず設定する。
	SetChangeScreenModeGraphicsSystemResetFlag(FALSE);

	// DXライブラリのデバイスロストした時のコールバックを設定する。
	// ウインドウとフルスクリーンの切り替えが発生する場合は必ず実行する。
	// ただし、DirectX11を使用する場合は実行する必要はない。
	Effekseer_SetGraphicsDeviceLostCallbackFunctions();

	SetLightEnable(FALSE);

	HandleManager::Instance().UseFont(FontName::BOLD_30);

	// Zバッファを有効にする。
	// Effekseerを使用する場合、2DゲームでもZバッファを使用する。
	SetUseZBuffer3D(TRUE);

	// Zバッファへの書き込みを有効にする。
	// Effekseerを使用する場合、2DゲームでもZバッファを使用する。
	SetWriteZBuffer3D(TRUE);

	std::unique_ptr<SceneManager> pManager = std::make_unique<SceneManager>();
	pManager->Init();


	// int timer = 0;
	// ゲームループ
	while (ProcessMessage() != -1)
	{
		// フレーム開始時刻を覚えておく
		LONGLONG start = GetNowHiPerformanceCount();

		// 描画を行う前に画面をクリアにする
		ClearDrawScreen();

		// ゲームの処理
		pManager->Update();

		// 画面が切り替わるのを待つ
		ScreenFlip();

		// escキーでゲーム終了
		if (pManager->GetisGameEnd() || CheckHitKey(KEY_INPUT_ESCAPE))
		{
			break;
		}

		// FPSを60に固定
		while (GetNowHiPerformanceCount() - start < 16667)
		{
			// 16,66ミリ秒(16667マイクロ秒)経過まで待つ
		}

#ifdef _DEBUG
		// FPSチェック
		auto count = GetNowHiPerformanceCount();
		auto fps = static_cast<float>(1000000 / (count - start));
		if (CheckHitKey(KEY_INPUT_Q))
		{
			printf("FPS : %f\n", fps);
		}
#endif // _DEBUG
	}


	// Effekseerを終了する。
	Effkseer_End();

	DxLib::DxLib_End();				// ＤＸライブラリ使用の終了処理

	// コンソール解放
#ifdef _DEBUG
	fclose(out); fclose(in); FreeConsole();
#endif // _DEBUG

	return 0;				// ソフトの終了 
}