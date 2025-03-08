#pragma once
#include <filesystem>
#include <string>
#include "Function/Message/Message.h"

using namespace std;
/// <summary>
/// 统一不同操作系统的路径表示
/// </summary>
class Path
{
public:
	Path operator/(const Path& p)
	{
		std::filesystem::path combined = m_path / p.m_path;
		return Path(combined);
	}
	Path operator/(const std::string& s)
	{
		std::filesystem::path combined = m_path / s;
		return Path(combined);
	}
	Path() {}

	Path(const filesystem::path& p_path)
	{
		filesystem::path new_path = filesystem::path(p_path);
		this->m_path = new_path;
	}


	std::string toString()
	{
		return m_path.string();
	}
private:
	filesystem::path m_path;
};