#pragma once

#include <cstdint>
#include <string>
#include <unordered_map>

struct TitleNameResolver {
    std::unordered_map<std::uint64_t, std::string> byId;
};

TitleNameResolver BuildTitleNameResolver();
std::string ResolveTitleName(const TitleNameResolver &resolver, std::uint64_t titleId);
