//-----------------------------------------------------------------------------
#ifdef UNIT_TESTS
#include "catch.hpp"

#include <cpr/cpr.h>

TEST_CASE("CPR test", "[!mayfail]")
{
    auto r = cpr::Get(cpr::Url{"https://google.com"});

    REQUIRE(r.status_code == 200);
}

#endif
