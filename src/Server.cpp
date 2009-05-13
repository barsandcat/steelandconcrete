#include <iostream>
#include <ServerGame.h>

int main(int argc, char **argv)
{
    try
    {
        ServerGame app;
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
