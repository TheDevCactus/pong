#include <iostream>
#include "GameConfig.h"

int main(int argc, char** argv) {
    std::cout << argv[0] << " Version " << Game_VERSION_MAJOR << "."
        << Game_VERSION_MINOR << std::endl;
    std::cout << "Usage: " << argv[0] << " number" << std::endl;

    return 0;
}
