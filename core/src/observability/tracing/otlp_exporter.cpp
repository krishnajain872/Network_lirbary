#include "networklib/core/observability/tracing/otlp_exporter.h"
#include "networklib/logging.h"
#include <iostream>
#include <sstream>

namespace networklib {
namespace core {
namespace observability {

OtlpExporter::OtlpExporter(const std::string& endpoint) : endpoint_(endpoint) {}

void OtlpExporter::Export(const std::vector<std::shared_ptr<Span>>& spans) {
    // Construct OTLP/JSON payload
    std::stringstream json;
    json << "{\"resourceSpans\":[{\"scopeSpans\":[{\"spans\":[";
    
    for (size_t i = 0; i < spans.size(); ++i) {
        auto s = spans[i];
        if (i > 0) json << ",";
        json << "{"
             << "\"traceId\":\"" << s->context.trace_id << "\","
             << "\"spanId\":\"" << s->context.span_id << "\","
             << "\"parentSpanId\":\"" << s->context.parent_span_id << "\","
             << "\"name\":\"" << s->name << "\","
             << "\"startTimeUnixNano\":\"" << s->start_time_us * 1000 << "\","
             << "\"endTimeUnixNano\":\"" << s->end_time_us * 1000 << "\""
             << "}";
    }
    json << "]}]}]}";

    // STUB: Send HTTP POST to endpoint_
    // Using std::cout to prove payload generation for Phase 21
    logging::Logger::Log(logging::LogLevel::Info, __FILE__, __LINE__, __FUNCTION__, "[OTLP] Exporting to %s: %s", endpoint_.c_str(), json.str().c_str());
}

} // namespace observability
} // namespace core
} // namespace networklib
