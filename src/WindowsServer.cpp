#include <pch.h>
#include <ServerGame.h>
#include <tclap/CmdLine.h>

#include <iostream>
#include <svn_revision.h>
#include <ProtocolVersion.h>

int main(int argc, char **argv)
{
    try
    {
        // Define the command line object.
        Ogre::String version = Ogre::StringConverter::toString(ProtocolVersion) +
            "." + SVN_REVISION;
        TCLAP::CmdLine cmd("Steelandconcrete server", ' ', version.c_str());

        TCLAP::ValueArg<Ogre::String> address("a", "address", "Server address", false, "localhost", "string");
        cmd.add(address);

        TCLAP::ValueArg<Ogre::String> port("p", "port", "Port", false, "4512", "string");
        cmd.add(port);

        TCLAP::ValueArg<int32> size("s", "size",
            "Map size: 1 - 162, 2 - 642, 3 - 2562, 4 - 10242, 5 - 40962, 6 - 163842, 7 - 655362 tiles",
            false, 4, "int");
        cmd.add(size);

        TCLAP::ValueArg<int32> seaLevel("l", "sealevel",
            "Sea level: 0-10000",
            false, 5000, "int");
        cmd.add(seaLevel);

        // Parse the args.
        cmd.parse( argc, argv );

        ServerGame app(size.getValue(), seaLevel.getValue());
        app.MainLoop(address.getValue(), port.getValue());
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