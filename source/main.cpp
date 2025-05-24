#include "example.h"
#include <iostream>

#include "Engine/Engine.h"

int main(int argc, char** argv)
{
#ifdef _DEBUG
	std::cout << "------DEBUG_MODE------" << std::endl;
#else
    #ifdef _RELEASE
        std::cout << "------RELEASE_MODE------" << std::endl;
    #else
        std::cerr << "undesired mode" << std::endl;
        exit(-1);
    #endif
#endif

#ifdef _WIN32
    std::cout << "------Windows_MODE------" << std::endl;
#endif
	Example::Main();

    TinyRenderer::Engine::get_instance().startup();
    TinyRenderer::Engine::get_instance().run();
    TinyRenderer::Engine::get_instance().shutdown();


	return 0;
}
