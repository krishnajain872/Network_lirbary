# Test Suite & Audit

This directory contains the automated test suite for the Network Library and Super-Server/Client.

## Components

1.  **run_audit.py**: The main entry point. It runs:
    *   **Unit Tests:** via CTest (GoogleTest).
    *   **Integration Tests:** via `super_client` scenarios against `super_server`.
    *   **Load Tests:** (Planned) using concurrent clients.

2.  **Audit Report**: Generates `audit_report.json` and `audit_report.md` with pass/fail status, duration, and error logs.

## Usage

```bash
# Build the project first
mkdir build-Debug && cd build-Debug && cmake .. && cmake --build .

# Run the audit
python3 scripts/audit/run_audit.py
```

## Scenarios
Integration scenarios are defined in `apps/super_client/scenarios.yaml`.
They cover:
*   Raw/Proto TCP
*   Raw/Proto UDP
*   gRPC, QUIC (Experimental)
*   Secure Variants (TLS)
