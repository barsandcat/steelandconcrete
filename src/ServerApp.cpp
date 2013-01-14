#include <pch.h>

#include <ServerGame.h>
#include <gflags/gflags.h>

#include <ReleaseVersion.h>
#include <ProtocolVersion.h>
#include <ConnectionManager.h>

DEFINE_bool(short_version, false, "Short version to use in build scripts");
DEFINE_string(address, "localhost", "Server address");
DEFINE_int32(port, 4512, "Port");
DEFINE_int32(size, 4, "Map size: 1 - 162, 2 - 642, 3 - 2562, 4 - 10242, 5 - 40962, 6 - 163842, 7 - 655362 tiles");

void Run(int argc, char **argv, const bool& aContinue)
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

        while (aContinue)
        {
            game.Update();
        }
        LOG(INFO) << "Game over";
    }

	google::ShutdownGoogleLogging();
}
