#include "Platform/Application.h"
#include "example.h"

int main(int argc, char** argv)
{
	Example::Main();
	TinyRenderer::Application* app = new TinyRenderer::WindowsApplication();
	app->Main(argc, argv);
	delete app;
	return 0;
}