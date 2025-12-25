#include "networklib/observability/metrics/metrics_collector.h"

namespace networklib {
namespace observability {
namespace metrics {

void MetricsCollector::IncrementCounter(const std::string& name) {
    // Note: atomic inside map is not directly accessible if map resizes, 
    // but standard map nodes are stable.
    // However, inserting needs lock.
    std::lock_guard<std::mutex> lock(mutex_);
    counters_[name]++;
}

void MetricsCollector::SetGauge(const std::string& name, int64_t value) {
    std::lock_guard<std::mutex> lock(mutex_);
    gauges_[name] = value;
}

void MetricsCollector::ObserveHistogram(const std::string& name, double value) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (histograms_.find(name) == histograms_.end()) {
        histograms_[name] = std::make_unique<Histogram>();
    }
    auto& h = histograms_[name];
    h->sum = h->sum + value; // atomic float add is C++20 feature (using double here with mutex is safer/simpler if not strict 20 atomic float)
    h->count++;
}

std::string MetricsCollector::GetPrometheusOutput() {
    std::lock_guard<std::mutex> lock(mutex_);
    std::stringstream ss;
    
    for (const auto& [name, val] : counters_) {
        ss << "# TYPE " << name << " counter\n";
        ss << name << " " << val << "\n";
    }
    
    for (const auto& [name, val] : gauges_) {
        ss << "# TYPE " << name << " gauge\n";
        ss << name << " " << val << "\n";
    }

    for (const auto& [name, h] : histograms_) {
        ss << "# TYPE " << name << "_sum counter\n";
        ss << name << "_sum " << h->sum << "\n";
        ss << "# TYPE " << name << "_count counter\n";
        ss << name << "_count " << h->count << "\n";
    }
    
    return ss.str();
}

} // namespace metrics
} // namespace observability
} // namespace networklib
