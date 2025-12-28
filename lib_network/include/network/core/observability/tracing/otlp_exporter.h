#ifndef NETWORKLIB_CORE_OBSERVABILITY_TRACING_OTLP_EXPORTER_H
#define NETWORKLIB_CORE_OBSERVABILITY_TRACING_OTLP_EXPORTER_H

#include "network/core/observability/tracing/tracer.h"
#include <string>
#include <vector>

namespace networklib {
namespace core {
namespace observability {

class OtlpExporter {
public:
    explicit OtlpExporter(const std::string& endpoint);
    
    void Export(const std::vector<std::shared_ptr<Span>>& spans);

private:
    std::string endpoint_;
    // In production, we'd hold a persistent HTTP client or gRPC channel here
};

} // namespace observability
} // namespace core
} // namespace networklib

#endif // NETWORKLIB_CORE_OBSERVABILITY_TRACING_OTLP_EXPORTER_H
