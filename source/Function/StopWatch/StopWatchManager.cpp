#include "StopWatchManager.h"

namespace TinyRenderer
{
	StopWatchManager& StopWatchManager::instance()
	{
		static StopWatchManager* instance = nullptr;
		if(instance == nullptr)
			instance = new StopWatchManager();
		return *instance;
	}
	void StopWatchManager::startUp()
	{
		instance();
	}

	void StopWatchManager::shutDown()
	{
		for (auto& stopWatch : stopWatchMap_)
		{
			delete stopWatch.second;
		}
		stopWatchMap_.clear();

		delete &instance();
	}

	StopWatch* StopWatchManager::create_stopWatch(const char* name)
	{
		const std::string key(name);

		StopWatch* stopWatch = nullptr;
		if (stopWatchMap_.find(key) == stopWatchMap_.end())
		{
			stopWatch = new StopWatch();
			stopWatchMap_[key] = stopWatch;
		}
		else
		{
			LOG_WARN(key << " is already existed in StpoWatchMap");
		}
		return stopWatch;
	}

	StopWatch* StopWatchManager::get_stopWatch(const char* name)
	{
		const std::string key(name);

		StopWatch* stopWatch = nullptr;
		if (stopWatchMap_.find(key) != stopWatchMap_.end())
		{
			stopWatch = stopWatchMap_[key];
		}
		return stopWatch;
	}
}