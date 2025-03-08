#include <iostream>
#include "Platform/Application.h"

int main(int argc, void* argv)
{
	TinyRenderer::Application* app = new TinyRenderer::WindowsApplication();
	app->Main(argc, argv);
	return 0;
}