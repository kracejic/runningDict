#include "spdlog/spdlog.h"
#include <memory>

namespace logging
{
void init();
}

extern std::shared_ptr<spdlog::logger> L;
