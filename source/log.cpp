#include "log.h"
#include "version.h"
#include <iostream>

#ifdef USE_BOOST_FILESYSTEM
#include <boost/filesystem.hpp>
namespace fs = boost::filesystem;
#else
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;
#endif

using namespace std;

std::shared_ptr<spdlog::logger> L;

void logging::init(const std::string& directory)
{
    static bool done = false;
    if (done == true)
        return;
    done = true;

    auto filepath = fs::path(directory) / "logfile";
    if (not fs::exists(fs::path(directory)))
        create_directories(fs::path(directory));

    vector<spdlog::sink_ptr> sinks;
    sinks.push_back(make_shared<spdlog::sinks::stdout_sink_mt>());
    sinks.push_back(std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
        filepath.string(), 1048576 * 2, 3));

    L = make_shared<spdlog::logger>("main", sinks.begin(), sinks.end());

    L->set_pattern("%H:%M:%S.%f %L %v");

    // register it if you need to access it globally
    spdlog::register_logger(L);

    L->info("=== App Started ===");
    L->info("Version: {}", Version::getVersionLong());
}
