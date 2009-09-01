#include <pch.h>
#include <ServerGame.h>
#include <tclap/CmdLine.h>

#include <iostream>


int main(int argc, char **argv)
{
    try
    {
        // Define the command line object.
        TCLAP::CmdLine cmd("Steelandconcrete server", ' ');

        TCLAP::ValueArg<Ogre::String> address("a", "address", "Server address", false, "localhost", "string");
        cmd.add(address);

        TCLAP::ValueArg<Ogre::String> port("p", "port", "Port", false, "4512", "string");
        cmd.add(port);

        TCLAP::ValueArg<int> size("s", "size",
            "Map size: 1 - 162, 2 - 642, 3 - 2562, 4 - 10242, 5 - 40962, 6 - 163842, 7 - 655362 tiles",
            false, 4, "int");
        cmd.add(size);

        // Parse the args.
        cmd.parse( argc, argv );

        ServerGame app(size.getValue());
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
