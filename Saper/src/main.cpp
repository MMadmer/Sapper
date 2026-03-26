#include "sapper/app/GameApp.hpp"

#include <cstdlib>
#include <exception>
#include <iostream>

#ifdef _WIN32
#include <windows.h>
#endif

int main()
{
    try
    {
        sapper::app::GameApp gameApp;
        return gameApp.run();
    }
    catch (const std::exception& exception)
    {
#ifdef _WIN32
        MessageBoxA(nullptr, exception.what(), "Sapper", MB_OK | MB_ICONERROR);
#else
        std::cerr << exception.what() << '\n';
#endif
        return EXIT_FAILURE;
    }
}
