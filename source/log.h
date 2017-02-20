#include "spdlog/spdlog.h"
#include <memory>

namespace logging
{
void init(std::string directory = ".");
}

extern std::shared_ptr<spdlog::logger> L;
