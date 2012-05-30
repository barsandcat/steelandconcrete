#include <pch.h>
#include <ClientApp.h>

#include <Platform.h>
#include <boost/filesystem/operations.hpp>

int main(int argc, char **argv)
{
    google::InstallFailureSignalHandler();

    // Create application object
    try
    {
        ClientApp app(argc, argv);
        app.MainLoop();
    }
    catch (std::exception& e)
    {
        std::cerr << "An exception has occured: " << e.what();
    }
    catch (...)
    {
        std::cerr << "Exception!";
    }
    // this is to save gmon.out
    return chdir("cpd");
}
