#include "log.h"

using namespace std;

std::shared_ptr<spdlog::logger> L;

void logging::init()
{
    static bool done = false;
    if (done == true)
        return;
    done = true;

    vector<spdlog::sink_ptr> sinks;
    sinks.push_back(make_shared<spdlog::sinks::stdout_sink_mt>());
    sinks.push_back(std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
        "logfile", "log", 1048576 * 2, 3));

    L = make_shared<spdlog::logger>("main", sinks.begin(), sinks.end());

    L->set_pattern("%H:%M:%S.%f %L %v");

    // register it if you need to access it globally
    spdlog::register_logger(L);

    L->info("=== App Started ===");
}
