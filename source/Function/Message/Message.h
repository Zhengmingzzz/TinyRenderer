#pragma once
#include <string>
#include "spdlog/spdlog.h"
#include <cassert>
#include <iostream>

#include "Platform/Application.h"



namespace TinyRenderer
{
	/// <summary>
	/// �����������Ϣ
	/// </summary>
	class Message
	{
	public:
		static inline void LogInfo(const std::string& msg)
		{
			spdlog::info(msg);
		}
		static inline void LogWarn(const std::string& msg)
		{
			spdlog::warn(msg);
		}
		static inline void LogError(const std::string& msg)
		{
			spdlog::error(msg);
		}
	};
}

#if DEBUG_MODE // ���Ϊdebugģʽ������assert
#define ASSERT(expr) \
        if (!(expr)) { \
            std::cerr << "File: " << __FILE__ << "\n" \
                      << "Line: " << __LINE__ << "\n"; \
            assert(expr); \
        } 
#ifndef LOG_INFO
#define LOG_INFO(msg) TinyRenderer::Message::LogInfo(msg);\
std::cerr << "File: " << __FILE__  << "  " \
<< "Line: " << __LINE__ << "\n";
#endif // !LOG_INFO

#ifndef LOG_WARN
#define LOG_WARN(msg) TinyRenderer::Message::LogWarn(msg);\
std::cerr << "File: " << __FILE__  << "  " \
	<< "Line: " << __LINE__ << "\n"; 
#endif // !LOG_WARN

#ifndef LOG_ERROR
#define LOG_ERROR(msg) TinyRenderer::Message::LogError(msg);\
		ASSERT(false);
#endif // !LOG_ERR

#else // ����ʲô������
#define ASSERT(expr)
#define LOG_INFO(msg)
#define LOG_WARN(msg)
#define LOG_ERROR(msg)
#endif






