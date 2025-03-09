#pragma once
#include "Platform/Path.h"
#include <string>
#include "Function/Message/Message.h"

#define TR_XSTR(s) TR_STR(s)
#define TR_STR(s) #s
namespace TinyRenderer
{
	/// <summary>
	/// 负责提供文件操作服务
	/// </summary>
	class FileServer
	{
	public:
		// 获取引擎根目录目录
		static const Path& getRootPath()
		{
			static Path* TR_RootPath = nullptr;
			if (!TR_RootPath)
				TR_RootPath = new Path(std::string(TR_XSTR(TinyRenderer_Root_Path)));
			return *TR_RootPath;
		}

		// 获取引擎source目录
		static const Path& getSourcePath()
		{
			static Path* TR_SourcePath = nullptr;
			if (!TR_SourcePath)
				TR_SourcePath = new Path(getRootPath().m_path / "source");
			return *TR_SourcePath;
		}

		// 获取引擎resource目录
		static const Path& getResourcePath()
		{
			static Path* TR_ResourcePath = nullptr;
			if (!TR_ResourcePath)
				TR_ResourcePath = new TinyRenderer::Path(getRootPath().m_path / "resource");
			return *TR_ResourcePath;
		}

		// 判断当前路径是否存在
		static inline bool isExist(const std::string& s_path)
		{
			if (std::filesystem::exists(s_path))
				return true;
			return false;
		}
		static inline bool isExist(const Path& path)
		{
			if (std::filesystem::exists(path.m_path))
				return true;
			return false;
		}

		// 判断是否为文件类型
		static bool isRegularFile(const Path& path)
		{
			if (isExist(path))
			{
				if (filesystem::is_regular_file(path.m_path))
				{
					return true;
				}
			}
			else
			{
				DebugWarn(path);
			}
			return false;
		}

		// 判断是否为目录
		static bool isDirectory(const Path& path)
		{
			if (isExist(path)) 
			{
				if (filesystem::is_directory(path.m_path))
				{
					return true;
				}
			}
			else
			{
				DebugWarn(path);
			}
			return false;
		}

		// 获取文件名
		static Path getFileName(const Path& path)
		{
			if (isExist(path))
			{
				return Path(path.m_path.filename());
			}
			else
			{
				DebugWarn(path);
			}
			return Path();
		}
		// 获取文件后缀
		static Path getExtension(const Path& path)
		{
			if (isExist(path) && !isDirectory(path))
			{
				return Path(path.m_path.extension());
			}
			else
			{
				DebugWarn(path);
			}
			return Path();
		}

		// 返回cur相较于des的相对地址
		static Path Relative(const Path& current, const Path& destination)
		{
			if (isExist(current) && isExist(destination))
			{
				return Path(filesystem::relative(destination.m_path, current.m_path));
			}
			else
			{
				DebugWarn(current);
				DebugWarn(destination);
			}
		}

		// 创建目录
		static bool CreateDir(const Path& path)
		{
			if (!isExist(path))
			{
				filesystem::create_directories(path.m_path);
				return true;
			}
			else
			{
				LOG_WARN("Path: " +path.toString() + " isExist");
				return false;
			}
		}

		// 创建删除
		static bool RemoveDir(const Path& path)
		{
			if (checkRemoveValid(path) == false)
			{
				LOG_ERROR("Can`t Remove Engine Root Directory Path:" + path.toString());
			}
			else if (isExist(path))
			{
				filesystem::remove_all(path.m_path);
				return true;
			}
			else
			{
				DebugWarn(path);
			}
			return false;
		}


		// 写文件操作和读文件操作

		private:
			static inline void DebugWarn(const Path& path)
			{
				LOG_WARN("Path: " + path.m_path.string() + " can`t find");
			}

			static bool checkRemoveValid(const Path& path)
			{
				std::string name = getFileName(path).toString();
				cout << name << "\n";
				if (name != "resource" && name != "source" && name != "third_party" && name != "CMakeLists.txt")
					return true;
				return false;
			}

	};
}