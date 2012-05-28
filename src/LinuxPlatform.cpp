#include <pch.h>

#include <Platform.h>
#include <sys/time.h>

#include <boost/timer/timer.hpp>

DEFINE_string(data_dir, "/usr/share/games/steelandconcrete", "Path for game data");

int64 GetMiliseconds()
{
    boost::timer::cpu_timer timer;
    boost::timer::cpu_times const elapsed_times(timer.elapsed());
    boost::timer::nanosecond_type const elapsed(elapsed_times.system + elapsed_times.user);
    return elapsed * 1e-6;
}

void LaunchServer()
{
    pid_t pID = fork();
    if (pID == 0) // child
    {
        // Code only executed by child process
        #ifndef NDEBUG
        const char* server = "steelandconcrete_d_server";
        #else
        const char* server = "steelandconcrete_server";
        #endif
        LOG(INFO) << "Launching server " << server;
        const char* const cmd[] = { server, 0 };
        execv(server, const_cast<char* const*>(cmd));

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
