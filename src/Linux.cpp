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
        Ogre::String home = getenv("HOME");
        CheckConfigFile(home);

        Ogre::String config = "steelandconcrete.flags";
        if (boost::filesystem::exists(config))
        {
            FLAGS_flagfile = config;
        }
        else
        {
            config = home + "/.steelandconcrete/steelandconcrete.flags";
            if (boost::filesystem::exists(config))
            {
                FLAGS_flagfile = config;
            }
        }


        google::ParseCommandLineFlags(&argc, &argv, true);
        google::InitGoogleLogging(argv[0]);

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
