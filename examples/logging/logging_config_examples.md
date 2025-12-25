# Logging Configuration Guide

The Network Library provides a flexible, string-based configuration system for logging. Configuration is passed as a semicolon-separated string of key-value pairs.

## Configuration Keys

| Key | Description | Default | Values |
|-----|-------------|---------|--------|
| `appname` | Application name (Required) | - | String |
| `console` | Enable console logging | `true` | `true`, `false` |
| `logfile` | Path to log file | (Disabled) | File path |
| `syslog` | Enable syslog | `false` | `true`, `false` |
| `kafka` | Enable Kafka logging | `false` | `true`, `false` |
| `severity` | Minimum log level | `INFO` | `TRACE`, `DEBUG`, `INFO`, `WARN`, `ERROR`, `FATAL`, `CRITICAL` |
| `async` | Enable async logging | `true` | `true`, `false` |

## Examples

### 1. Basic Console Logging (Debug)
```
appname=MyApp;console=true;severity=DEBUG
```

### 2. File Logging with Rotation
```
appname=FileApp;logfile=/var/log/app.log;rotation_size=10MB;max_backups=5
```

### 3. Syslog Integration
```
appname=DaemonApp;syslog=true;syslog_facility=LOCAL0;console=false
```

### 4. High Performance Async Logging
```
appname=HFTApp;async=true;async_queue_size=1000000;flush_interval=500ms;console=false;logfile=fast.log
```

### 5. Kafka Logging (Stubbed)
```
appname=CloudApp;kafka=true;kafka_broker=localhost:9092;kafka_topic=logs
```
