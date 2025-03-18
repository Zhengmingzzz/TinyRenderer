#include "Application.h"
#include "Function/Message/Message.h"
#include "Core/MemoryManager/MemoryManager.h"
#include "Function/StopWatch/StopWatchManager.h"
#include <cstdlib>

void* operator new(size_t size)
{
	return TinyRenderer::MemoryManager::GetInstance().Allocate(size);
}

void operator delete(void* ptr, size_t) noexcept
{
	TinyRenderer::MemoryManager::GetInstance().Deallocate(ptr);
}
void* operator new[](size_t size)
{
	return TinyRenderer::MemoryManager::GetInstance().Allocate(size);
}
void operator delete[](void* ptr, size_t size) noexcept
{
	TinyRenderer::MemoryManager::GetInstance().Deallocate(ptr);
}

namespace TinyRenderer
{
	Application* Application::instance = nullptr;
	Application* WindowsApplication::GetInstance()
	{
		if (instance == nullptr)
			instance = new WindowsApplication();
		return instance;
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
		MemoryManager::GetInstance().StartUp(8, 2048);
		StopWatchManager::GetInstance()->StartUp();

		return true;
	}

	void MemMgr_test()
	{
		char* c[10000];
		int* d[10000];
		int c_cnt = -1;
		int i_cnt = -1;
		for (int i = 0; i < 10000000; i++) {
			if (rand() % 2 == 0)
			{
				if (rand() % 2 == 0)
				{
					c_cnt++;
					c[c_cnt] = new char;
				}
				else
				{
					i_cnt++;
					d[i_cnt] = new int;
				}
			}
			else
			{
				if (rand() % 2 == 0)
				{
					if (c_cnt > -1)
					{
						delete c[c_cnt];
						c_cnt--;
					}
				}
				else
				{
					if (i_cnt > -1)
					{
						delete d[i_cnt];
						i_cnt--;
					}
				}
			}
		}

		if (c_cnt != -1)
		{
			for (int i = 0; i <= c_cnt; i++)
			{
				delete c[i];
			}
		}
		if (i_cnt != -1)
		{
			for (int i = 0; i <= i_cnt; i++)
			{
				delete d[i];
			}
		}
	}


	bool WindowsApplication::Run()
	{
		/*StopWatch_Start(application);
			MemMgr_test();
		StopWatch_Pause(application);
		StopWatch_Microseconds(application);*/
		return true;
	}

	bool WindowsApplication::TerminalEngine()
	{
		StopWatchManager::GetInstance()->ShutDown();
		MemoryManager::GetInstance().ShutDown();
		return true;
	}
}