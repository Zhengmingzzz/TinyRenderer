#pragma once
#include "Platform/Path.h"
#include <string>

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
		static Path* getRootPath()
		{
			static Path* TR_RootPath = nullptr;
			if(!TR_RootPath)
				TR_RootPath = new Path(std::string(TR_XSTR(TinyRenderer_Root_Path)));
			return TR_RootPath;
		}

		static inline bool isExist(const std::string s_path)
		{
			if (std::filesystem::exists(s_path))
			{
				return true;
			}
			return false;
		}
		
		// д�ļ������Ͷ��ļ�����
	};
}