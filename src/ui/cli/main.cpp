/**
 * @file main.cpp
 * @brief CLI application entry point
 */

#include "calc/ui/cli/cli_app.h"

int main(int argc, char* argv[]) {
    using namespace calc::cli;

    // Create CLI app with command line arguments
    CliApp app(argc, argv);

    // Run
    return app.run();
}
