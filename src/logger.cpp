
#include "neocpp/logger.hpp"

namespace neocpp {

// Initialize static members
LogLevel Logger::currentLevel_ = LogLevel::INFO;
std::mutex Logger::logMutex_;
bool Logger::colorEnabled_ = true;

} // namespace neocpp
