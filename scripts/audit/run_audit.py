import os
import subprocess
import time
import json
import sys

class AuditReport:
    def __init__(self):
        self.results = {
            "unit_tests": {},
            "integration_tests": {},
            "load_tests": {},
            "scenarios": []
        }

    def run_command(self, cmd, description, cwd=None):
        print(f"Running: {description}")
        start_time = time.time()
        try:
            result = subprocess.run(cmd, shell=True, capture_output=True, text=True, cwd=cwd)
            duration = time.time() - start_time
            return {
                "command": cmd,
                "success": result.returncode == 0,
                "duration": duration,
                "output": result.stdout,
                "error": result.stderr
            }
        except Exception as e:
            return {
                "command": cmd,
                "success": False,
                "duration": time.time() - start_time,
                "error": str(e)
            }

    def run_unit_tests(self):
        # Assuming build is already done in build-Debug
        res = self.run_command("ctest -V", "Unit Tests (CTest)", cwd="build-Debug")
        self.results["unit_tests"] = res

    def run_integration_tests(self):
        # Use super_server and super_client
        # Start server
        server_cmd = "./build-Debug/bin/super_server"
        server_proc = subprocess.Popen(server_cmd.split(), stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        time.sleep(2) # Wait for startup

        # Run Client
        client_res = self.run_command("./build-Debug/bin/super_client", "Integration Scenarios")

        server_proc.kill()
        self.results["integration_tests"] = client_res

    def run_load_test(self):
        # Using oms_client benchmark scenario if available, or custom
        # Note: oms_client benchmark logic exists in memory context
        cmd = "./build-Debug/bin/oms_client apps/client/scenarios/benchmark_config.yaml"
        # We might need to create this config or assume logic
        # For now, placeholder or using super_client repeated?
        # Let's assume super_client is functional test.
        # I'll create a simple load test script or use super_client in a loop?
        # Better: run super_client multiple times in parallel.
        pass

    def generate_report(self):
        with open("audit_report.json", "w") as f:
            json.dump(self.results, f, indent=4)

        # Markdown summary
        with open("audit_report.md", "w") as f:
            f.write("# Audit Report\n\n")

            f.write("## Unit Tests\n")
            s = self.results["unit_tests"]
            f.write(f"- Status: {'PASS' if s['success'] else 'FAIL'}\n")
            f.write(f"- Duration: {s['duration']:.2f}s\n")

            f.write("## Integration Tests\n")
            s = self.results["integration_tests"]
            f.write(f"- Status: {'PASS' if s['success'] else 'FAIL'}\n")
            f.write(f"- Duration: {s['duration']:.2f}s\n")

            if not s['success']:
                f.write("### Error Output\n")
                f.write(f"```\n{s['error']}\n{s['output']}\n```\n")

if __name__ == "__main__":
    report = AuditReport()
    report.run_unit_tests()
    report.run_integration_tests()
    report.generate_report()
    print("Audit Complete. See audit_report.md")
