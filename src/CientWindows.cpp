#include <pch.h>
#include <ClientApp.h>


INT WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT)
{
    try
    {
        ClientApp app(__argc, __argv);
        app.MainLoop();
    }
    catch (std::exception& e)
    {
        MessageBox(NULL, e.what(), "Steelandconcrete exception!", MB_OK | MB_ICONERROR | MB_TASKMODAL);
    }
    catch (...)
    {
        MessageBox(NULL, "Unknown excepiton", "Steelandconcrete occured!", MB_OK | MB_ICONERROR | MB_TASKMODAL);
    }
    google::ShutdownGoogleLogging();
    return 0;

}
