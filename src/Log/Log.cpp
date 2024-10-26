#include "Log.h"

#include <spdlog/sinks/stdout_color_sinks.h>

namespace REASM {

std::shared_ptr<spdlog::logger> Log::s_Logger;
	
void Log::Init() {
	spdlog::set_pattern("%^[%l]: %v%$");
	s_Logger = spdlog::stdout_color_mt("REASM");
	s_Logger->set_level(spdlog::level::trace);
}

}