#include <pch.h>

#include <ServerApp.h>

int main(int argc, char **argv)
{
    google::InstallFailureSignalHandler();
    signal(SIGPIPE, SIG_IGN);

    try
    {
        Run(argc, argv);
    }
    catch (std::exception& e)
    {
        std::cerr << "An exception has occured: " << e.what();
    }
    catch (...)
    {
        std::cerr << "Exception!";
    }

    return 0;
}

