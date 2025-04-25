#include "CSVReader.h"

CSVReader::CSVReader()
{
	// パスの設定
	m_path.push_back("Data/CSV/DataNum.csv");
}

CSVReader::~CSVReader()
{
	m_path.clear();
}
