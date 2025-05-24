#pragma once

#include "StopWatch.h"
#include <unordered_map>

namespace TinyRenderer
{
	class StopWatchManager
	{
	public:
		static StopWatchManager& instance();

		void startUp();

		void shutDown();

		StopWatch* create_stopWatch(const char* name);

		StopWatch* get_stopWatch(const char* name);

		~StopWatchManager() = default;
	private:
		std::unordered_map<std::string, StopWatch*> stopWatchMap_;

		StopWatchManager() {};
	public:
		StopWatchManager(const StopWatchManager&) = delete;
		StopWatchManager& operator=(const StopWatchManager&) = delete;
	};
} // namespace TinyRenderer
#ifdef _DEBUG
#define StopWatch_Start(arg_name) \
		do{ \
			StopWatch* stopWatch_##arg_name = StopWatchManager::instance().get_stopWatch(#arg_name); \
			if(stopWatch_##arg_name == nullptr) \
			{ \
				stopWatch_##arg_name = StopWatchManager::instance().create_stopWatch(#arg_name); \
			} \
			if(stopWatch_##arg_name->getState() == StopWatch::State::RUNNING){ \
				LOG_WARN(#arg_name << " Stopwatch is already running");} \
			else \
				stopWatch_##arg_name->Start(); \
		}while(0);

#define StopWatch_Pause(arg_name) \
		do{ \
			StopWatch* stopWatch_##arg_name = StopWatchManager::instance().get_stopWatch(#arg_name); \
			if(stopWatch_##arg_name != nullptr) \
			{ \
				if(stopWatch_##arg_name->getState() == StopWatch::State::PAUSED || stopWatch_##arg_name->getState() == StopWatch::State::STOPPED) \
					{LOG_WARN(#arg_name << "Stopwatch is already paused");} \
				else \
					stopWatch_##arg_name->Pause(); \
			}\
			else \
			{ \
				LOG_WARN(#arg_name << " is not exist in stopWatchMap_"); \
			} \
		}while (0);


#define StopWatch_Reset(arg_name) \
		do{ \
			StopWatch* stopWatch_##arg_name = StopWatchManager::instance().get_stopWatch(#arg_name); \
			if(stopWatch_##arg_name != nullptr) \
			{ \
				stopWatch_##arg_name->Reset(); \
			} \
		}while(0);

		// 微秒
#define StopWatch_Microseconds(arg_name) \
		do{ \
			StopWatch* stopWatch_##arg_name = StopWatchManager::instance().get_stopWatch(#arg_name); \
			if(stopWatch_##arg_name != nullptr) \
			{ \
				LOG_INFO("stopWatch__"#arg_name << " cost:" << stopWatch_##arg_name->microseconds() << " microseconds"); \
			} \
			else{ \
				LOG_WARN("stopWatch__"#arg_name << " not exist"); \
			} \
		}while(0);

		// 毫秒
#define StopWatch_Milliseconds(arg_name) \
		do{ \
			StopWatch* stopWatch_##arg_name = StopWatchManager::instance().get_stopWatch(#arg_name); \
			if(stopWatch_##arg_name != nullptr) \
			{ \
				LOG_INFO("stopWatch"#arg_name << " cost:" << stopWatch_##arg_name->milliseconds() << " millionseconds"); \
			} \
		}while (0);

#define StopWatch_Seconds(arg_name) \
		do{ \
			StopWatch* stopWatch_##arg_name = StopWatchManager::instance().get_stopWatch(#arg_name); \
			if(stopWatch_##arg_name != nullptr) \
			{ \
				LOG_INFO("stopWatch"#arg_name << " cost:" << stopWatch_##arg_name->seconds() << " seconds"); \
			} \
		}while(0);
#else
#define StopWatch_Start(arg_name)
#define StopWatch_Pause(arg_name)
#define StopWatch_Reset(arg_name)
#define StopWatch_Microseconds(arg_name)
#define StopWatch_Milliseconds(arg_name)
#define StopWatch_Seconds(arg_name)
#endif
