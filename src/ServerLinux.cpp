#include <pch.h>

#include <ServerApp.h>

int main(int argc, char **argv)
{
    signal(SIGPIPE, SIG_IGN);
    google::InstallFailureSignalHandler();

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
