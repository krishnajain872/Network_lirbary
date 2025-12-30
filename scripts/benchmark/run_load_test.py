import os
import subprocess
import time
import json
import argparse
from concurrent.futures import ThreadPoolExecutor

class BenchmarkRunner:
    def __init__(self, server_bin, client_bin, output_dir="benchmark_results"):
        self.server_bin = server_bin
        self.client_bin = client_bin
        self.output_dir = output_dir
        if not os.path.exists(output_dir):
            os.makedirs(output_dir)

    def run_server(self, config_path):
        cmd = [self.server_bin, config_path]
        print(f"Starting server: {' '.join(cmd)}")
        # Use Popen to run in background
        self.server_proc = subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        time.sleep(2) # Warmup
        return self.server_proc

    def run_client_load(self, protocol, clients=1, messages=1000, rate=0):
        """
        Runs a load test using super_client or a specialized load tool.
        Since super_client is scenario based, we generate a temp scenario for load.
        """
        scenario_file = self.generate_load_scenario(protocol, messages)

        results = []
        with ThreadPoolExecutor(max_workers=clients) as executor:
            futures = [executor.submit(self._execute_client, scenario_file) for _ in range(clients)]
            for f in futures:
                results.append(f.result())

        return self.aggregate_results(results)

    def _execute_client(self, scenario_path):
        start = time.time()
        cmd = [self.client_bin, scenario_path]
        proc = subprocess.run(cmd, capture_output=True, text=True)
        duration = time.time() - start
        return {
            "success": proc.returncode == 0,
            "duration": duration,
            "output": proc.stdout
        }

    def generate_load_scenario(self, protocol, messages):
        filename = f"{self.output_dir}/load_{protocol}_{messages}.yaml"
        # We assume target based on protocol/server config defaults
        target_map = {
            "tcp_raw": "127.0.0.1:9001",
            "tcp_proto": "127.0.0.1:9002",
            "udp_raw": "127.0.0.1:9003",
            "udp_proto": "127.0.0.1:9004",
            "tcp_secure": "127.0.0.1:9011"
        }
        target = target_map.get(protocol, "127.0.0.1:9001")
        mode = "raw" if "raw" in protocol else "proto"
        type_ = "udp" if "udp" in protocol else "tcp"
        secure = "secure" in protocol

        steps = []
        if type_ == "tcp":
            steps.append({"action": "connect"})
        elif type_ == "udp":
            # UPDATED: UDP requires connect action in ScenarioEngine now to init client
            steps.append({"action": "connect"})

        # Use loop construct
        steps.append({
            "action": "loop",
            "count": messages,
            "steps": [
                {"action": "send", "data": "LoadData1234567890"},
                {"action": "expect", "data": "LoadData1234567890"} # Expect echo
            ]
        })

        if type_ == "tcp" or type_ == "udp":
            steps.append({"action": "disconnect"})

        scenario = {
            "scenarios": [{
                "name": f"Load {protocol}",
                "type": type_,
                "mode": mode,
                "secure": secure,
                "target": target,
                "steps": steps
            }]
        }

        import yaml
        with open(filename, "w") as f:
            yaml.dump(scenario, f)
        return filename

    def aggregate_results(self, results):
        total_time = sum(r["duration"] for r in results)
        success_count = sum(1 for r in results if r["success"])
        return {
            "total_clients": len(results),
            "success_count": success_count,
            "avg_duration": total_time / len(results) if results else 0
        }

    def cleanup(self):
        if self.server_proc:
            self.server_proc.kill()

if __name__ == "__main__":
    runner = BenchmarkRunner("./build-Debug/bin/super_server", "./build-Debug/bin/super_client")
    try:
        runner.run_server("apps/super_server/server_config.yaml")

        print("Running TCP Raw Load Test...")
        res = runner.run_client_load("tcp_raw", clients=5, messages=100)
        print(json.dumps(res, indent=2))

        print("Running TCP Proto Load Test...")
        res = runner.run_client_load("tcp_proto", clients=5, messages=100)
        print(json.dumps(res, indent=2))

    finally:
        runner.cleanup()
