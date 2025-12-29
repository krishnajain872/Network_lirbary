#include "scenario_engine.h"
#include "logger/logging.h"
#include <iostream>

int main(int argc, char** argv) {
    std::string scenarios_path = "apps/super_client/scenarios.yaml";
    if (argc > 1) {
        scenarios_path = argv[1];
    }

    networklib::logging::Logger::Initialize("severity=info;console=true;");

    try {
        auto scenarios = ScenarioEngine::Parse(scenarios_path);
        bool all_passed = true;

        for (const auto& scenario : scenarios) {
            if (!ScenarioEngine::Execute(scenario)) {
                LOG_ERROR("Scenario '%s' FAILED", scenario.name.c_str());
                all_passed = false;
            } else {
                LOG_INFO("Scenario '%s' PASSED", scenario.name.c_str());
            }
        }

        return all_passed ? 0 : 1;
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        return 1;
    }
}
