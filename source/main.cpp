#include <iostream>
#include "Platform/Application.h"
#include "example.h"

#include "Function/Message/Message.h"
int main(int argc, char** argv)
{
	Example::Main();
	TinyRenderer::Application* app = new TinyRenderer::WindowsApplication();
	app->Main(argc, argv);
	return 0;
}