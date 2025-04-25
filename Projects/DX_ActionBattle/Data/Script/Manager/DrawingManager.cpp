#include <cassert>
#include "DrawingManager.h"
#include "ToonShader.h"
#include "GameData.h"

void DrawingManager::RegisterModel(std::string name, int handle, VECTOR pos, VECTOR dir, VECTOR scale)
{
	ModelData initData;
	initData.handle = MV1DuplicateModel(handle);
	initData.pos = pos;
	initData.dir = dir;
	initData.scale = scale;
	m_models.insert(std::make_pair(name, initData));
	MV1SetPosition(m_models[name].handle, m_models[name].pos);
	MV1SetRotationXYZ(m_models[name].handle, VGet(0, atan2(-m_models[name].dir.x, -m_models[name].dir.z), 0));
	MV1SetScale(m_models[name].handle, m_models[name].scale);

}

void DrawingManager::RegisterMapModel(std::string name, int handle, VECTOR pos, VECTOR dir, VECTOR scale)
{
	// namaeが重複していたら消す
	for (auto& t : m_models)
	{
		if (t.first == name)
		{
			m_models.erase(name);
			break;
		}
	}

	ModelData initData;
	initData.handle = handle;
	initData.pos = pos;
	initData.dir = dir;
	initData.scale = scale;
	m_models.insert(std::make_pair(name, initData));
	MV1SetPosition(m_models[name].handle, m_models[name].pos);
	MV1SetRotationXYZ(m_models[name].handle, VGet(0, atan2(-m_models[name].dir.x, -m_models[name].dir.z), 0));
	MV1SetScale(m_models[name].handle, m_models[name].scale);
}

void DrawingManager::RegisterOtherModel(std::string name, VECTOR pos, ModelForm form, int difColor, int spcColor, float radius, bool isFill, VECTOR pos2)
{
	ModelData initData;
	initData.handle = 0;
	initData.pos = pos;
	initData.form = form;
	initData.difColor = difColor;
	initData.spcColor = spcColor;
	initData.radius = radius;
	initData.isFill = isFill;
	initData.pos2 = pos2;
	m_models.insert(std::make_pair(name, initData));
}

void DrawingManager::RegisterWeaponModel(std::string weaponName, int handle, VECTOR scale, VECTOR rot, std::string ownerModelName, std::string frameName)
{
	if (weaponName != "NONE")
	{
		for (auto& t : m_models)
		{
			// 所持者側で武器ポインタを切り替えるために
			// 武器ハンドルが変わる
			// →１度既に登録されているデータを消して作り直す
			if (t.first == weaponName)
			{
				m_models.erase(weaponName);
				break;
			}
		}
		// swap試してみる
		// 持たせる座標の確定
		const TCHAR* ownerFrameName = frameName.c_str();
		int frameNum = MV1SearchFrame(m_models[ownerModelName].handle, ownerFrameName);

		ModelData initData;
		initData.ornerName = ownerModelName;
		initData.setFrameNum = frameNum;
		initData.handle = handle;
		initData.scale = scale;
		initData.rot = rot;
		m_models.insert(std::make_pair(weaponName, initData));
		MV1SetScale(m_models[weaponName].handle, m_models[weaponName].scale);
		UpdateWeaponModelData(weaponName);
	}
}

void DrawingManager::DeleteModel(std::string name)
{
	for (auto& t : m_models)
	{
		if (t.first == name)
		{
			MV1DeleteModel(t.second.handle); // メモリ解放
			m_models.erase(name);
			return;
		}
	}
}

void DrawingManager::DeleteAllModel()
{
	for (auto& t : m_models)
	{
		// メモリ解放
		MV1DeleteModel(t.second.handle);
	}
	m_models.clear();
}

void DrawingManager::UpdateModelData(std::string name, VECTOR pos, VECTOR dir, VECTOR scale)
{
	if (m_models[name].handle != -1)
	{
		m_models[name].pos = pos;
		if (dir.x != 0 || dir.y != 0 || dir.z != 0)
			m_models[name].dir = dir;
		if (scale.x != -1 || scale.y != -1 || scale.z != -1)
		{
			m_models[name].scale = scale;
			m_models[name].radius = scale.x;
		}
		if (m_models[name].form == ModelForm::MODEL)
		{
			MV1SetPosition(m_models[name].handle, m_models[name].pos);
			MV1SetRotationXYZ(m_models[name].handle, VGet(0, atan2(-m_models[name].dir.x, -m_models[name].dir.z), 0));
			MV1SetScale(m_models[name].handle, m_models[name].scale);

			AnimationUpdate(name);
		}
	}
}

void DrawingManager::UpdateWeaponModelData(std::string name)
{
	if (name != "NONE")
	{
		if (m_models[name].handle != -1)
		{
			VECTOR AttachSuruFrame1Position;
			MATRIX TransMat;
			MATRIX AttachSareruFrame10Mat;
			MATRIX MixMatrix;

			// アタッチするモデルの MV1SetMatrix の設定を無効化する
			MV1SetMatrix(m_models[name].handle, MGetIdent());

			// アタッチするモデルのフレームの座標を取得する
			AttachSuruFrame1Position = MV1GetFramePosition(m_models[name].handle, 0);

			// アタッチするモデルをフレームの座標を原点にするための平行移動行列を作成
			TransMat = MGetTranslate(VScale(AttachSuruFrame1Position, -1.0f));

			auto ownerName = m_models[name].ornerName;

			// アタッチされるモデルのフレームの行列を取得
			AttachSareruFrame10Mat = MV1GetFrameLocalWorldMatrix(m_models[ownerName].handle, m_models[name].setFrameNum);

			// アタッチするモデルをフレームの座標を原点にするための平行移動行列と、
			// アタッチされるモデルのフレームの行列を合成
			MixMatrix = MMult(MGetScale(m_models[name].scale),				// スケール値調整
						MMult(MGetRotX(m_models[name].rot.x),				// 角度調整X
						MMult(MGetRotY(m_models[name].rot.y),				// 角度調整Y
						MMult(MGetRotZ(m_models[name].rot.z),				// 角度調整Z
						MMult(TransMat, AttachSareruFrame10Mat)))));		// 移動量設定
			// 合成した行列をアタッチするモデルにセット
			MV1SetMatrix(m_models[name].handle, MixMatrix);

#ifdef _DEBUG
			// 武器座標の獲得
			auto vec = VTransform(VZero(), MixMatrix);
			// MV1SetPosition(m_models[name].handle, vec);
			DrawFormatString(0, 10, 0xffff00, "%f, %f, %f", vec.x, vec.y, vec.z);
#endif // _DEBUG
		}
	}
}

void DrawingManager::Draw()
{
	// ステージモデル用のシャドウマップへの描画の準備
	ShadowMap_DrawSetup(m_shadowHandle);
	for (auto& t : m_models)
	{
		// ステージモデル用のシャドウマップへステージモデルの描画
		if (t.second.isDraw && t.second.isShadow)
		{
			// 影のサイズスケール値
			float ShadowScale = 1.0f;
			// Y座標が0以下の場合は影のサイズを1.0fに固定
			if (t.second.pos.y <= 0.0f)
			{
				ShadowScale = 1.0f;
			}
			// それ以外の場合はY座標が大きいほど影のサイズが小さくなる
			else
			{
				// 0.01fは影の調整用、1.0fはyがゼロでスケール1.0fになるように 兼 ゼロ除算避け
				ShadowScale = 1 / (t.second.pos.y * 0.01f + 1.0f);
			}

			switch (t.second.form)
			{
			case ModelForm::MODEL:
				MV1SetMaterialOutLineWidth(t.second.handle, 0, 1000000000.0f);
				MV1SetMaterialOutLineColor(t.second.handle, 0, { 1.0f, 1.0f, 1.0f, 1.0f });
				// 元のスケール値を保存
				auto scale = t.second.scale;
				// モデルスケール値を影のサイズに変更
				MV1SetScale(t.second.handle, VScale(t.second.scale, ShadowScale));
				MV1DrawModel(t.second.handle);
				// スケール値を元に戻す
				MV1SetScale(t.second.handle, scale);
				break;
			case ModelForm::SPHERE:
				DrawSphere3D(t.second.pos, t.second.radius * ShadowScale, 8, t.second.difColor, t.second.spcColor, t.second.isFill);
				break;
			case ModelForm::CAPSULE:
				DrawCapsule3D(t.second.pos, t.second.pos2, t.second.radius * ShadowScale, 8, t.second.difColor, t.second.spcColor, t.second.isFill);
				break;
			}
		}

	}
	// ステージモデル用のシャドウマップへの描画を終了
	ShadowMap_DrawEnd();


	// シャドウマップの反映開始
	SetUseShadowMap(0, m_shadowHandle);

	for (auto& t : m_models)
	{
		// 描画
		if (t.second.isDraw)
		{

			switch (t.second.form)
			{
			case ModelForm::MODEL:
				MV1DrawModel(t.second.handle);
				break;
			case ModelForm::SPHERE:
				DrawSphere3D(t.second.pos, t.second.radius, 8, t.second.difColor, t.second.spcColor, t.second.isFill);
				break;
			case ModelForm::CAPSULE:
				DrawCapsule3D(t.second.pos, t.second.pos2, t.second.radius, 8, t.second.difColor, t.second.spcColor, t.second.isFill);
				break;
			}
		}
	}
	// 反映終了
	SetUseShadowMap(0, -1);
}

void DrawingManager::ChangePopModelFrame(std::string name, int frameNum, bool isPop)
{
	if (m_models[name].handle != -1)
	{
		// モデルフレームの表示切り替え
		MV1SetFrameVisible(m_models[name].handle, frameNum, isPop);
	}
}

void DrawingManager::SetFrameOpacityRate(std::string name, int frameNum, float rate)
{
	// シャドウ諸々やってるから透明度を変えたくない
	/*if (frameNum == -1)
	{
		m_models[name].alpha = 255 * rate;
	}
	else */
	if (m_models[name].handle != -1)
	{
		MV1SetFrameOpacityRate(m_models[name].handle, frameNum, rate);
	}
}

void DrawingManager::SetStageDoor(bool isRight, bool isFront, bool isLeft, bool isBack)
{
	if (m_models["Stage"].handle != -1)
	{
		// 右
		if (isRight)
		{
			MV1SetFrameVisible(m_models["Stage"].handle, 110, true);
			auto err = MV1SetFrameVisible(m_models["Stage"].handle, 114, false);
			if (err == -1)
			{
				printf("err\n");
			}
		}
		else
		{
			auto err = MV1SetFrameVisible(m_models["Stage"].handle, 110, false);
			MV1SetFrameVisible(m_models["Stage"].handle, 114, true);
			if (err == -1)
			{
				printf("err\n");
			}
		}
		// 前
		if (isFront)
		{
			MV1SetFrameVisible(m_models["Stage"].handle, 119, true);
			auto err = MV1SetFrameVisible(m_models["Stage"].handle, 123, false);
			if (err == -1)
			{
				printf("err\n");
			}
		}
		else
		{
			auto err = MV1SetFrameVisible(m_models["Stage"].handle, 119, false);
			MV1SetFrameVisible(m_models["Stage"].handle, 123, true);
			if (err == -1)
			{
				printf("err\n");
			}
		}
		// 左
		if (isLeft)
		{
			MV1SetFrameVisible(m_models["Stage"].handle, 101, true);
			auto err = MV1SetFrameVisible(m_models["Stage"].handle, 105, false);
			if (err == -1)
			{
				printf("err\n");
			}
		}
		else
		{
			auto err = MV1SetFrameVisible(m_models["Stage"].handle, 101, false);
			MV1SetFrameVisible(m_models["Stage"].handle, 105, true);
			if (err == -1)
			{
				printf("err\n");
			}
		}
		// 後ろ
		if (isBack)
		{
			MV1SetFrameVisible(m_models["Stage"].handle, 128, true);
			auto err = MV1SetFrameVisible(m_models["Stage"].handle, 132, false);
			if (err == -1)
			{
				printf("err\n");
			}
		}
		else
		{
			auto err = MV1SetFrameVisible(m_models["Stage"].handle, 128, false);
			MV1SetFrameVisible(m_models["Stage"].handle, 132, true);
			if (err == -1)
			{
				printf("err\n");
			}
		}
	}
}

void DrawingManager::CallAnimation(std::string modelName, std::string animationName, int animationFrame)
{
	const TCHAR* name = animationName.c_str();

	// name = MV1GetAnimName(m_models[modelName].handle, 3);

	// 変更先のアニメーション番号を獲得
	int index = MV1GetAnimIndex(m_models[modelName].handle, name);
	// 現行のアニメーション番号を獲得
	auto nowindex = MV1GetAttachAnim(m_models[modelName].handle, m_models[modelName].animation.nowAttach);
	// すでに変更予定のアニメーション番号を獲得
	auto nextindex = MV1GetAttachAnim(m_models[modelName].handle, m_models[modelName].animation.nextAttach);
	assert(index != -1);
	// アニメーション変更中でないことの確認
	if (nowindex == nextindex)
	{
		// 変更先が同アニメーション出ないことを確認
		if (index != nextindex)
		{
			m_models[modelName].animation.nextAttach = MV1AttachAnim(m_models[modelName].handle, index, -1);
			m_models[modelName].animation.animaNextPlayFrame = animationFrame;
		}
		// 同アニメーションの場合は速度だけ更新
		m_models[modelName].animation.animaPlayFrame = animationFrame;
	}
}

void DrawingManager::CallTransAnimation(std::string modelName, std::string animationName, int animationFrame)
{
	const TCHAR* name = animationName.c_str();

	// name = MV1GetAnimName(m_models[modelName].handle, 3);

	// 変更先のアニメーション番号を獲得
	int index = MV1GetAnimIndex(m_models[modelName].handle, name);
	// 現行のアニメーション番号を獲得
	auto nowindex = MV1GetAttachAnim(m_models[modelName].handle, m_models[modelName].animation.nowAttach);
	// すでに変更予定のアニメーション番号を獲得
	auto nextindex = MV1GetAttachAnim(m_models[modelName].handle, m_models[modelName].animation.nextAttach);
	// 移行先のアニメーション番号を獲得
	auto transindex = MV1GetAttachAnim(m_models[modelName].handle, m_models[modelName].animation.transAttach);
	assert(index != -1);
	// 移行先のアニメーションが存在しない場合のみ
	if (transindex == -1)
	{
		m_models[modelName].animation.transAttach = MV1AttachAnim(m_models[modelName].handle, index, -1);
	}
	m_models[modelName].animation.animaTransPlayFrame = animationFrame;
}

void DrawingManager::ChangeSpeedAnimation(std::string modelName, int animationFrame)
{
	m_models[modelName].animation.animaPlayFrame = animationFrame;
}

std::string DrawingManager::GetPlayingAnimationName(std::string modelName)
{
	auto handle = m_models[modelName].handle;
	auto attach = m_models[modelName].animation.nowAttach;

	auto index = MV1GetAttachAnim(handle, attach);

 	if (index != -1)
	{
		auto indexname = MV1GetAnimName(handle, index);

		return indexname;
	}
	else
	{
		return "NONE";
	}
}

void DrawingManager::AnimationUpdate(std::string& modelName)
{
	// assert(m_modelHandle != -1);
	// アニメーション遷移時
	if (m_models[modelName].animation.nowAttach != m_models[modelName].animation.nextAttach)
	{
		RateAnimation(modelName);
	}
	// アニメーション継続時
	else
	{
		// 現在使用中のアニメーションの終了フレーム
		float TotalTime = MV1GetAttachAnimTotalTime(m_models[modelName].handle, m_models[modelName].animation.nowAttach);

		float deltaTime = TotalTime / m_models[modelName].animation.animaPlayFrame;
		int indexnow = MV1GetAttachAnim(m_models[modelName].handle, m_models[modelName].animation.nowAttach);
		int indexnext = MV1GetAttachAnim(m_models[modelName].handle, m_models[modelName].animation.nextAttach);
		int indextrans = MV1GetAttachAnim(m_models[modelName].handle, m_models[modelName].animation.transAttach);
		// アニメフレーム進行
		m_models[modelName].animation.animaFrame += deltaTime;

		// アニメフレーム更新
		MV1SetAttachAnimTime(m_models[modelName].handle, m_models[modelName].animation.nowAttach, m_models[modelName].animation.animaFrame);

		// アニメーションが1周したとき
		if (m_models[modelName].animation.animaFrame >= deltaTime * m_models[modelName].animation.animaPlayFrame)
		{
			m_models[modelName].animation.animaFrame = 0.0f;
			// 移行するアニメーションが存在する場合
			if (m_models[modelName].animation.transAttach != -1 )
			{
				m_models[modelName].animation.nextAttach = m_models[modelName].animation.transAttach;
				m_models[modelName].animation.animaNextPlayFrame = m_models[modelName].animation.animaTransPlayFrame;
				// 移行後はtransは削除
				m_models[modelName].animation.transAttach = -1;
			}
		}
	}
}

void DrawingManager::RateAnimation(std::string& modelName)
{
	// 初期アニメーション
	if (m_models[modelName].animation.nowAttach == -1)
	{
		m_models[modelName].animation.nowAttach = m_models[modelName].animation.nextAttach;
		m_models[modelName].animation.animaPlayFrame = m_models[modelName].animation.animaNextPlayFrame;
	}
	// それ以降
	else
	{
		if (m_models[modelName].animation.animaRate < 1.0f)
		{
			// アニメーション遷移
			m_models[modelName].animation.animaRate += 0.2f;
			auto detachRate = 1.0f - m_models[modelName].animation.animaRate;
			int indexnow = MV1GetAttachAnim(m_models[modelName].handle, m_models[modelName].animation.nowAttach);
			int indexnext = MV1GetAttachAnim(m_models[modelName].handle, m_models[modelName].animation.nextAttach);
			int indextrans = MV1GetAttachAnim(m_models[modelName].handle, m_models[modelName].animation.transAttach);
			MV1SetAttachAnimBlendRate(m_models[modelName].handle, m_models[modelName].animation.nowAttach, detachRate);
			MV1SetAttachAnimBlendRate(m_models[modelName].handle, m_models[modelName].animation.nextAttach, m_models[modelName].animation.animaRate);
			// MV1DrawModel(m_models[modelName].handle);
		}
		else
		{
			auto rate = m_models[modelName].animation.animaRate;
			auto index = MV1GetAttachAnim(m_models[modelName].handle, m_models[modelName].animation.nowAttach);
			auto name = MV1GetAnimName(m_models[modelName].handle, index);
			// 直前のアニメーションインデックスを消去
			MV1DetachAnim(m_models[modelName].handle, m_models[modelName].animation.nowAttach);
			// 次のインデックスを付与
			m_models[modelName].animation.nowAttach = m_models[modelName].animation.nextAttach;
			m_models[modelName].animation.animaPlayFrame = m_models[modelName].animation.animaNextPlayFrame;
			m_models[modelName].animation.animaRate = 0;
			m_models[modelName].animation.animaFrame = 0;
			// 最後に次のアニメーションの０フレーム目を再生しておくことでブレンドアニメーションを断ち切る
			MV1SetAttachAnimTime(m_models[modelName].handle, m_models[modelName].animation.nowAttach, m_models[modelName].animation.animaFrame);

#ifdef _DEBUG

			printf("\nアニメーション変更：%s\n", modelName.c_str());
			index = MV1GetAttachAnim(m_models[modelName].handle, m_models[modelName].animation.nowAttach);
			name = MV1GetAnimName(m_models[modelName].handle, index);
			printf("%s\n", name);

#endif // _DEBUG

		}
	}
}

//void DrawingManager::UpdateModelData(std::string& name, Model& modelData)
//{
//	m_models[name] = modelData;
//}
