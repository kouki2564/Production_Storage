#pragma once
#include "ObjectBase.h"
class Stage :
	public ObjectBase
{
public:
	Stage();
	~Stage();
	/// <summary>
	/// 初期化処理
	/// </summary>
	void Init(std::string name, VECTOR pos, VECTOR dir);

	void InitStage(std::string name, int handle);

	/// <summary>
	/// キャラ周り全体の更新
	/// </summary>
	void Update();

	/// <summary>
	/// 最終的な更新内容の反映
	/// </summary>
	void ApplyUpdating(){}

	void InitTitleAnimation() {}

	void SetWeapon(int num) {}

	void SetStageHandle(int handle);

	void SetChaseTarget(VECTOR targetPos) {}

	bool GetIsHitAttack(Collider& colOther) { return false; }

	bool SetDamagePoint(float damagePoint, bool isStan, bool isPowKnock) { return false; }

	void SetUpgrade(int paramNum) {}

	/// <summary>
	/// 使用しないモデルフレームの設定
	/// </summary>
	void SetNoUseFrame(int frame) { m_collider.noUseFrame.push_back(frame); }

	void ResetNoUseFrame() { m_collider.noUseFrame.clear(); }

	/// <summary>
	/// ノックバックベクトルの設定（knockBackだけほかで使うため、セッター配置）
	/// </summary>
	/// <param name="vec">ノックバックベクトル</param>
	void SetKnockVec(VECTOR vec) { m_physics.SetKnockVec(vec); }

	/// <summary>
	/// コライダー情報の受け渡し
	/// </summary>
	/// <returns>コライダー</returns>
	Collider GetCollider() { return m_collider; }


	// ハンドル取得
	const int GetHandle() const { return m_handle; }

	void SetPosAndDir(VECTOR pos, VECTOR dir);

protected:
};

