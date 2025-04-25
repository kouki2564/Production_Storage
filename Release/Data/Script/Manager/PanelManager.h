#pragma once
#include <map>
#include <string>
#include <vector>
#include "DxLib.h"

struct ImageData
{
	// 画像のハンドル
	int handle;
	// 画像の設置中央座標
	float posX; float posY;
	// 画像サイズ
	float graphRangeX; float graphRangeY;
	// 画像の拡大率
	float scaleX;
	float scaleY;
	// 画像を描画するかどうか
	bool isDraw = true;
	// 画像のアルファ値
	int alpha = 255;
};

struct numData
{
	// 描画する桁ごとの数値
	std::vector<int> number;
	// 時間ごとに低下するアルファ値
	int decAlpha;
	// 描画する時間
	int time;
	// 描画する時間
	int timeMax;
	// 描画する座標
	VECTOR pos;
	// 時間ごとのスケール拡大値
	float scale;
	// カメラ方向
	VECTOR cameraDir;
};

class PanelManager
{
public:
	PanelManager();
	~PanelManager();

	//インスタンスを返す
	static PanelManager& Instance() {
		static PanelManager instance;
		return instance;
	}
	void sum() {}

	void PanelUpdate();

	// 画像の登録
	void RegisterImage(std::string name, int handle, float posX, float posY, float scaleX, float scaleY);
	
	// 画像の削除
	void DeleteImage(std::string name);

	void DeleteAllImage();

	void Draw();


	// 画像を描画するかどうか
	void SetIsDrawImage(std::string name, bool isDraw);

	// 画像のアルファ値を設定
	void SetAlpha(std::string name, int alpha);
	
	// 画像の座標を設定
	void UpdateImagePos(std::string name, float posX, float posY);

	// 画像の拡大率を設定
	void UpdateImageScale(std::string name, float scaleX, float scaleY);

	// 画像の座標を取得
	void GetPos(std::string name, float* posX, float* posY);

	// 画像のサイズを取得
	void GetSize(std::string name, float* rangeX, float* rangeY);

	// 画像の描画するかどうかのフラグを取得
	bool GetIsDrawImage(std::string name);

	/// <summary>
	/// 描画する数値の設定
	/// </summary>
	/// <param name="number">描画する数値</param>
	/// <param name="pos">描画座標</param>
	/// <param name="scale">描画最大拡大値</param>
	/// <param name="drawTime">描画フレーム</param>
	void SetNumber(int number, VECTOR pos, float scale, int drawTime, VECTOR cameraDir);


private:
	// 数値の描画
	void DrawNumber();

	// 画像の描画
	void DrawPanel();

	std::map<std::string, ImageData> m_imageData;

	int m_numberHandle[10];
	
	std::vector<numData> m_numberData;
};

