#pragma once
#include <filesystem>
#include <string>
#include "Function/Message/Message.h"

using namespace std;
namespace TinyRenderer
{
	/// <summary>
	/// 统一不同操作系统的路径表示
	/// </summary>
	class Path
	{
	public:
		friend class FileServer;
		// 重载操作符/ 可以用除号表示下一级
		Path operator/(const char* c_str)
		{
			std::filesystem::path combined = m_path / c_str;
			return Path(combined);
		}
		Path operator/(const Path& p)
		{
			std::filesystem::path combined = m_path / p.m_path;
			return Path(combined);
		}
		Path() {}

		Path(const filesystem::path& p_path)
		{
			filesystem::path new_path = filesystem::path(p_path);
			this->m_path = new_path;
		}

		std::string toString()const
		{
			return m_path.string();
		}
	private:
		filesystem::path m_path;
	};
}