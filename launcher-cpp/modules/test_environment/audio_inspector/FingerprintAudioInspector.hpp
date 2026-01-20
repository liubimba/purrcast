//
// Created by bimba on 1/15/26.
//

#ifndef FINGERPINTAUDIOINSPECTOR_HPP
#define FINGERPINTAUDIOINSPECTOR_HPP
#include <array>
#include <chrono>
#include <cstdint>
#include <cmath>
#include <deque>
#include <string>
#include <vector>

#include "IAudioInspector.hpp"
#include "../data/VerificationStats.hpp"
#include "../data/VerificationReport.hpp"
#include "../data/VerificationResult.hpp"

#include "../../services/Services.hpp"

struct SpectralFingerprint
{
    static constexpr size_t NUM_BANDS = 8;

    uint64_t seq{0};
    float rms_energy{0.0f};
    uint32_t zero_crossing_rate{0};
    std::array<float, NUM_BANDS> energy_bands{};
    std::chrono::steady_clock::time_point timestamp;

    [[nodiscard]] uint64_t hash() const
    {
        uint64_t h = 0;
        for (size_t i = 0; i < NUM_BANDS; ++i)
        {
            uint8_t quantized = static_cast<uint8_t>(
                std::min(7.0f, energy_bands[i] * 8.0f)
            );
            h |= (static_cast<uint64_t>(quantized) << (i * 8));
        }
        return h;
    }

    [[nodiscard]] double similarity(const SpectralFingerprint& other) const
    {
        double band_diff = 0.0;
        for (size_t i = 0; i < NUM_BANDS; ++i)
        {
            double diff = std::abs(energy_bands[i] - other.energy_bands[i]);
            band_diff += diff * diff;
        }
        band_diff = std::sqrt(band_diff / NUM_BANDS);
        double band_similarity = std::max(0.0, 1.0 - band_diff);

        double zcr_diff = std::abs(
            static_cast<int>(zero_crossing_rate) -
            static_cast<int>(other.zero_crossing_rate)
        ) / 1000.0;
        double zcr_similarity = std::max(0.0, 1.0 - zcr_diff);

        double energy_ratio = std::min(rms_energy, other.rms_energy) /
            std::max(rms_energy, other.rms_energy);
        if (std::isnan(energy_ratio) || std::isinf(energy_ratio))
        {
            energy_ratio = 0.0;
        }

        return 0.6 * band_similarity +
            0.2 * zcr_similarity +
            0.2 * energy_ratio;
    }
};

class FingerprintAudioInspector : public IAudioInspector
{
    static constexpr size_t MAX_FINGERPRINTS = 100;
    static constexpr int64_t FINGERPRINT_TIMEOUT_MS = 5000;
    static constexpr double SILENCE_THRESHOLD = 0.01;
    static constexpr double VERIFICATION_THRESHOLD = 0.75;
    Stats stats_;
    int expected_sample_rate_{48000};
    std::deque<SpectralFingerprint> reference_buffer_;

    static std::vector<float> to_mono(const audio_chunk& chunk);
    static SpectralFingerprint create_fingerprint(const std::vector<float>& signal);
    static bool is_silent(const std::vector<float>& signal);

    void cleanup_old_references_();
    void update_error_stats_(const VerificationError error);

public:
    explicit FingerprintAudioInspector(int sample_rate = 48000);
    ~FingerprintAudioInspector() override;
    void add_reference(const audio_chunk& chunk, uint64_t seq) override;
    VerificationReport verify(const audio_chunk& chunk) override;

    [[nodiscard]] const Stats& get_stats() const override;
    void reset_stats() override;
    void clear_references();
};


#endif //FINGERPINTAUDIOINSPECTOR_HPP
