#pragma once

#include <memory>
#include <stdio.h>

#include <spdlog/spdlog.h>

namespace REASM {

class Log {
public:
	static void Init();

	inline static std::shared_ptr<spdlog::logger>& GetLogger() { return s_Logger; }
private:
	static std::shared_ptr<spdlog::logger> s_Logger;
};

}

#define TRACE(...)		::REASM::Log::GetLogger()->trace(__VA_ARGS__)
#define INFO(...)		::REASM::Log::GetLogger()->info(__VA_ARGS__)
#define WARN(...)		::REASM::Log::GetLogger()->warn(__VA_ARGS__)
#define ERROR(...)		::REASM::Log::GetLogger()->error(__VA_ARGS__)