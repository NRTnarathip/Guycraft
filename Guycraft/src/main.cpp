#include <ClientEngine.h>
#include <iostream>
// My Game

class LaunchConfig {
public:
    enum LaunchType {
        Client, Server
    };
    LaunchConfig(int& argc, char** argv) {
        for (int i = 1; i < argc; ++i)
            this->tokens.push_back(std::string(argv[i]));
        type = Client;
    }
    const std::string& getCmdOption(const std::string& option) const {
        std::vector<std::string>::const_iterator itr;
        itr = std::find(this->tokens.begin(), this->tokens.end(), option);
        if (itr != this->tokens.end() && ++itr != this->tokens.end()) {
            return *itr;
        }
        static const std::string empty_string("");
        return empty_string;
    }
    bool cmdOptionExists(const std::string& option) const {
        return std::find(this->tokens.begin(), this->tokens.end(), option)
            != this->tokens.end();
    }

    LaunchType type;
private:
    std::vector <std::string> tokens;
};
int main(int argc, char** argv)
{
    LaunchConfig launchConfig(argc, argv);
    if (launchConfig.cmdOptionExists("-server")) {
        //run server
        printf("No Server Enine!!!\n");
        printf("Press enter key to close window\n");
        std::cin.get();
        return 0;
    }
    //else run client standalone
    ClientEngine clientEngine;
    int err = clientEngine.launch(); //while loop main game
    return err;
}
