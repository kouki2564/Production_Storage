#pragma once

struct Level
{
	int Atk = 1;
	int Def = 1;
	int Agi = 1;
};

class Parameter
{
public:


	// キャラクターパラメータ
	struct Para
	{
		int MaxHP;
		int HP;
		int Atk;
		int Def;
		int Agi;
	};

	// パラメータ登録
	void SetPalameter(int hp, int atk, int def, int agi)
	{
		m_para.MaxHP = hp;
		m_para.HP = hp;
		m_para.Atk = atk;
		m_para.Def = def;
		m_para.Agi = agi;
	}

	// ダメージ分HPから減算
	void SetDamage(int damagePoint)
	{
		m_para.HP -= damagePoint;
		if (m_para.HP < 0)
		{
			m_para.HP = 0;
		}
	}

	/// <summary>
	/// ステータスパラメータ更新
	/// </summary>
	/// <param name="paramNum">0:HP, 1:Atk, 2:Def, 3:Agi</param>
	/// <param name="upPoint">パラメータ上昇値</param>
	void UpGradeParameter(int paramNum, int upPoint)
	{
		if (paramNum == 0)
		{
			m_para.MaxHP += upPoint;
			m_para.HP = m_para.MaxHP;
		}
		else if (paramNum == 1)
		{
			m_para.Atk += upPoint;
		}
		else if (paramNum == 2)
		{
			m_para.Def += upPoint;
		}
		else
		{
			m_para.Agi += upPoint;
		}
	}

	

	/// <summary>
	/// パラメータ値獲得
	/// </summary>
	/// <param name="paramNum">0:HP, 1:Atk, 2:Def, 3:Agi</param>
	/// <returns>値を返す</returns>
	int GetPalameter(int paramNum)
	{
		if (paramNum == 0)
		{
			return m_para.HP;
		}
		else if (paramNum == 1)
		{
			return m_para.Atk;
		}
		else if(paramNum == 2)
		{
			return m_para.Def;
		}
		else
		{
			return m_para.Agi;
		}
	}

	// 残HPの割合の獲得
	float GetHPRate() 
	{
		return (static_cast<float>(m_para.HP) / static_cast<float>(m_para.MaxHP)); 
	}

private:
	Para m_para;
	
};

