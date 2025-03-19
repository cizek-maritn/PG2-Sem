#include "LoggerClass.h"

void LoggerClass::debug(const std::string& msg) {
	LoggerClass::log(Severity::DEBUG, msg);
}

void LoggerClass::info(const std::string& msg) {
	LoggerClass::log(Severity::INFO, msg);
}

void LoggerClass::warning(const std::string& msg) {
	LoggerClass::log(Severity::WARNING, msg);
}

void LoggerClass::error(const std::string& msg) {
	LoggerClass::log(Severity::ERR, msg);
}

void LoggerClass::critical(const std::string& msg) {
	LoggerClass::log(Severity::CRITICAL , msg);
}