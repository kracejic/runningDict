//-----------------------------------------------------------------------------
#ifdef UNIT_TESTS
#include "catch.hpp"

#include <cpr/cpr.h>

TEST_CASE("CPR test", "[server][!mayfail]")
{
    auto r = cpr::Get(cpr::Url{"https://google.com"}, cpr::Timeout{2000});

    REQUIRE(r.status_code == 200);
}

#endif
