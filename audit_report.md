# Audit Report

## Unit Tests
- Status: PASS
- Duration: 0.02s
## Integration Tests
- Status: FAIL
- Duration: 20.05s
### Error Output
```

[2025-12-30 06:25:13.114475] [INFO] [36358:140027905705920] [140027905705920] [scenario_engine.cc:57:Execute] Executing Scenario: Raw TCP Echo
[2025-12-30 06:25:13.115054] [INFO] [36358:140027905705920] [140027905705920] [client.cc:33:Client] Client initialized for raw mode
[2025-12-30 06:25:13.115102] [INFO] [36358:140027905705920] [140027905705920] [scenario_engine.cc:138:operator()] Action: Connect to 127.0.0.1:9001
[2025-12-30 06:25:13.115296] [INFO] [36358:140027893909184] [140027893909184] [event_loop.cc:112:Run] Event loop starting
[2025-12-30 06:25:13.131268] [INFO] [36358:140027905705920] [140027905705920] [scenario_engine.cc:168:operator()] Action: Disconnect
[2025-12-30 06:25:13.131408] [INFO] [36358:140027905705920] [140027905705920] [event_loop.cc:151:Stop] Stopping event loop
[2025-12-30 06:25:13.132294] [INFO] [36358:140027893909184] [140027893909184] [event_loop.cc:147:Run] Event loop stopped
[2025-12-30 06:25:13.132519] [INFO] [36358:140027905705920] [140027905705920] [scenario_engine.cc:211:Execute] Performance: 1 messages in 0.02s (58.82 msg/sec)
[2025-12-30 06:25:13.132543] [INFO] [36358:140027905705920] [140027905705920] [event_loop.cc:151:Stop] Stopping event loop
[2025-12-30 06:25:13.132612] [INFO] [36358:140027905705920] [140027905705920] [event_loop.cc:151:Stop] Stopping event loop
[2025-12-30 06:25:13.132683] [INFO] [36358:140027905705920] [140027905705920] [main.cc:22:main] Scenario 'Raw TCP Echo' PASSED
[2025-12-30 06:25:13.132690] [INFO] [36358:140027905705920] [140027905705920] [scenario_engine.cc:57:Execute] Executing Scenario: Proto TCP Echo
[2025-12-30 06:25:13.132746] [INFO] [36358:140027905705920] [140027905705920] [client.cc:33:Client] Client initialized for tcp mode
[2025-12-30 06:25:13.132782] [INFO] [36358:140027905705920] [140027905705920] [scenario_engine.cc:138:operator()] Action: Connect to 127.0.0.1:9002
[2025-12-30 06:25:13.132968] [INFO] [36358:140027893909184] [140027893909184] [event_loop.cc:112:Run] Event loop starting
[2025-12-30 06:25:13.135959] [INFO] [36358:140027905705920] [140027905705920] [scenario_engine.cc:168:operator()] Action: Disconnect
[2025-12-30 06:25:13.136051] [INFO] [36358:140027905705920] [140027905705920] [event_loop.cc:151:Stop] Stopping event loop
[2025-12-30 06:25:13.136180] [INFO] [36358:140027893909184] [140027893909184] [event_loop.cc:147:Run] Event loop stopped
[2025-12-30 06:25:13.136318] [INFO] [36358:140027905705920] [140027905705920] [scenario_engine.cc:211:Execute] Performance: 1 messages in 0.00s (333.33 msg/sec)
[2025-12-30 06:25:13.136354] [INFO] [36358:140027905705920] [140027905705920] [event_loop.cc:151:Stop] Stopping event loop
[2025-12-30 06:25:13.136377] [INFO] [36358:140027905705920] [140027905705920] [event_loop.cc:151:Stop] Stopping event loop
[2025-12-30 06:25:13.136398] [INFO] [36358:140027905705920] [140027905705920] [main.cc:22:main] Scenario 'Proto TCP Echo' PASSED
[2025-12-30 06:25:13.136403] [INFO] [36358:140027905705920] [140027905705920] [scenario_engine.cc:57:Execute] Executing Scenario: Raw UDP Packet
[2025-12-30 06:25:13.136457] [INFO] [36358:140027905705920] [140027905705920] [client.cc:33:Client] Client initialized for udp mode
[2025-12-30 06:25:13.136513] [INFO] [36358:140027905705920] [140027905705920] [scenario_engine.cc:138:operator()] Action: Connect to 127.0.0.1:9003
[2025-12-30 06:25:13.136612] [INFO] [36358:140027893909184] [140027893909184] [event_loop.cc:112:Run] Event loop starting
[2025-12-30 06:25:13.136786] [INFO] [36358:140027905705920] [140027905705920] [connection.cc:105:Connect] Connected to 127.0.0.1:9003
[2025-12-30 06:25:18.137089] [ERROR] [36358:140027905705920] [140027905705920] [scenario_engine.cc:158:operator()] Timeout waiting for data
[2025-12-30 06:25:18.137142] [INFO] [36358:140027905705920] [140027905705920] [scenario_engine.cc:211:Execute] Performance: 1 messages in 5.00s (0.20 msg/sec)
[2025-12-30 06:25:18.137209] [INFO] [36358:140027905705920] [140027905705920] [event_loop.cc:151:Stop] Stopping event loop
[2025-12-30 06:25:18.137395] [INFO] [36358:140027893909184] [140027893909184] [event_loop.cc:147:Run] Event loop stopped
[2025-12-30 06:25:18.137602] [INFO] [36358:140027905705920] [140027905705920] [event_loop.cc:151:Stop] Stopping event loop
[2025-12-30 06:25:18.137680] [ERROR] [36358:140027905705920] [140027905705920] [main.cc:19:main] Scenario 'Raw UDP Packet' FAILED
[2025-12-30 06:25:18.137687] [INFO] [36358:140027905705920] [140027905705920] [scenario_engine.cc:57:Execute] Executing Scenario: gRPC Call
[2025-12-30 06:25:18.137764] [INFO] [36358:140027905705920] [140027905705920] [client.cc:33:Client] Client initialized for grpc mode
[2025-12-30 06:25:18.137796] [INFO] [36358:140027905705920] [140027905705920] [scenario_engine.cc:138:operator()] Action: Connect to 127.0.0.1:9007
[2025-12-30 06:25:18.137957] [INFO] [36358:140027893909184] [140027893909184] [event_loop.cc:112:Run] Event loop starting
[2025-12-30 06:25:18.139889] [ERROR] [36358:140027893909184] [140027893909184] [connection.cc:341:HandleError] Connection error on fd 5
[2025-12-30 06:25:23.139797] [ERROR] [36358:140027905705920] [140027905705920] [scenario_engine.cc:158:operator()] Timeout waiting for data
[2025-12-30 06:25:23.139848] [INFO] [36358:140027905705920] [140027905705920] [scenario_engine.cc:211:Execute] Performance: 1 messages in 5.00s (0.20 msg/sec)
[2025-12-30 06:25:23.139874] [INFO] [36358:140027905705920] [140027905705920] [event_loop.cc:151:Stop] Stopping event loop
[2025-12-30 06:25:23.140038] [INFO] [36358:140027893909184] [140027893909184] [event_loop.cc:147:Run] Event loop stopped
[2025-12-30 06:25:23.140242] [INFO] [36358:140027905705920] [140027905705920] [event_loop.cc:151:Stop] Stopping event loop
[2025-12-30 06:25:23.140316] [ERROR] [36358:140027905705920] [140027905705920] [main.cc:19:main] Scenario 'gRPC Call' FAILED
[2025-12-30 06:25:23.140357] [INFO] [36358:140027905705920] [140027905705920] [scenario_engine.cc:57:Execute] Executing Scenario: RPC Call
[2025-12-30 06:25:23.140488] [INFO] [36358:140027905705920] [140027905705920] [client.cc:33:Client] Client initialized for rpc mode
[2025-12-30 06:25:23.140522] [INFO] [36358:140027905705920] [140027905705920] [scenario_engine.cc:138:operator()] Action: Connect to 127.0.0.1:9008
[2025-12-30 06:25:23.141058] [INFO] [36358:140027893909184] [140027893909184] [event_loop.cc:112:Run] Event loop starting
[2025-12-30 06:25:23.143023] [ERROR] [36358:140027893909184] [140027893909184] [connection.cc:341:HandleError] Connection error on fd 5
[2025-12-30 06:25:28.142590] [ERROR] [36358:140027905705920] [140027905705920] [scenario_engine.cc:158:operator()] Timeout waiting for data
[2025-12-30 06:25:28.142645] [INFO] [36358:140027905705920] [140027905705920] [scenario_engine.cc:211:Execute] Performance: 1 messages in 5.00s (0.20 msg/sec)
[2025-12-30 06:25:28.142668] [INFO] [36358:140027905705920] [140027905705920] [event_loop.cc:151:Stop] Stopping event loop
[2025-12-30 06:25:28.143077] [INFO] [36358:140027893909184] [140027893909184] [event_loop.cc:147:Run] Event loop stopped
[2025-12-30 06:25:28.143374] [INFO] [36358:140027905705920] [140027905705920] [event_loop.cc:151:Stop] Stopping event loop
[2025-12-30 06:25:28.143447] [ERROR] [36358:140027905705920] [140027905705920] [main.cc:19:main] Scenario 'RPC Call' FAILED
[2025-12-30 06:25:28.143454] [INFO] [36358:140027905705920] [140027905705920] [scenario_engine.cc:57:Execute] Executing Scenario: QUIC Stream
[2025-12-30 06:25:28.143527] [INFO] [36358:140027905705920] [140027905705920] [client.cc:33:Client] Client initialized for quic mode
[2025-12-30 06:25:28.143558] [INFO] [36358:140027905705920] [140027905705920] [scenario_engine.cc:138:operator()] Action: Connect to 127.0.0.1:9010
[2025-12-30 06:25:28.143745] [INFO] [36358:140027893909184] [140027893909184] [event_loop.cc:112:Run] Event loop starting
[2025-12-30 06:25:28.143834] [INFO] [36358:140027905705920] [140027905705920] [connection.cc:105:Connect] Connected to 127.0.0.1:9010
[2025-12-30 06:25:28.144525] [ERROR] [36358:140027893909184] [140027893909184] [connection.cc:341:HandleError] Connection error on fd 5
[2025-12-30 06:25:33.144293] [ERROR] [36358:140027905705920] [140027905705920] [scenario_engine.cc:158:operator()] Timeout waiting for data
[2025-12-30 06:25:33.144366] [INFO] [36358:140027905705920] [140027905705920] [scenario_engine.cc:211:Execute] Performance: 1 messages in 5.00s (0.20 msg/sec)
[2025-12-30 06:25:33.144408] [INFO] [36358:140027905705920] [140027905705920] [event_loop.cc:151:Stop] Stopping event loop
[2025-12-30 06:25:33.144659] [INFO] [36358:140027893909184] [140027893909184] [event_loop.cc:147:Run] Event loop stopped
[2025-12-30 06:25:33.144829] [INFO] [36358:140027905705920] [140027905705920] [event_loop.cc:151:Stop] Stopping event loop
[2025-12-30 06:25:33.144918] [ERROR] [36358:140027905705920] [140027905705920] [main.cc:19:main] Scenario 'QUIC Stream' FAILED
[2025-12-30 06:25:33.144967] [INFO] [36358:140027905705920] [140027905705920] [scenario_engine.cc:57:Execute] Executing Scenario: Secure TCP Echo
[2025-12-30 06:25:33.145082] [INFO] [36358:140027905705920] [140027905705920] [client.cc:33:Client] Client initialized for tcp mode
[2025-12-30 06:25:33.145091] [INFO] [36358:140027905705920] [140027905705920] [scenario_engine.cc:138:operator()] Action: Connect to 127.0.0.1:9011
[2025-12-30 06:25:33.145294] [INFO] [36358:140027893909184] [140027893909184] [event_loop.cc:112:Run] Event loop starting

```
