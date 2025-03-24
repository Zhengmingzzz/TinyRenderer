#pragma once

#include "StopWatch.h"
#include <unordered_map>

namespace TinyRenderer
{
	class StopWatchManager
	{
	public:
		static StopWatchManager* GetInstance();

		void StartUp();

		void ShutDown();


		StopWatch* CreateStopWatch(const char* name);

		StopWatch* GetStopWatch(const char* name);

		~StopWatchManager() {};
	private:
		static StopWatchManager* instance;
		std::unordered_map<std::string, StopWatch*> stopWatchMap;

		StopWatchManager() {};
		StopWatchManager(const StopWatchManager&) = delete;
		StopWatchManager& operator=(const StopWatchManager&) = delete;


	};

	#define StopWatch_Start(arg_name) \
			do{ \
				StopWatch* stopWatch_##arg_name = StopWatchManager::GetInstance()->GetStopWatch(#arg_name); \
				if(stopWatch_##arg_name == nullptr) \
				{ \
					stopWatch_##arg_name = StopWatchManager::GetInstance()->CreateStopWatch(#arg_name); \
				} \
				stopWatch_##arg_name->Start(); \
			}while(0);

	#define StopWatch_Pause(arg_name) \
			do{ \
				StopWatch* stopWatch_##arg_name = StopWatchManager::GetInstance()->GetStopWatch(#arg_name); \
				if(stopWatch_##arg_name != nullptr) \
				{ \
					stopWatch_##arg_name->Pause(); \
				}\
			}while (0);


	#define StopWatch_Reset(arg_name) \
			do{ \
				StopWatch* stopWatch_##arg_name = StopWatchManager::GetInstance()->GetStopWatch(#arg_name); \
				if(stopWatch_##arg_name != nullptr) \
				{ \
					stopWatch_##arg_name->Reset(); \
				} \
			}while(0);

			// ΢��
	#define StopWatch_Microseconds(arg_name) \
			do{ \
				StopWatch* stopWatch_##arg_name = StopWatchManager::GetInstance()->GetStopWatch(#arg_name); \
				if(stopWatch_##arg_name != nullptr) \
				{ \
					LOG_INFO("stopWatch"#arg_name << " cost:" << stopWatch_##arg_name->microseconds() << " microseconds"); \
				} \
			}while(0);

			// ����
	#define StopWatch_Milliseconds(arg_name) \
			do{ \
				StopWatch* stopWatch_##arg_name = StopWatchManager::GetInstance()->GetStopWatch(#arg_name); \
				if(stopWatch_##arg_name != nullptr) \
				{ \
					LOG_INFO("stopWatch"#arg_name << " cost:" << stopWatch_##arg_name->milliseconds() << " millionseconds"); \
				} \
			}while (0);

	#define StopWatch_Seconds(arg_name) \
			do{ \
				StopWatch* stopWatch_##arg_name = StopWatchManager::GetInstance()->GetStopWatch(#arg_name); \
				if(stopWatch_##arg_name != nullptr) \
				{ \
					LOG_INFO("stopWatch"#arg_name << " cost:" << stopWatch_##arg_name->seconds() << " seconds"); \
				} \
			}while(0);
} // namespace TinyRenderer