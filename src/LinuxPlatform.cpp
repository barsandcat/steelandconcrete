#include <pch.h>

#include <Platform.h>
#include <sys/time.h>

DEFINE_string(data_dir, "/usr/share/games/steelandconcrete", "Path for game data");

int64 GetMiliseconds()
{
    timeval epoch;
    gettimeofday(&epoch, NULL);
    return epoch.tv_sec * 1000 + epoch.tv_usec / 1000;
}

void LaunchServer()
{
    pid_t pID = fork();
    if (pID == 0) // child
    {
        // Code only executed by child process
        const char* const cmd[] = { "steelandconcrete_d_server", 0 };
        execv("steelandconcrete_d_server", const_cast<char* const*>(cmd));

    }
    else if (pID < 0) // failed to fork
    {
        LOG(ERROR) << "Not launced";
    }
    else // parent
    {
        LOG(INFO) << "Launched";
    }
}
