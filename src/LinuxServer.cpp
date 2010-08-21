#include <pch.h>

#include <Server.h>
#include <iostream>

int main(int argc, char **argv)
{
    signal(SIGPIPE, SIG_IGN);

    try
    {
        RunServer(argc, argv);
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
