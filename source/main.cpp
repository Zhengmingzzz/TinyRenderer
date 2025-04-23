#include "Platform/Application.h"
#include "example.h"
#include <iostream>

int main(int argc, char** argv)
{
#ifdef _DEBUG
	std::cout << "------DEBUG_MODE------" << std::endl;
#elifdef _RELEASE
	std::cout << "------RELEASE_MODE------" << std::endl;
#endif
	Example::Main();
	//TinyRenderer::Application* app = new TinyRenderer::WindowsApplication();
	//app->Main(argc, argv);
	//delete app;
	return 0;
}