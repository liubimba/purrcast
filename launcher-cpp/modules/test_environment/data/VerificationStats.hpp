//
// Created by bimba on 1/15/26.
//

#ifndef VERIFICATIONSTATS_HPP
#define VERIFICATIONSTATS_HPP
#include <cstdint>
#include <map>
enum class VerificationError;

struct Stats
{
    uint64_t total_references{0};
    uint64_t total_verifications{0};
    uint64_t successful_verifications{0};
    std::map<VerificationError, uint64_t> error_counts;
};

#endif //VERIFICATIONSTATS_HPP
