#include "Magic.h"
#include "Shot.h"
#include "Quaternion.h"
#include "EffectManager.h"
#include "DrawingManager.h"
#include "HandleManager.h"

namespace
{
	constexpr int kShotTime = 190;
	constexpr int kShotMax = 30;
}

Magic::Magic()
{ 
	kind = Weapon::MAGIC;
	m_spherePos = VGet(0, 10, 0);
	DrawingManager::Instance().RegisterOtherModel("MagicSphere", m_spherePos, ModelForm::SPHERE, 0x00a0a0, 0xffffff, 1.5f, true);
	DrawingManager::Instance().SetIsShadowModel("MagicSphere", false);
	EffectManager::Instance().RegisterEffect("Magic", HandleManager::Instance().GetEffectHandle(EffectName::MAGIC), m_spherePos, VZero());
	EffectManager::Instance().PlayEffect("Magic", m_spherePos, VZero());
	EffectManager::Instance().RegisterEffect("MagicCircle", HandleManager::Instance().GetEffectHandle(EffectName::CIRCLE), m_spherePos, VZero());
}

Magic::~Magic()
{
	MV1DeleteModel(handle);
	EffectManager::Instance().StopEffect("Magic");
	EffectManager::Instance().StopEffect("MagicCircle");
	DrawingManager::Instance().DeleteModel("MagicSphere");
	// 弾処理
	for (int i = 0; i < m_shots.size(); i++)
	{
		// 消去処理
		// モデルの削除
		m_shots[i]->DeleteModel();
		// vectorでeraseを行ったら空要素分自動で詰めて要素数も更新される
		m_shots.erase(m_shots.begin() + i);
		colliders.erase(colliders.begin() + i);
		// 詰めた分要素数がずれるため補正
		i--;
	}
}

void Magic::WeaponUpdate()
{
	// 弾処理
	for (int i = 0; i < m_shots.size(); i++)
	{
		// 更新処理
		m_shots[i]->UpdateTargetPos(m_targetPos);
		m_shots[i]->Update();
		colliders[i].pos[0] = m_shots[i]->GetPos();
		colliders[i].radius = m_shots[i]->GetRadius();
		colliders[i].isChackOther = m_shots[i]->GetIsHit();

		// 消去処理
		if (!m_shots[i]->GetIsApp())
			// || !colliders[i].isChackOther)
		{
			EffectManager::Instance().StopEffect("MagicCircle");
			// モデルの削除
			m_shots[i]->DeleteModel();
			// vectorでeraseを行ったら空要素分自動で詰めて要素数も更新される
			m_shots.erase(m_shots.begin() + i);
			colliders.erase(colliders.begin() + i);
			// 詰めた分要素数がずれるため補正
			i--;
		}
	}
#ifdef _DEBUG


#endif // _DEBUG

	
	DrawingManager::Instance().UpdateModelData("MagicSphere", m_spherePos, VZero());
	EffectManager::Instance().UpdateEffectData("Magic", m_spherePos, VZero());
}

void Magic::ColUpdate(float scale, int time)
{
}

void Magic::SetShot(int& shotHandle, VECTOR shotDir, float shotSize, int shotNum)
{
	// １弾のみなら正面そのまま
	if (shotNum == 1)
	{
		auto efcDir = VGet(shotDir.z, 0, -shotDir.x);
		auto circlePos = VAdd(m_spherePos, VScale(shotDir, 8.0f));
		EffectManager::Instance().PlayEffect("MagicCircle", circlePos, efcDir);
		auto name = "Pshot_" + std::to_string(m_shotNum);
		// エフェクトの登録
		EffectManager::Instance().RegisterEffect(name, HandleManager::Instance().GetEffectHandle(EffectName::PLAYERSHOT), m_spherePos, shotDir);
		Shot* temp = new Shot;
		temp->SetUp(m_shotNum ,shotHandle, m_spherePos, m_targetPos, kShotTime, shotDir, shotDir, shotSize);
		m_shots.push_back(temp);
		Collider colTemp;
		colTemp.SetSphere(m_spherePos, -1, 5, false);
		colTemp.isChackOther = true;
		colliders.push_back(colTemp);
		m_shotNum++;
		if (m_shotNum >= kShotMax)
		{
			m_shotNum = 0;
		}
		temp = nullptr;
	}
}

void Magic::OffIsCollider()
{
	for (auto& t : colliders)
	{
		t.isChackOther = false;
	}
	for (int i = 0; i < m_shots.size(); i++)
	{
		EffectManager::Instance().StopEffect("MagicCircle");
		// 消去処理
		// モデルの削除
		m_shots[i]->DeleteModel();
		// vectorでeraseを行ったら空要素分自動で詰めて要素数も更新される
		m_shots.erase(m_shots.begin() + i);
		colliders.erase(colliders.begin() + i);
		// 詰めた分要素数がずれるため補正
		i--;
	}
}

void Magic::SetModel(VECTOR pos, VECTOR dir)
{
}
