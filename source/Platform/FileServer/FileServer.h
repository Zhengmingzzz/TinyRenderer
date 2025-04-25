#pragma once
#include "Platform/Path.h"
#include <string>
#include "Function/Message/Message.h"

#define TR_XSTR(s) TR_STR(s)
#define TR_STR(s) #s
namespace TinyRenderer
{
	/// <summary>
	/// �����ṩ�ļ���������
	/// </summary>
	class FileServer
	{
	public:
		// ��ȡ�����Ŀ¼Ŀ¼
		static Path get_rootPath()
		{
			return Path(std::string(TR_XSTR(TinyRenderer_Root_Path)));
		}

		// ��ȡ����sourceĿ¼
		static Path get_sourcePath(){
			return Path(get_rootPath().m_path / "source");

		}

		// ��ȡ����resourceĿ¼
		static Path get_resourcePath()
		{
			return Path(get_rootPath().m_path / "resource");
		}

		// �жϵ�ǰ·���Ƿ����
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

		// �ж��Ƿ�Ϊ�ļ�����
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

		// �ж��Ƿ�ΪĿ¼
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

		// ��ȡ�ļ���
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
		// ��ȡ�ļ���׺
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

		// ����cur�����des����Ե�ַ
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

		// ����Ŀ¼
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

		// ����ɾ��
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


		// д�ļ������Ͷ��ļ�����

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