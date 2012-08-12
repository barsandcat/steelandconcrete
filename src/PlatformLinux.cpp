#include <pch.h>

#include <Platform.h>


DEFINE_string(data_dir, "/usr/share/games/steelandconcrete", "Path for game data");

Ogre::String GetFlagsFilePath()
{
    Ogre::String home = getenv("HOME");
    return home + "/.steelandconcrete/steelandconcrete.flags";
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
