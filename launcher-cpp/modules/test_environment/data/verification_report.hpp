//
// Created by bimba on 1/15/26.
//

#ifndef VERIFICATIONREPORT_HPP
#define VERIFICATIONREPORT_HPP
#include "verification_result.hpp"

struct verification_report
{
    bool verified{false};
    double similarity{0.0}; // 0.0 - 1.0
    int64_t estimated_delay_ms{0}; // Примерная задержка
    uint64_t reference_id{0};
    verification_error error{verification_error::NONE};
    size_t matched_features{0}; // Количество совпавших признаков
    size_t total_features{0}; // Всего признаков

    [[nodiscard]] const char* error_string() const
    {
        switch (error)
        {
        case verification_error::NONE: return "Verified successfully";
        case verification_error::NO_REFERENCES: return "No reference audio available";
        case verification_error::EMPTY_TEST_SIGNAL: return "Empty test signal";
        case verification_error::LOW_SIMILARITY: return "Similarity below threshold";
        case verification_error::SIGNAL_TOO_QUIET: return "Signal too quiet";
        case verification_error::FINGERPRINT_MISMATCH: return "Fingerprint mismatch";
        case verification_error::TOO_MANY_DIFFERENCES: return "Too many differences detected";
        default: return "Unknown error";
        }
    }
};

#endif //VERIFICATIONREPORT_HPP
