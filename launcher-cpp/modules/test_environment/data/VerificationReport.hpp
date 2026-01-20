//
// Created by bimba on 1/15/26.
//

#ifndef VERIFICATIONREPORT_HPP
#define VERIFICATIONREPORT_HPP
#include "VerificationResult.hpp"

struct VerificationReport
{
    bool verified{false};
    double similarity{0.0}; // 0.0 - 1.0
    int64_t estimated_delay_ms{0}; // Примерная задержка
    uint64_t reference_id{0};
    VerificationError error{VerificationError::NONE};
    size_t matched_features{0}; // Количество совпавших признаков
    size_t total_features{0}; // Всего признаков

    [[nodiscard]] const char* error_string() const
    {
        switch (error)
        {
        case VerificationError::NONE: return "Verified successfully";
        case VerificationError::NO_REFERENCES: return "No reference audio available";
        case VerificationError::EMPTY_TEST_SIGNAL: return "Empty test signal";
        case VerificationError::LOW_SIMILARITY: return "Similarity below threshold";
        case VerificationError::SIGNAL_TOO_QUIET: return "Signal too quiet";
        case VerificationError::FINGERPRINT_MISMATCH: return "Fingerprint mismatch";
        case VerificationError::TOO_MANY_DIFFERENCES: return "Too many differences detected";
        default: return "Unknown error";
        }
    }
};

#endif //VERIFICATIONREPORT_HPP
