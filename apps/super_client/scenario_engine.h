#ifndef SCENARIO_ENGINE_H
#define SCENARIO_ENGINE_H

#include "network/network_lib.h"
#include <string>
#include <vector>
#include <yaml-cpp/yaml.h>
#include <memory>
#include <functional>

struct Step {
    std::string action; // connect, send, expect, disconnect, loop
    std::string data;
    int count = 0; // For loop
    std::vector<Step> sub_steps; // For loop
};

struct Scenario {
    std::string name;
    std::string type;
    std::string mode;
    std::string target;
    bool secure = false;
    std::vector<Step> steps;
};

class ScenarioEngine {
public:
    static std::vector<Scenario> Parse(const std::string& filepath);
    static bool Execute(const Scenario& scenario);
};

#endif // SCENARIO_ENGINE_H
