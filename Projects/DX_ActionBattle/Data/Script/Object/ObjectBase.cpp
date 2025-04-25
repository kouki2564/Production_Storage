#include "ObjectBase.h"
#include <cassert>

void ObjectBase::SetUpObject(std::string name, int handle, VECTOR pos, VECTOR dir, VECTOR scale)
{
	DrawingManager::Instance().RegisterModel(name, handle,pos,dir,scale);
}

void ObjectBase::FixPos()
{
	int x = m_pos.x * 100;
	int y = m_pos.y * 100;
	int z = m_pos.z * 100;

	if ((float)x + 0.5f <= m_pos.x)
	{
		x++;
	}
	if ((float)y + 0.5f <= m_pos.y)
	{
		y++;
	}
	if ((float)z + 0.5f <= m_pos.z)
	{
		z++;
	}

	m_pos = VGet((float)(x * 0.01f), (float)(y * 0.01f), (float)(z * 0.01f));
}

void ObjectBase::SetLevel(Level level)
{
	auto upGradeAtk = level.Atk - m_level.Atk;
	auto upGradeDef = level.Def - m_level.Def;
	auto upGradeAgi = level.Agi - m_level.Agi;

	if (upGradeAtk > 0)
	{
		auto upPoint = 3 * upGradeAtk;
		m_parameter.UpGradeParameter(1, upPoint);
	}
	if (upGradeDef > 0)
	{
		auto upPoint = 3 * upGradeDef;
		m_parameter.UpGradeParameter(2, upPoint);
		upPoint = 50 * upGradeDef;
		m_parameter.UpGradeParameter(0, upPoint);
	}
	if (upGradeAgi > 0)
	{
		auto upPoint = 2 * upGradeAgi;
		m_parameter.UpGradeParameter(3, upPoint);
	}

	m_level = level;
}
