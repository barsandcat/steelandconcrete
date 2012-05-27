#include <pch.h>
#include <ClientApp.h>

#include <Platform.h>

int main(int argc, char **argv)
{
    google::InstallFailureSignalHandler();
    google::InitGoogleLogging(argv[0]);
    FLAGS_alsologtostderr = true;
    google::ParseCommandLineFlags(&argc, &argv, true);

    // Create application object
    try
    {
        Ogre::String home = getenv("HOME");
        CheckConfigFile(home);
        ClientApp app(home + "/.steelandconcrete/steelandconcrete1.cfg");
        LOG(INFO) << "ClientApp ready";
        app.MainLoop();
        LOG(INFO) << "Main loop ended";
    }
    catch (std::exception& e)
    {
        std::cerr << "An exception has occured: " << e.what();
    }
    catch (...)
    {
        std::cerr << "Exception!";
    }
    google::ShutdownGoogleLogging();
    // this is to save gmon.out
    return chdir("cpd");
}
