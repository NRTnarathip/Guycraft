#include <ClientEngine.h>
#include <Client/LaunchConfig.h>

// My Game

int main(int argc, char** argv)
//int WinMain()
{
    //LaunchConfig launchConfig(argc, argv);
    //if (launchConfig.cmdOptionExists("-server")) {
    //    //run server
    //    printf("No Server Enine!!!\n");
    //    printf("Press enter key to close window\n");
    //    std::cin.get();
    //    return 0;
    //}
    //else run client standalone
    ClientEngine clientEngine;
    int err = clientEngine.launch(); //while loop main game
    return err;
}
