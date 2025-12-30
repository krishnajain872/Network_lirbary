# Audit Report

## Unit Tests
- Status: PASS
- Duration: 0.06s
## Integration Tests
- Status: FAIL
- Duration: 30.03s
### Error Output
```

[2025-12-30 10:22:39.555934] [INFO] [47344:140646973630400] [140646973630400] [scenario_engine.cc:57:Execute] Executing Scenario: Raw TCP Echo
[2025-12-30 10:22:39.556448] [INFO] [47344:140646973630400] [140646973630400] [client.cc:33:Client] Client initialized for raw mode
[2025-12-30 10:22:39.556493] [INFO] [47344:140646973630400] [140646973630400] [scenario_engine.cc:138:operator()] Action: Connect to 127.0.0.1:9001
[2025-12-30 10:22:39.556700] [INFO] [47344:140646889293504] [140646889293504] [event_loop.cc:112:Run] Event loop starting
[2025-12-30 10:22:39.559934] [ERROR] [47344:140646889293504] [140646889293504] [connection.cc:341:HandleError] Connection error on fd 5
[2025-12-30 10:22:44.559309] [ERROR] [47344:140646973630400] [140646973630400] [scenario_engine.cc:158:operator()] Timeout waiting for data
[2025-12-30 10:22:44.559363] [INFO] [47344:140646973630400] [140646973630400] [scenario_engine.cc:211:Execute] Performance: 1 messages in 5.00s (0.20 msg/sec)
[2025-12-30 10:22:44.559397] [INFO] [47344:140646973630400] [140646973630400] [event_loop.cc:151:Stop] Stopping event loop
[2025-12-30 10:22:44.559528] [INFO] [47344:140646889293504] [140646889293504] [event_loop.cc:147:Run] Event loop stopped
[2025-12-30 10:22:44.559813] [INFO] [47344:140646973630400] [140646973630400] [event_loop.cc:151:Stop] Stopping event loop
[2025-12-30 10:22:44.559909] [ERROR] [47344:140646973630400] [140646973630400] [main.cc:19:main] Scenario 'Raw TCP Echo' FAILED
[2025-12-30 10:22:44.559916] [INFO] [47344:140646973630400] [140646973630400] [scenario_engine.cc:57:Execute] Executing Scenario: Proto TCP Echo
[2025-12-30 10:22:44.559994] [INFO] [47344:140646973630400] [140646973630400] [client.cc:33:Client] Client initialized for tcp mode
[2025-12-30 10:22:44.560028] [INFO] [47344:140646973630400] [140646973630400] [scenario_engine.cc:138:operator()] Action: Connect to 127.0.0.1:9002
[2025-12-30 10:22:44.560173] [INFO] [47344:140646889293504] [140646889293504] [event_loop.cc:112:Run] Event loop starting
[2025-12-30 10:22:44.562274] [ERROR] [47344:140646889293504] [140646889293504] [connection.cc:341:HandleError] Connection error on fd 5
[2025-12-30 10:22:49.562034] [ERROR] [47344:140646973630400] [140646973630400] [scenario_engine.cc:158:operator()] Timeout waiting for data
[2025-12-30 10:22:49.562084] [INFO] [47344:140646973630400] [140646973630400] [scenario_engine.cc:211:Execute] Performance: 1 messages in 5.00s (0.20 msg/sec)
[2025-12-30 10:22:49.562109] [INFO] [47344:140646973630400] [140646973630400] [event_loop.cc:151:Stop] Stopping event loop
[2025-12-30 10:22:49.562273] [INFO] [47344:140646889293504] [140646889293504] [event_loop.cc:147:Run] Event loop stopped
[2025-12-30 10:22:49.562470] [INFO] [47344:140646973630400] [140646973630400] [event_loop.cc:151:Stop] Stopping event loop
[2025-12-30 10:22:49.562532] [ERROR] [47344:140646973630400] [140646973630400] [main.cc:19:main] Scenario 'Proto TCP Echo' FAILED
[2025-12-30 10:22:49.562542] [INFO] [47344:140646973630400] [140646973630400] [scenario_engine.cc:57:Execute] Executing Scenario: Raw UDP Packet
[2025-12-30 10:22:49.562620] [INFO] [47344:140646973630400] [140646973630400] [client.cc:33:Client] Client initialized for udp mode
[2025-12-30 10:22:49.562651] [INFO] [47344:140646973630400] [140646973630400] [scenario_engine.cc:138:operator()] Action: Connect to 127.0.0.1:9003
[2025-12-30 10:22:49.562839] [INFO] [47344:140646889293504] [140646889293504] [event_loop.cc:112:Run] Event loop starting
[2025-12-30 10:22:49.562952] [INFO] [47344:140646973630400] [140646973630400] [connection.cc:105:Connect] Connected to 127.0.0.1:9003
[2025-12-30 10:22:54.563259] [ERROR] [47344:140646973630400] [140646973630400] [scenario_engine.cc:158:operator()] Timeout waiting for data
[2025-12-30 10:22:54.563312] [INFO] [47344:140646973630400] [140646973630400] [scenario_engine.cc:211:Execute] Performance: 1 messages in 5.00s (0.20 msg/sec)
[2025-12-30 10:22:54.563415] [INFO] [47344:140646973630400] [140646973630400] [event_loop.cc:151:Stop] Stopping event loop
[2025-12-30 10:22:54.563582] [INFO] [47344:140646889293504] [140646889293504] [event_loop.cc:147:Run] Event loop stopped
[2025-12-30 10:22:54.563749] [INFO] [47344:140646973630400] [140646973630400] [event_loop.cc:151:Stop] Stopping event loop
[2025-12-30 10:22:54.563807] [ERROR] [47344:140646973630400] [140646973630400] [main.cc:19:main] Scenario 'Raw UDP Packet' FAILED
[2025-12-30 10:22:54.563813] [INFO] [47344:140646973630400] [140646973630400] [scenario_engine.cc:57:Execute] Executing Scenario: gRPC Call
[2025-12-30 10:22:54.563886] [INFO] [47344:140646973630400] [140646973630400] [client.cc:33:Client] Client initialized for grpc mode
[2025-12-30 10:22:54.563912] [INFO] [47344:140646973630400] [140646973630400] [scenario_engine.cc:138:operator()] Action: Connect to 127.0.0.1:9007
[2025-12-30 10:22:54.564054] [INFO] [47344:140646889293504] [140646889293504] [event_loop.cc:112:Run] Event loop starting
[2025-12-30 10:22:54.564067] [INFO] [47344:140646973630400] [140646973630400] [grpc_protocol.h:29:OnConnect] Sent HTTP/2 Preface and SETTINGS
[2025-12-30 10:22:59.566031] [ERROR] [47344:140646973630400] [140646973630400] [scenario_engine.cc:158:operator()] Timeout waiting for data
[2025-12-30 10:22:59.566080] [INFO] [47344:140646973630400] [140646973630400] [scenario_engine.cc:211:Execute] Performance: 1 messages in 5.00s (0.20 msg/sec)
[2025-12-30 10:22:59.566102] [INFO] [47344:140646973630400] [140646973630400] [event_loop.cc:151:Stop] Stopping event loop
[2025-12-30 10:22:59.566261] [INFO] [47344:140646889293504] [140646889293504] [event_loop.cc:147:Run] Event loop stopped
[2025-12-30 10:22:59.566475] [INFO] [47344:140646973630400] [140646973630400] [event_loop.cc:151:Stop] Stopping event loop
[2025-12-30 10:22:59.566541] [ERROR] [47344:140646973630400] [140646973630400] [main.cc:19:main] Scenario 'gRPC Call' FAILED
[2025-12-30 10:22:59.566593] [INFO] [47344:140646973630400] [140646973630400] [scenario_engine.cc:57:Execute] Executing Scenario: RPC Call
[2025-12-30 10:22:59.566682] [INFO] [47344:140646973630400] [140646973630400] [client.cc:33:Client] Client initialized for rpc mode
[2025-12-30 10:22:59.566713] [INFO] [47344:140646973630400] [140646973630400] [scenario_engine.cc:138:operator()] Action: Connect to 127.0.0.1:9008
[2025-12-30 10:22:59.566858] [INFO] [47344:140646889293504] [140646889293504] [event_loop.cc:112:Run] Event loop starting
[2025-12-30 10:22:59.568596] [ERROR] [47344:140646889293504] [140646889293504] [connection.cc:341:HandleError] Connection error on fd 5
[2025-12-30 10:23:04.568557] [ERROR] [47344:140646973630400] [140646973630400] [scenario_engine.cc:158:operator()] Timeout waiting for data
[2025-12-30 10:23:04.568612] [INFO] [47344:140646973630400] [140646973630400] [scenario_engine.cc:211:Execute] Performance: 1 messages in 5.00s (0.20 msg/sec)
[2025-12-30 10:23:04.568633] [INFO] [47344:140646973630400] [140646973630400] [event_loop.cc:151:Stop] Stopping event loop
[2025-12-30 10:23:04.568794] [INFO] [47344:140646889293504] [140646889293504] [event_loop.cc:147:Run] Event loop stopped
[2025-12-30 10:23:04.569077] [INFO] [47344:140646973630400] [140646973630400] [event_loop.cc:151:Stop] Stopping event loop
[2025-12-30 10:23:04.569151] [ERROR] [47344:140646973630400] [140646973630400] [main.cc:19:main] Scenario 'RPC Call' FAILED
[2025-12-30 10:23:04.569158] [INFO] [47344:140646973630400] [140646973630400] [scenario_engine.cc:57:Execute] Executing Scenario: QUIC Stream
[2025-12-30 10:23:04.569235] [INFO] [47344:140646973630400] [140646973630400] [client.cc:33:Client] Client initialized for quic mode
[2025-12-30 10:23:04.569246] [INFO] [47344:140646973630400] [140646973630400] [scenario_engine.cc:138:operator()] Action: Connect to 127.0.0.1:9010
[2025-12-30 10:23:04.569407] [INFO] [47344:140646889293504] [140646889293504] [event_loop.cc:112:Run] Event loop starting
[2025-12-30 10:23:04.569486] [INFO] [47344:140646973630400] [140646973630400] [connection.cc:105:Connect] Connected to 127.0.0.1:9010
[2025-12-30 10:23:04.570735] [ERROR] [47344:140646889293504] [140646889293504] [connection.cc:341:HandleError] Connection error on fd 5
[2025-12-30 10:23:09.569817] [ERROR] [47344:140646973630400] [140646973630400] [scenario_engine.cc:158:operator()] Timeout waiting for data
[2025-12-30 10:23:09.569875] [INFO] [47344:140646973630400] [140646973630400] [scenario_engine.cc:211:Execute] Performance: 1 messages in 5.00s (0.20 msg/sec)
[2025-12-30 10:23:09.569895] [INFO] [47344:140646973630400] [140646973630400] [event_loop.cc:151:Stop] Stopping event loop
[2025-12-30 10:23:09.570114] [INFO] [47344:140646889293504] [140646889293504] [event_loop.cc:147:Run] Event loop stopped
[2025-12-30 10:23:09.570262] [INFO] [47344:140646973630400] [140646973630400] [event_loop.cc:151:Stop] Stopping event loop
[2025-12-30 10:23:09.570336] [ERROR] [47344:140646973630400] [140646973630400] [main.cc:19:main] Scenario 'QUIC Stream' FAILED
[2025-12-30 10:23:09.570376] [INFO] [47344:140646973630400] [140646973630400] [scenario_engine.cc:57:Execute] Executing Scenario: Secure TCP Echo
[2025-12-30 10:23:09.570559] [INFO] [47344:140646973630400] [140646973630400] [client.cc:33:Client] Client initialized for tcp mode
[2025-12-30 10:23:09.570594] [INFO] [47344:140646973630400] [140646973630400] [scenario_engine.cc:138:operator()] Action: Connect to 127.0.0.1:9011
[2025-12-30 10:23:09.570739] [INFO] [47344:140646889293504] [140646889293504] [event_loop.cc:112:Run] Event loop starting

```
