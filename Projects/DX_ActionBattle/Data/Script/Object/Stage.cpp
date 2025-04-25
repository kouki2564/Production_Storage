#include "Stage.h"
#include "DrawingManager.h"

Stage::Stage()
{
}

Stage::~Stage()
{
	MV1DeleteModel(m_handle);
}

void Stage::Init(std::string name, VECTOR pos, VECTOR dir)
{
	// m_modelData.handle = MV1LoadModel("Data/Model/DebugStage.mv1");
	/*m_pos = pos;
	m_dir = dir;
	DrawingManager::Instance().RegisterMapModel(name, m_handle, m_pos, m_dir, VGet(20.0f, 20.0f, 20.0f));
	m_physics.SetGravity(0, VZero());
	m_collider.SetMap(m_handle, VGet(20.0f, 20.0f, 20.0f));*/
}

void Stage::InitStage(std::string name, int handle)
{
	m_handle = MV1DuplicateModel(handle);
	m_pos = VGet(0, 0, 0);
	m_dir = VGet(0, 0, -1);
	DrawingManager::Instance().RegisterMapModel(name, m_handle, m_pos, m_dir, VGet(12.0f, 12.0f, 12.0f));
	DrawingManager::Instance().SetIsShadowModel(name, false);
	m_physics.SetGravity(0, VZero());
	m_collider.SetMap(m_handle, VGet(12.0f, 12.0f, 12.0f), true);
}

void Stage::Update()
{
}

void Stage::SetStageHandle(int handle)
{
	m_handle = handle;
}

void Stage::SetPosAndDir(VECTOR pos, VECTOR dir)
{
}
