#include <pch.h>

#include <ServerApp.h>
#include <iostream>

int main(int argc, char **argv)
{
    try
    {
        bool continueRun = true;
        Run(argc, argv, continueRun);
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
