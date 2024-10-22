#include <pch.h>

#include <ServerApp.h>
#include <ServerGame.h>
#include <gflags/gflags.h>
#include <UnitList.h>
#include <ReleaseVersion.h>
#include <ProtocolVersion.h>
#include <ConnectionManager.h>

#ifndef _XOPEN_SOURCE_EXTENDED
# define _XOPEN_SOURCE_EXTENDED 1
#endif

#include <TUI.h>

DEFINE_bool(short_version, false, "Short version to use in build scripts");
DEFINE_string(address, "localhost", "Server address");
DEFINE_int32(port, 4512, "Port");
DEFINE_int32(size, 4, "Map size: 1 - 162, 2 - 642, 3 - 2562, 4 - 10242, 5 - 40962, 6 - 163842, 7 - 655362 tiles");

void GameLoop(ServerGame& aGame)
{
    while (true)
    {
        aGame.Update();
    }
}

void Run(int argc, char **argv)
{
    Ogre::String localConfig = "steelandconcrete_server.flags";
    if (boost::filesystem::exists(localConfig))
    {
        FLAGS_flagfile = localConfig;
    }

    google::InitGoogleLogging(argv[0]);
    google::ParseCommandLineFlags(&argc, &argv, true);

    if (FLAGS_short_version)
    {
        std::cout << PROTOCOL_VERSION << '.' << RELEASE_VERSION;
    }
    else
    {
        ServerGame game(FLAGS_size);
        boost::thread cm(ConnectionManager, boost::ref(game), FLAGS_address, FLAGS_port);
        boost::thread ml(GameLoop, boost::ref(game));

        try
        {
            TUI tui(argc, argv, game);
            tui.Run();
        }
        catch(std::exception& e)
        {
            LOG(INFO) << "TUI: " << e.what();
        }
        catch(...)
        {
            LOG(ERROR) << "TUI unknown exception!";
        }

        cm.interrupt();
        ml.interrupt();
        ml.join();
    }

    google::ShutdownGoogleLogging();
}

