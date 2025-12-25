#ifndef NETWORKLIB_CORE_OBSERVABILITY_TRACING_TRACER_H
#define NETWORKLIB_CORE_OBSERVABILITY_TRACING_TRACER_H

#include <string>
#include <memory>
#include <map>
#include <chrono>
#include <vector>

namespace networklib {
namespace core {
namespace observability {

struct SpanContext {
    std::string trace_id;
    std::string span_id;
    std::string parent_span_id;
    bool sampled;
};

struct Span {
    std::string name;
    SpanContext context;
    long long start_time_us;
    long long end_time_us;
    std::map<std::string, std::string> attributes;
    
    void End();
};

class Tracer {
public:
    static Tracer& Instance();

    std::shared_ptr<Span> StartSpan(const std::string& name, const SpanContext& parent = SpanContext());
    
    void Export(std::shared_ptr<Span> span);

    // Helpers for W3C Trace Context
    static SpanContext Extract(const std::map<std::string, std::string>& headers);
    static void Inject(const SpanContext& ctx, std::map<std::string, std::string>& headers);

private:
    Tracer() = default;
    
    std::string GenerateId(int length);
};

} // namespace observability
} // namespace core
} // namespace networklib

#endif // NETWORKLIB_CORE_OBSERVABILITY_TRACING_TRACER_H
