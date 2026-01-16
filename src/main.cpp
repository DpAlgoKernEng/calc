/**
 * @file main.cpp
 * @brief Entry point for the calc command-line calculator
 */

#include "calc/ui/cli/cli_app.h"
#include <iostream>

int main(int argc, char* argv[]) {
    calc::cli::CliApp app(argc, argv);
    return app.run();
}
