#include "spdlog/spdlog.h"
#include <memory>

namespace logging
{
void init(const std::string& directory = "./");
}

extern std::shared_ptr<spdlog::logger> L;
