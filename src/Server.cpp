#include <pch.h>

#include <ServerGame.h>
#include <gflags/gflags.h>

#include <ReleaseVersion.h>
#include <ProtocolVersion.h>

DEFINE_bool(short_version, false, "Short version to use in build scripts");
DEFINE_string(address, "localhost", "Server address");
DEFINE_int32(port, 4512, "Port");
DEFINE_int32(size, 4, "Map size: 1 - 162, 2 - 642, 3 - 2562, 4 - 10242, 5 - 40962, 6 - 163842, 7 - 655362 tiles");

void RunServer(int argc, char **argv)
{
    google::InstallFailureSignalHandler();
    google::InitGoogleLogging(argv[0]);
	google::ParseCommandLineFlags(&argc, &argv, true);

	// Define the command line object.
	Ogre::String version = Ogre::StringConverter::toString(PROTOCOL_VERSION) +
		"." + Ogre::StringConverter::toString(RELEASE_VERSION);


	if (FLAGS_short_version)
    {
        std::cout << version;
        return;
    }

	ServerGame app(FLAGS_size);
	app.MainLoop(FLAGS_address, FLAGS_port);
}
