#define CATCH_CONFIG_RUNNER
#include "catch.hpp"
#include "log.h"

int main(int argc, char* argv[])
{
    // global setup...
    logging::init();

    int result = Catch::Session().run(argc, argv);

    // global clean-up...

    return (result < 0xff ? result : 0xff);
}
