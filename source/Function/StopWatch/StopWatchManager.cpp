#include "StopWatchManager.h"

namespace TinyRenderer
{
	StopWatchManager* StopWatchManager::instance = nullptr;
	
	StopWatchManager* StopWatchManager::GetInstance()
	{
		if(instance == nullptr)
			instance = new StopWatchManager();
		return instance;
	}
	void StopWatchManager::StartUp()
	{

	}

	void StopWatchManager::ShutDown()
	{
		for (auto& stopWatch : stopWatchMap)
		{
			delete stopWatch.second;
		}
		stopWatchMap.clear();

		delete instance;
	}

	StopWatch* StopWatchManager::CreateStopWatch(const char* name)
	{
		const std::string key(name);

		StopWatch* stopWatch = nullptr;
		if (stopWatchMap.find(key) == stopWatchMap.end())
		{
			stopWatch = new StopWatch();
			stopWatchMap[key] = stopWatch;
		}
		else
		{
			LOG_WARN(key << " StopWatch is already existed");
		}
		return stopWatch;
	}

	StopWatch* StopWatchManager::GetStopWatch(const char* name)
	{
		const std::string key(name);

		StopWatch* stopWatch = nullptr;
		if (stopWatchMap.find(key) != stopWatchMap.end())
		{
			stopWatch = stopWatchMap[key];
		}
		return stopWatch;
	}
}