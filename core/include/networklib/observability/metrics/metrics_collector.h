#ifndef NETWORKLIB_OBSERVABILITY_METRICS_COLLECTOR_H
#define NETWORKLIB_OBSERVABILITY_METRICS_COLLECTOR_H

#include <map>
#include <string>
#include <atomic>
#include <mutex>
#include <sstream>
#include <memory>

namespace networklib {
namespace observability {
namespace metrics {

class MetricsCollector {
public:
    static MetricsCollector& Instance() {
        static MetricsCollector instance;
        return instance;
    }

    void IncrementCounter(const std::string& name);
    void SetGauge(const std::string& name, int64_t value);
    void ObserveHistogram(const std::string& name, double value);

    std::string GetPrometheusOutput();

private:
    MetricsCollector() = default;
    
    std::map<std::string, std::atomic<int64_t>> counters_;
    std::map<std::string, std::atomic<int64_t>> gauges_;
    // Histogram simplified for Phase 12 (sum/count)
    struct Histogram {
        std::atomic<double> sum{0};
        std::atomic<int64_t> count{0};
    };
    std::map<std::string, std::unique_ptr<Histogram>> histograms_;
    std::mutex mutex_;
};

} // namespace metrics
} // namespace observability
} // namespace networklib

#endif // NETWORKLIB_OBSERVABILITY_METRICS_COLLECTOR_H
