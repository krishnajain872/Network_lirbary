# Super-Client

A scenario-driven client for testing `lib_network` servers.

## Usage
```bash
./bin/super_client [scenarios_path]
```
Defaults to `apps/super_client/scenarios.yaml`.

## Scenarios
Scenarios are defined in YAML. Example:

```yaml
scenarios:
  - name: "Test Echo"
    type: "tcp"
    mode: "raw"
    target: "localhost:9001"
    steps:
      - action: connect
      - action: send
        data: "Ping"
      - action: expect
        data: "Ping"
      - action: disconnect
```
