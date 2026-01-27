//
// Created by bimba on 1/15/26.
//

#include "fingerprint_audio_inspector.hpp"

fingerprint_audio_inspector::fingerprint_audio_inspector(int sample_rate)
    : expected_sample_rate_(sample_rate)
{
}

std::vector<float> fingerprint_audio_inspector::to_mono(const audio_chunk& chunk)
{
    std::vector<float> mono(chunk.frames_per_buffer);

    if (chunk.bytes_per_sample == 2)
    {
        const auto* samples = static_cast<const int16_t*>(chunk.data);
        for (size_t i = 0; i < chunk.frames_per_buffer; ++i)
        {
            float sum = 0.0f;
            for (int ch = 0; ch < chunk.channels; ++ch)
            {
                sum += samples[i * chunk.channels + ch] / 32768.0f;
            }
            mono[i] = sum / chunk.channels;
        }
    }
    else if (chunk.bytes_per_sample == 4)
    {
        const auto* samples = static_cast<const float*>(chunk.data);
        for (size_t i = 0; i < chunk.frames_per_buffer; ++i)
        {
            float sum = 0.0f;
            for (int ch = 0; ch < chunk.channels; ++ch)
            {
                sum += samples[i * chunk.channels + ch];
            }
            mono[i] = sum / chunk.channels;
        }
    }

    return mono;
}

SpectralFingerprint fingerprint_audio_inspector::create_fingerprint(const std::vector<float>& signal)
{
    SpectralFingerprint fp;

    if (signal.empty()) return fp;

    const size_t N = signal.size();

    float energy_sum = 0.0f;
    for (float sample : signal)
    {
        energy_sum += sample * sample;
    }
    fp.rms_energy = std::sqrt(energy_sum / N);

    uint32_t zero_crossings = 0;
    for (size_t i = 1; i < N; ++i)
    {
        if ((signal[i - 1] >= 0 && signal[i] < 0) ||
            (signal[i - 1] < 0 && signal[i] >= 0))
        {
            zero_crossings++;
        }
    }
    fp.zero_crossing_rate = zero_crossings;

    const size_t band_size = N / SpectralFingerprint::NUM_BANDS;

    for (size_t band = 0; band < SpectralFingerprint::NUM_BANDS; ++band)
    {
        float band_energy = 0.0f;
        size_t start = band * band_size;
        size_t end = std::min(start + band_size, N);

        for (size_t i = start + 1; i < end; ++i)
        {
            float diff = signal[i] - signal[i - 1];
            float freq_weight = (band + 1) /
                static_cast<float>(SpectralFingerprint::NUM_BANDS);
            band_energy += diff * diff * freq_weight;
        }

        if (end > start)
        {
            fp.energy_bands[band] = std::sqrt(band_energy / (end - start));
        }
    }

    float max_band_energy = 0.0f;
    for (float e : fp.energy_bands)
    {
        max_band_energy = std::max(max_band_energy, e);
    }

    if (max_band_energy > 1e-6f)
    {
        for (float& e : fp.energy_bands)
        {
            e /= max_band_energy;
        }
    }

    return fp;
}

bool fingerprint_audio_inspector::is_silent(const std::vector<float>& signal)
{
    if (signal.empty()) return true;

    float rms = 0.0f;
    for (float sample : signal)
    {
        rms += sample * sample;
    }
    rms = std::sqrt(rms / signal.size());

    return rms < SILENCE_THRESHOLD;
}

void fingerprint_audio_inspector::cleanup_old_references_()
{
    auto now = std::chrono::steady_clock::now();

    while (!reference_buffer_.empty())
    {
        auto age = std::chrono::duration_cast<std::chrono::milliseconds>(
            now - reference_buffer_.front().timestamp
        ).count();

        if (age > FINGERPRINT_TIMEOUT_MS)
        {
            reference_buffer_.pop_front();
        }
        else
        {
            break;
        }
    }
}

void fingerprint_audio_inspector::update_error_stats_(const verification_error error)
{
    stats_.error_counts[error]++;
}


fingerprint_audio_inspector::~fingerprint_audio_inspector() = default;

void fingerprint_audio_inspector::add_reference(const audio_chunk& chunk, uint64_t seq)
{
    std::vector<float> signal = to_mono(chunk);

    if (is_silent(signal))
    {
        return;
    }

    SpectralFingerprint fp = create_fingerprint(signal);
    fp.seq = seq;
    fp.timestamp = std::chrono::steady_clock::now();

    reference_buffer_.push_back(fp);

    cleanup_old_references_();

    while (reference_buffer_.size() > MAX_FINGERPRINTS)
    {
        reference_buffer_.pop_front();
    }

    stats_.total_references++;
}

verification_report fingerprint_audio_inspector::verify(const audio_chunk& chunk)
{
    verification_report report;
    stats_.total_verifications++;

    if (reference_buffer_.empty())
    {
        report.error = verification_error::NO_REFERENCES;
        update_error_stats_(report.error);
        return report;
    }

    std::vector<float> test_signal = to_mono(chunk);
    if (test_signal.empty())
    {
        report.error = verification_error::EMPTY_TEST_SIGNAL;
        update_error_stats_(report.error);
        return report;
    }

    if (is_silent(test_signal))
    {
        report.error = verification_error::SIGNAL_TOO_QUIET;
        update_error_stats_(report.error);
        return report;
    }

    SpectralFingerprint test_fp = create_fingerprint(test_signal);

    double best_similarity = 0.0;
    uint64_t best_ref_id = 0;
    size_t best_match_count = 0;
    int64_t best_delay_estimate = 0;

    auto now = std::chrono::steady_clock::now();

    for (const auto& ref : reference_buffer_)
    {
        double sim = test_fp.similarity(ref);

        if (sim > best_similarity)
        {
            best_similarity = sim;
            best_ref_id = ref.seq;

            best_delay_estimate = std::chrono::duration_cast<
                std::chrono::milliseconds>(now - ref.timestamp).count();

            best_match_count = 0;
            for (size_t i = 0; i < SpectralFingerprint::NUM_BANDS; ++i)
            {
                if (std::abs(test_fp.energy_bands[i] - ref.energy_bands[i]) < 0.2f)
                {
                    best_match_count++;
                }
            }
        }
    }

    report.similarity = best_similarity;
    report.reference_id = best_ref_id;
    report.estimated_delay_ms = best_delay_estimate;
    report.matched_features = best_match_count;
    report.total_features = SpectralFingerprint::NUM_BANDS;

    if (best_similarity < VERIFICATION_THRESHOLD)
    {
        report.error = verification_error::LOW_SIMILARITY;
        report.verified = false;
    }
    else if (best_match_count < SpectralFingerprint::NUM_BANDS / 2)
    {
        report.error = verification_error::TOO_MANY_DIFFERENCES;
        report.verified = false;
    }
    else
    {
        report.error = verification_error::NONE;
        report.verified = true;
    }

    if (report.verified)
    {
        stats_.successful_verifications++;
    }

    update_error_stats_(report.error);
    return report;
}

const verification_stats& fingerprint_audio_inspector::get_stats() const
{
    return stats_;
}

void fingerprint_audio_inspector::reset_stats()
{
    stats_ = verification_stats{};
}

void fingerprint_audio_inspector::clear_references()
{
    reference_buffer_.clear();
}
