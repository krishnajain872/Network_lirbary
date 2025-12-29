# Super-Server

A unified server application demonstrating multi-protocol support using `lib_network`.

## Usage
```bash
./bin/super_server [config_path]
```
Defaults to `apps/super_server/server_config.yaml`.

## Configuration
The server is configured via YAML. Example:

```yaml
server:
  protocols:
    - name: "RawTCP"
      type: "tcp"
      mode: "raw"
      port: 9001
    - name: "ProtoTCP"
      type: "tcp"
      mode: "proto"
      port: 9002
```
