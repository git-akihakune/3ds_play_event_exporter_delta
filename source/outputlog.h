#pragma once

#include <string_view>

std::string_view OutputLogPath();
bool WriteLog(std::string_view contents);
