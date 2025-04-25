#pragma once
#include "SceneBase.h"

class WeaponBase;

struct bg
{
    // 座標
	float posX;
	float posY;
    
	// 円の半径
    int range;

    // 移動スピード
    float speed;
};

class TitleScene :
    public SceneBase
{
public:
    TitleScene();
    ~TitleScene();

    void InitScene();
    void UpdateScene();

private:
    // 画像データの準備
    void InitImage();
    void ObjectsUpdate();

    void CollisionUpdate();

    void CameraUpdate();

	void BackgroundUpdate();

    /// <summary>
    /// スタート文の点滅
    /// </summary>
    /// <param name="value">アルファ変動値</param>
    void BlinkStartString(int value);

    // タイトルロゴ
    int m_titleHandle;
    
    // 点滅フラグ
    bool m_isBlink;
    // スタート押せのアルファ値
    int m_startAlpha = 0;

	// 背景のやつ
	bg m_bg[10];

	// sin波の移動
	float m_sinMove1 = 0;
	float m_sinMove2 = 0;

    // 基準線の色
    int m_bgColorR;
    int m_bgColorG;
	int m_bgColorB;

    // 配置するオブジェクト
    std::map<std::string, std::unique_ptr<ObjectBase>> m_objects;

	std::map<std::string, std::unique_ptr<WeaponBase>> m_weapons;

    std::map<std::string, std::unique_ptr<Stage>> m_stage;

    // 武器たちの中心座標
	VECTOR m_centerPos = VZero();
	// 武器たちの基準座標
    VECTOR m_weaponPos[3];
	// 武器たちの回転角度
    float m_weaponRot[3] = { 0.0f, 2 / 3 * DX_PI_F, 4 / 3 * DX_PI_F };

    // 武器たちの回転フラグ
	bool m_isLockPos = false;
};

