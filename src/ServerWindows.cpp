#include <pch.h>

#include <ServerApp.h>
#include <iostream>

int main(int argc, char **argv)
{
    try
    {
        Run(argc, argv);
    }
    catch (std::exception& e)
    {
        std::cerr << "An exception has occured: " << e.what();
        return 1;
    }
    catch (...)
    {
        std::cerr << "Exception!";
        return 2;
    }
    return 0;
}
