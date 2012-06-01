#include <pch.h>

#include <ServerApp.h>

int terminateSignal = 0;
bool continueRun = true;

void  HandleTerminateSignal(int sig)
{
    LOG(INFO) << "Recived signal " << sig;
    terminateSignal = sig;
    continueRun = false;
}

int main(int argc, char **argv)
{
    google::InstallFailureSignalHandler();
    signal(SIGPIPE, SIG_IGN);
    signal(SIGINT, HandleTerminateSignal);
    signal(SIGQUIT, HandleTerminateSignal);

    try
    {
        Run(argc, argv, continueRun);
    }
    catch (std::exception& e)
    {
        std::cerr << "An exception has occured: " << e.what();
    }
    catch (...)
    {
        std::cerr << "Exception!";
    }

    // Terminate with proper status/exit result, so calling program knows
    if (terminateSignal)
    {
        signal(terminateSignal, SIG_DFL);
        kill(getpid(), terminateSignal);
    }

    return 0;
}

