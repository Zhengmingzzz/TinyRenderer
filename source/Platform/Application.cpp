#include "Application.h"
#include "Function/Message/Message.h"
#include "Core/MemoryManager/MemoryManager.h"
#include "Function/StopWatch/StopWatchManager.h"
#include <cstdlib>

// void* operator new(size_t size)
// {
// 	return TinyRenderer::MemoryManager::instance().Allocate(size);
// }
//
// void operator delete(void* ptr, size_t) noexcept
// {
// 	TinyRenderer::MemoryManager::instance().Deallocate(ptr);
// }
// void* operator new[](size_t size)
// {
// 	return TinyRenderer::MemoryManager::instance().Allocate(size);
// }
// void operator delete[](void* ptr, size_t size) noexcept
// {
// 	TinyRenderer::MemoryManager::instance().Deallocate(ptr);
// }

namespace TinyRenderer
{
	Application* Application::instance_ = nullptr;
	Application* WindowsApplication::instance()
	{
		if (instance_ == nullptr)
			instance_ = new WindowsApplication();
		return instance_;
	};
	
	int WindowsApplication::Main(int argc, char** argv)
	{
		if (!InitialEngine())
		{
			LOG_ERROR("Engine Initial Failed");
		}
		Run();
		TerminalEngine();
		return 0;
	}

	bool WindowsApplication::InitialEngine()
	{
		//MemoryManager::instance().startUp(8, 2048);
		StopWatchManager::instance().startUp();

		return true;
	}



	bool WindowsApplication::Run()
	{

		return true;
	}

	bool WindowsApplication::TerminalEngine()
	{
		StopWatchManager::instance().shutDown();
		//MemoryManager::instance().shutDown();
		return true;
	}
}