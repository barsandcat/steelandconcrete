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

        TCLAP::ValueArg<std::string> address("a", "address", "Server address", false, "localhost", "string");
        cmd.add(address);

        TCLAP::ValueArg<int> port("p", "port", "Port", false, 4512, "int");
        cmd.add(port);

        TCLAP::ValueArg<int> size("s", "size", "Map size", false, 4, "int");
        cmd.add(size);

        // Parse the args.
        cmd.parse( argc, argv );

        ServerGame app(address.getValue(), port.getValue(), size.getValue());
        app.MainLoop();
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
