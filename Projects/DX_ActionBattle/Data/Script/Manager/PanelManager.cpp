#include "PanelManager.h"
#include "DxLib.h"
#include <cassert>

namespace
{
}

PanelManager::PanelManager()
{
	for (int i = 0; i < 10; i++)
	{
		std::string path = "Data/Image/number_" + std::to_string(i) + ".png";
		m_numberHandle[i] = LoadGraph(path.c_str());
	}
}

PanelManager::~PanelManager()
{
	for (int i = 0; i < 10; i++)
	{
		DeleteGraph(m_numberHandle[i]);
	}
}

void PanelManager::PanelUpdate()
{
	DrawNumber();
	DrawPanel();
}

void PanelManager::RegisterImage(std::string name, int handle, float posX, float posY, float scaleX, float scaleY)
{
	ImageData initData;
	initData.handle = handle;
	initData.posX = posX;
	initData.posY = posY;
	// 画像サイズ獲得
	GetGraphSizeF(initData.handle, &initData.graphRangeX, &initData.graphRangeY);
	initData.scaleX = scaleX;
	initData.scaleY = scaleY;
	initData.isDraw = true;
	m_imageData.insert(std::make_pair(name, initData));
}

void PanelManager::DeleteImage(std::string name)
{
	// nameが存在しない場合はエラー
	assert(m_imageData.find(name) != m_imageData.end());
	
	// ハンドルは一括管理してるからデリートいらない
	// DeleteGraph(m_imageData[name].handle);

	m_imageData.erase(name);
}

void PanelManager::DeleteAllImage()
{
	// ハンドルは一括管理してるからデリートいらない
	/*for (auto& image : m_imageData)
	{
		DeleteGraph(image.second.handle);
	}*/
	m_imageData.clear();
}

void PanelManager::Draw()
{
	DrawNumber();
	DrawPanel();
}

void PanelManager::DrawPanel()
{
	for (auto& image : m_imageData)
	{
		if (image.second.isDraw)
		{
			// 画像のアルファ値を設定
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, image.second.alpha);
			// 画像の拡大率を設定
			float halfRangeX = image.second.graphRangeX * image.second.scaleX * 0.5f;
			float halfRangeY = image.second.graphRangeY * image.second.scaleY * 0.5f;
			// 画像の描画
			DrawExtendGraphF(image.second.posX - halfRangeX, image.second.posY - halfRangeY, image.second.posX + halfRangeX, image.second.posY + halfRangeY, image.second.handle, true);
			
		}
	}
	// 描画終了後にアルファ値を元に戻す
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
}

void PanelManager::SetIsDrawImage(std::string name, bool isDraw)
{
	// nameが存在しない場合はエラー
	assert(m_imageData.find(name) != m_imageData.end());

	m_imageData[name].isDraw = isDraw;
}

void PanelManager::SetAlpha(std::string name, int alpha)
{
	// nameが存在しない場合はエラー
	assert(m_imageData.find(name) != m_imageData.end());

	m_imageData[name].alpha = alpha;
}

void PanelManager::UpdateImagePos(std::string name, float posX, float posY)
{
	// nameが存在しない場合はエラー
	assert(m_imageData.find(name) != m_imageData.end());

	m_imageData[name].posX = posX;
	m_imageData[name].posY = posY;
}

void PanelManager::UpdateImageScale(std::string name, float scaleX, float scaleY)
{
	// nameが存在しない場合はエラー
	assert(m_imageData.find(name) != m_imageData.end());

	m_imageData[name].scaleX = scaleX;
	m_imageData[name].scaleY = scaleY;
}

void PanelManager::GetPos(std::string name, float* posX, float* posY)
{
	// nameが存在しない場合はエラー
	assert(m_imageData.find(name) != m_imageData.end());

	*posX = m_imageData[name].posX;
	*posY = m_imageData[name].posY;
}

void PanelManager::GetSize(std::string name, float* rangeX, float* rangeY)
{
	// nameが存在しない場合はエラー
	assert(m_imageData.find(name) != m_imageData.end());

	*rangeX = m_imageData[name].graphRangeX;
	*rangeY = m_imageData[name].graphRangeY;
}

bool PanelManager::GetIsDrawImage(std::string name)
{
	// nameが存在しない場合はエラー
	assert(m_imageData.find(name) != m_imageData.end());

	return m_imageData[name].isDraw; 
}

void PanelManager::SetNumber(int number, VECTOR pos, float scale, int drawTime, VECTOR cameraDir)
{
	numData initData;
	// 数値を桁ごとに分解してvectorに格納
	while (number > 0)
	{
		initData.number.push_back(number % 10);
		number /= 10;
	}
	initData.decAlpha = static_cast<int>(255 / (drawTime * 0.5f));
	initData.time = drawTime;
	initData.timeMax = drawTime;
	initData.pos = pos;
	initData.pos.y += 20.0f;
	initData.scale = scale;
	initData.cameraDir = cameraDir;
	m_numberData.push_back(initData);
}

void PanelManager::DrawNumber()
{
	for (auto it = m_numberData.begin(); it != m_numberData.end(); )
	{
		auto& num = *it;
		         
		// 描画時間が残っている場合
		if (num.time > 0)
		{
			if (num.time > num.timeMax * 0.5f)
			{
				SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
			}
			else
			{
				// ここから
				int alpha = static_cast<int>(255 - num.decAlpha * (num.timeMax * 0.5f - num.time));
				SetDrawBlendMode(DX_BLENDMODE_ALPHA, alpha);
			}

			float center = (num.number.size() - 1) * 0.5f;

			for (int i = 0; i < num.number.size(); i++)
			{
				int drawNum = num.number[i];

				float scale = (i - center) * 4;
				auto rightDir = VNorm(VCross(VGet(0, 1, 0), num.cameraDir));
				auto drawPos = ConvWorldPosToScreenPos(VAdd(num.pos, VScale(rightDir, scale)));

				if (drawPos.z > 0.0f && drawPos.z < 1.0f)
				{
					VECTOR pos = VGet(drawPos.x, drawPos.y, 0.0f);
					float temp = (static_cast<float>(num.timeMax - num.time) / static_cast<float>(num.timeMax));
					
					float drawScale = num.scale * temp;

					// 描画
					if (i - center == 0)		// 中央の数字
					{
						DrawExtendGraphF(
						   drawPos.x - 27.5f * drawScale,
						   drawPos.y - 100 * drawScale,
						   drawPos.x + 27.5f * drawScale,
						   drawPos.y,
						   m_numberHandle[drawNum],
						   TRUE
						);
					}
					else if (i - center < 0)	// 左側の数字
					{
						DrawExtendGraphF(
						   drawPos.x + 35 * (center - i),
						   drawPos.y - 100 * drawScale,
						   drawPos.x + 35 * (center - i) + 55 * drawScale,
						   drawPos.y,
						   m_numberHandle[drawNum],
						   TRUE
						);
					}
					else						// 右側の数字
					{
						DrawExtendGraphF(
						   drawPos.x + 35 * (center - i) - 55 * drawScale,
						   drawPos.y - 100 * drawScale,
						   drawPos.x + 35 * (center - i),
						   drawPos.y,
						   m_numberHandle[drawNum],
						   TRUE
						);
					}
					
				}
			}

			num.time--;
			num.pos.y += 0.5f;

			++it;  // 次の要素へ
		}
		else
		{
			// erase()の戻り値を新しいイテレータに更新
			it = m_numberData.erase(it);
		}
	}
	// 描画終了後にアルファ値を元に戻す
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
}
