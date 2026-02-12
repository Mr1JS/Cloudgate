/*
 *  Main.cpp *
 *  Created on: Dec 08, 2017
 *      Author: Thomas Wiemann
 *
 *  Copyright (c) 2017 Thomas Wiemann.
 *  Restricted usage. Licensed for participants of the course "The C++ Programming Language" only.
 *  No unauthorized distribution.
 */

#include "include/Level.hpp"
#include "include/MainWindow.hpp"

#include <iostream>

using namespace jumper;

int main(int argc, char** argv)
{
    if (argc != 2)
    {
        std::cout << "Please specify a level to render" << std::endl;
        return -1;
    }

    MainWindow window("Jumper", std::string(argv[1]), 800, 600);
    window.run();
}
