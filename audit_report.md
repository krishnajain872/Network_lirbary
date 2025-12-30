# Audit Report

## Unit Tests
- Status: PASS
- Duration: 0.06s
## Integration Tests
- Status: FAIL
- Duration: 5.04s
### Error Output
```
./build-Debug/bin/super_client: symbol lookup error: /app/build-Debug/lib/libnetwork.so: undefined symbol: _ZN10networklib9protocols5http212HpackEncoder6EncodeERKSt3mapINSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEES9_St4lessIS9_ESaISt4pairIKS9_S9_EEE

[2025-12-30 09:02:54.346789] [INFO] [43165:139979057874880] [139979057874880] [scenario_engine.cc:57:Execute] Executing Scenario: Raw TCP Echo
[2025-12-30 09:02:54.347546] [INFO] [43165:139979057874880] [139979057874880] [client.cc:33:Client] Client initialized for raw mode
[2025-12-30 09:02:54.347595] [INFO] [43165:139979057874880] [139979057874880] [scenario_engine.cc:138:operator()] Action: Connect to 127.0.0.1:9001
[2025-12-30 09:02:54.347866] [INFO] [43165:139979047044800] [139979047044800] [event_loop.cc:112:Run] Event loop starting
[2025-12-30 09:02:54.362476] [INFO] [43165:139979057874880] [139979057874880] [scenario_engine.cc:168:operator()] Action: Disconnect
[2025-12-30 09:02:54.362594] [INFO] [43165:139979057874880] [139979057874880] [event_loop.cc:151:Stop] Stopping event loop
[2025-12-30 09:02:54.363305] [INFO] [43165:139979047044800] [139979047044800] [event_loop.cc:147:Run] Event loop stopped
[2025-12-30 09:02:54.363455] [INFO] [43165:139979057874880] [139979057874880] [scenario_engine.cc:211:Execute] Performance: 1 messages in 0.01s (66.67 msg/sec)
[2025-12-30 09:02:54.363577] [INFO] [43165:139979057874880] [139979057874880] [event_loop.cc:151:Stop] Stopping event loop
[2025-12-30 09:02:54.363641] [INFO] [43165:139979057874880] [139979057874880] [event_loop.cc:151:Stop] Stopping event loop
[2025-12-30 09:02:54.363701] [INFO] [43165:139979057874880] [139979057874880] [main.cc:22:main] Scenario 'Raw TCP Echo' PASSED
[2025-12-30 09:02:54.363708] [INFO] [43165:139979057874880] [139979057874880] [scenario_engine.cc:57:Execute] Executing Scenario: Proto TCP Echo
[2025-12-30 09:02:54.363964] [INFO] [43165:139979057874880] [139979057874880] [client.cc:33:Client] Client initialized for tcp mode
[2025-12-30 09:02:54.364002] [INFO] [43165:139979057874880] [139979057874880] [scenario_engine.cc:138:operator()] Action: Connect to 127.0.0.1:9002
[2025-12-30 09:02:54.364157] [INFO] [43165:139979047044800] [139979047044800] [event_loop.cc:112:Run] Event loop starting
[2025-12-30 09:02:54.366851] [INFO] [43165:139979057874880] [139979057874880] [scenario_engine.cc:168:operator()] Action: Disconnect
[2025-12-30 09:02:54.366935] [INFO] [43165:139979057874880] [139979057874880] [event_loop.cc:151:Stop] Stopping event loop
[2025-12-30 09:02:54.367033] [INFO] [43165:139979047044800] [139979047044800] [event_loop.cc:147:Run] Event loop stopped
[2025-12-30 09:02:54.367145] [INFO] [43165:139979057874880] [139979057874880] [scenario_engine.cc:211:Execute] Performance: 1 messages in 0.00s (333.33 msg/sec)
[2025-12-30 09:02:54.367180] [INFO] [43165:139979057874880] [139979057874880] [event_loop.cc:151:Stop] Stopping event loop
[2025-12-30 09:02:54.367200] [INFO] [43165:139979057874880] [139979057874880] [event_loop.cc:151:Stop] Stopping event loop
[2025-12-30 09:02:54.367220] [INFO] [43165:139979057874880] [139979057874880] [main.cc:22:main] Scenario 'Proto TCP Echo' PASSED
[2025-12-30 09:02:54.367225] [INFO] [43165:139979057874880] [139979057874880] [scenario_engine.cc:57:Execute] Executing Scenario: Raw UDP Packet
[2025-12-30 09:02:54.367269] [INFO] [43165:139979057874880] [139979057874880] [client.cc:33:Client] Client initialized for udp mode
[2025-12-30 09:02:54.367298] [INFO] [43165:139979057874880] [139979057874880] [scenario_engine.cc:138:operator()] Action: Connect to 127.0.0.1:9003
[2025-12-30 09:02:54.367397] [INFO] [43165:139979047044800] [139979047044800] [event_loop.cc:112:Run] Event loop starting
[2025-12-30 09:02:54.367505] [INFO] [43165:139979057874880] [139979057874880] [connection.cc:105:Connect] Connected to 127.0.0.1:9003
[2025-12-30 09:02:59.367710] [ERROR] [43165:139979057874880] [139979057874880] [scenario_engine.cc:158:operator()] Timeout waiting for data
[2025-12-30 09:02:59.367758] [INFO] [43165:139979057874880] [139979057874880] [scenario_engine.cc:211:Execute] Performance: 1 messages in 5.00s (0.20 msg/sec)
[2025-12-30 09:02:59.367826] [INFO] [43165:139979057874880] [139979057874880] [event_loop.cc:151:Stop] Stopping event loop
[2025-12-30 09:02:59.368011] [INFO] [43165:139979047044800] [139979047044800] [event_loop.cc:147:Run] Event loop stopped
[2025-12-30 09:02:59.368232] [INFO] [43165:139979057874880] [139979057874880] [event_loop.cc:151:Stop] Stopping event loop
[2025-12-30 09:02:59.368287] [ERROR] [43165:139979057874880] [139979057874880] [main.cc:19:main] Scenario 'Raw UDP Packet' FAILED
[2025-12-30 09:02:59.368295] [INFO] [43165:139979057874880] [139979057874880] [scenario_engine.cc:57:Execute] Executing Scenario: gRPC Call
[2025-12-30 09:02:59.368385] [INFO] [43165:139979057874880] [139979057874880] [client.cc:33:Client] Client initialized for grpc mode
[2025-12-30 09:02:59.368410] [INFO] [43165:139979057874880] [139979057874880] [scenario_engine.cc:138:operator()] Action: Connect to 127.0.0.1:9007
[2025-12-30 09:02:59.368506] [INFO] [43165:139979047044800] [139979047044800] [event_loop.cc:112:Run] Event loop starting
[2025-12-30 09:02:59.368533] [INFO] [43165:139979057874880] [139979057874880] [grpc_protocol.h:29:OnConnect] Sent HTTP/2 Preface and SETTINGS

```
