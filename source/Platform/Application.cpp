#include "Application.h"
#include "Function/Message/Message.h"

namespace TinyRenderer
{
	Application* Application::Instance;
	Application* WindowsApplication::getInstance()
	{
		if (this->Instance == nullptr)
			this->Instance = new WindowsApplication();
		return this->Instance;
	};
	
	int WindowsApplication::Main(int argc, void* argv)
	{
		if (!initialEngine())
		{
			LOG_ERROR("Engine Initial Failed");
		}
		return 0;
	}
	bool WindowsApplication::run()
	{
		return true;
	}
	bool WindowsApplication::initialEngine()
	{
		return true;
	}
	bool WindowsApplication::terminalEngine()
	{
		return true;
	}
}