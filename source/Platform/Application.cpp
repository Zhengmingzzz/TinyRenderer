#include "Application.h"
#include "Function/Message/Message.h"

namespace TinyRenderer
{
	Application* Application::Instance;
	Application* WindowsApplication::GetInstance()
	{
		if (Instance == nullptr)
			Instance = new WindowsApplication();
		return Instance;
	};
	
	int WindowsApplication::Main(int argc, char** argv)
	{
		if (!InitialEngine())
		{
			LOG_ERROR("Engine Initial Failed");
		}
		return 0;
	}
	bool WindowsApplication::Run()
	{
		return true;
	}
	bool WindowsApplication::InitialEngine()
	{
		return true;
	}
	bool WindowsApplication::TerminalEngine()
	{
		return true;
	}
}