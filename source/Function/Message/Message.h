#pragma once
#include <string>
#include "spdlog/spdlog.h"
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
			spdlog::info(msg);
			ClearOss();
		}
		static inline void LogWarn(const std::string& msg)
		{
			spdlog::warn(msg);
			ClearOss();
		}
		static inline void LogError(const std::string& msg)
		{
			spdlog::error(msg);
			ClearOss();
		}
		static inline void ClearOss()
		{
			oss.str("");
		}
		static std::ostringstream oss;
	};
}

#if DEBUG_MODE // 如果为debug模式，则开启assert
#define ASSERT(expr) \
        if (!(expr)) { \
            assert(expr); \
        } 

#ifndef LOG_INFO
#define LOG_INFO(msg) \
TinyRenderer::Message::oss << msg <<"\n"<< "File:" << __FILE__ \
<< " Line: " << __LINE__<<"\n"; \
TinyRenderer::Message::LogInfo(TinyRenderer::Message::oss.str());
#endif // !LOG_INFO

#ifndef LOG_WARN
#define LOG_WARN(msg) \
TinyRenderer::Message::oss << msg << "\n" << "File:" << __FILE__ \
<< " Line: " << __LINE__ << "\n"; \
TinyRenderer::Message::LogWarn(TinyRenderer::Message::oss.str());
#endif // !LOG_WARN

#ifndef LOG_ERROR
#define LOG_ERROR(msg) \
TinyRenderer::Message::oss << msg << "\n" << "File:" << __FILE__ \
<< " Line: " << __LINE__ << "\n"; \
TinyRenderer::Message::LogError(TinyRenderer::Message::oss.str()); \
ASSERT(false);
#endif // !LOG_ERROR

#else // 否则什么都不做
#define ASSERT(expr)
#define LOG_INFO(msg)
#define LOG_WARN(msg)
#define LOG_ERROR(msg)
#endif






