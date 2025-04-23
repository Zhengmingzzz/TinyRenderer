#pragma once
#include <string>
#include <cassert>
#include <iostream>

#include "Platform/Application.h"

#include <sstream>

namespace TinyRenderer
{
	/// <summary>
	/// 负责程序发送信息
	/// </summary>
	class Message
	{
	public:
		static inline void LogInfo(const std::string& msg)
		{
			std::cout << msg << std::endl;
		}
		static inline void LogWarn(const std::string& msg)
		{
			std::cout << msg << std::endl;
		}
		static inline void LogError(const std::string& msg)
		{
			std::cout << msg << std::endl;

			//spdlog::error(msg);
		}

	};
}

#ifdef _DEBUG // 如果为debug模式，则开启assert
#define ASSERT(expr) \
        if (!(expr)) { \
            assert(expr); \
        } 

#ifndef LOG_INFO
#define LOG_INFO(msg) \
do{ \
	std::ostringstream _oss; \
	_oss << "----LOG_INFO----: "  << msg << "\n" \
	<< "Message occur place :" << __FILE__ \
	<< ":" << __LINE__ << "\n"; \
	TinyRenderer::Message::LogInfo(_oss.str()); \
}while(0);
#endif // !LOG_INFO

#ifndef LOG_WARN
#define LOG_WARN(msg) \
do{ \
	std::ostringstream _oss; \
	_oss << "----LOG_WARN----: "  << msg << "\n" \
	<< "Message occur place :" << __FILE__ \
	<< ":" << __LINE__ << "\n"; \
	TinyRenderer::Message::LogWarn(_oss.str()); \
}while(0);
#endif // !LOG_WARN

#ifndef LOG_ERROR
#define LOG_ERROR(msg) \
{ \
	do{ \
		std::ostringstream _oss; \
		_oss << "----LOG_ERROR----: "  << msg << "\n" \
		<< "Message occur place :" << __FILE__ \
		<< ":" << __LINE__ << "\n"; \
		TinyRenderer::Message::LogError(_oss.str()); \
	}while(0); \
	ASSERT(false); \
}
#endif // !LOG_ERROR

#else // 否则什么都不做
#define ASSERT(expr)
#define LOG_INFO(msg)
#define LOG_WARN(msg)
#define LOG_ERROR(msg)
#endif