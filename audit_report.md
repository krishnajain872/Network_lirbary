# Audit Report

## Unit Tests
- Status: PASS
- Duration: 0.05s
## Integration Tests
- Status: FAIL
- Duration: 25.04s
### Error Output
```

[2025-12-30 08:10:36.711281] [INFO] [38851:140305568155584] [140305568155584] [scenario_engine.cc:57:Execute] Executing Scenario: Raw TCP Echo
[2025-12-30 08:10:36.712072] [INFO] [38851:140305568155584] [140305568155584] [client.cc:33:Client] Client initialized for raw mode
[2025-12-30 08:10:36.712123] [INFO] [38851:140305568155584] [140305568155584] [scenario_engine.cc:138:operator()] Action: Connect to 127.0.0.1:9001
[2025-12-30 08:10:36.712458] [INFO] [38851:140305556833984] [140305556833984] [event_loop.cc:112:Run] Event loop starting
[2025-12-30 08:10:36.727593] [INFO] [38851:140305568155584] [140305568155584] [scenario_engine.cc:168:operator()] Action: Disconnect
[2025-12-30 08:10:36.727721] [INFO] [38851:140305568155584] [140305568155584] [event_loop.cc:151:Stop] Stopping event loop
[2025-12-30 08:10:36.727955] [INFO] [38851:140305556833984] [140305556833984] [event_loop.cc:147:Run] Event loop stopped
[2025-12-30 08:10:36.728542] [INFO] [38851:140305568155584] [140305568155584] [scenario_engine.cc:211:Execute] Performance: 1 messages in 0.02s (62.50 msg/sec)
[2025-12-30 08:10:36.728676] [INFO] [38851:140305568155584] [140305568155584] [event_loop.cc:151:Stop] Stopping event loop
[2025-12-30 08:10:36.728749] [INFO] [38851:140305568155584] [140305568155584] [event_loop.cc:151:Stop] Stopping event loop
[2025-12-30 08:10:36.728822] [INFO] [38851:140305568155584] [140305568155584] [main.cc:22:main] Scenario 'Raw TCP Echo' PASSED
[2025-12-30 08:10:36.728829] [INFO] [38851:140305568155584] [140305568155584] [scenario_engine.cc:57:Execute] Executing Scenario: Proto TCP Echo
[2025-12-30 08:10:36.728916] [INFO] [38851:140305568155584] [140305568155584] [client.cc:33:Client] Client initialized for tcp mode
[2025-12-30 08:10:36.728949] [INFO] [38851:140305568155584] [140305568155584] [scenario_engine.cc:138:operator()] Action: Connect to 127.0.0.1:9002
[2025-12-30 08:10:36.729086] [INFO] [38851:140305556833984] [140305556833984] [event_loop.cc:112:Run] Event loop starting
[2025-12-30 08:10:36.731854] [ERROR] [38851:140305556833984] [140305556833984] [connection.cc:341:HandleError] Connection error on fd 5
[2025-12-30 08:10:41.731292] [ERROR] [38851:140305568155584] [140305568155584] [scenario_engine.cc:158:operator()] Timeout waiting for data
[2025-12-30 08:10:41.731339] [INFO] [38851:140305568155584] [140305568155584] [scenario_engine.cc:211:Execute] Performance: 1 messages in 5.00s (0.20 msg/sec)
[2025-12-30 08:10:41.731368] [INFO] [38851:140305568155584] [140305568155584] [event_loop.cc:151:Stop] Stopping event loop
[2025-12-30 08:10:41.731520] [INFO] [38851:140305556833984] [140305556833984] [event_loop.cc:147:Run] Event loop stopped
[2025-12-30 08:10:41.731662] [INFO] [38851:140305568155584] [140305568155584] [event_loop.cc:151:Stop] Stopping event loop
[2025-12-30 08:10:41.731722] [ERROR] [38851:140305568155584] [140305568155584] [main.cc:19:main] Scenario 'Proto TCP Echo' FAILED
[2025-12-30 08:10:41.731730] [INFO] [38851:140305568155584] [140305568155584] [scenario_engine.cc:57:Execute] Executing Scenario: Raw UDP Packet
[2025-12-30 08:10:41.731840] [INFO] [38851:140305568155584] [140305568155584] [client.cc:33:Client] Client initialized for udp mode
[2025-12-30 08:10:41.731870] [INFO] [38851:140305568155584] [140305568155584] [scenario_engine.cc:138:operator()] Action: Connect to 127.0.0.1:9003
[2025-12-30 08:10:41.731980] [INFO] [38851:140305556833984] [140305556833984] [event_loop.cc:112:Run] Event loop starting
[2025-12-30 08:10:41.732137] [INFO] [38851:140305568155584] [140305568155584] [connection.cc:105:Connect] Connected to 127.0.0.1:9003
[2025-12-30 08:10:46.732379] [ERROR] [38851:140305568155584] [140305568155584] [scenario_engine.cc:158:operator()] Timeout waiting for data
[2025-12-30 08:10:46.732431] [INFO] [38851:140305568155584] [140305568155584] [scenario_engine.cc:211:Execute] Performance: 1 messages in 5.00s (0.20 msg/sec)
[2025-12-30 08:10:46.732500] [INFO] [38851:140305568155584] [140305568155584] [event_loop.cc:151:Stop] Stopping event loop
[2025-12-30 08:10:46.732677] [INFO] [38851:140305556833984] [140305556833984] [event_loop.cc:147:Run] Event loop stopped
[2025-12-30 08:10:46.732885] [INFO] [38851:140305568155584] [140305568155584] [event_loop.cc:151:Stop] Stopping event loop
[2025-12-30 08:10:46.732942] [ERROR] [38851:140305568155584] [140305568155584] [main.cc:19:main] Scenario 'Raw UDP Packet' FAILED
[2025-12-30 08:10:46.732949] [INFO] [38851:140305568155584] [140305568155584] [scenario_engine.cc:57:Execute] Executing Scenario: gRPC Call
[2025-12-30 08:10:46.733041] [INFO] [38851:140305568155584] [140305568155584] [client.cc:33:Client] Client initialized for grpc mode
[2025-12-30 08:10:46.733051] [INFO] [38851:140305568155584] [140305568155584] [scenario_engine.cc:138:operator()] Action: Connect to 127.0.0.1:9007
[2025-12-30 08:10:46.733185] [INFO] [38851:140305556833984] [140305556833984] [event_loop.cc:112:Run] Event loop starting
[2025-12-30 08:10:46.733198] [INFO] [38851:140305568155584] [140305568155584] [grpc_protocol.h:28:OnConnect] Sent HTTP/2 Preface and SETTINGS
[2025-12-30 08:10:46.734815] [ERROR] [38851:140305556833984] [140305556833984] [connection.cc:341:HandleError] Connection error on fd 5
[2025-12-30 08:10:51.734813] [ERROR] [38851:140305568155584] [140305568155584] [scenario_engine.cc:158:operator()] Timeout waiting for data
[2025-12-30 08:10:51.734865] [INFO] [38851:140305568155584] [140305568155584] [scenario_engine.cc:211:Execute] Performance: 1 messages in 5.00s (0.20 msg/sec)
[2025-12-30 08:10:51.734900] [INFO] [38851:140305568155584] [140305568155584] [event_loop.cc:151:Stop] Stopping event loop
[2025-12-30 08:10:51.735063] [INFO] [38851:140305556833984] [140305556833984] [event_loop.cc:147:Run] Event loop stopped
[2025-12-30 08:10:51.735279] [INFO] [38851:140305568155584] [140305568155584] [event_loop.cc:151:Stop] Stopping event loop
[2025-12-30 08:10:51.735354] [ERROR] [38851:140305568155584] [140305568155584] [main.cc:19:main] Scenario 'gRPC Call' FAILED
[2025-12-30 08:10:51.735361] [INFO] [38851:140305568155584] [140305568155584] [scenario_engine.cc:57:Execute] Executing Scenario: RPC Call
[2025-12-30 08:10:51.735487] [INFO] [38851:140305568155584] [140305568155584] [client.cc:33:Client] Client initialized for rpc mode
[2025-12-30 08:10:51.735497] [INFO] [38851:140305568155584] [140305568155584] [scenario_engine.cc:138:operator()] Action: Connect to 127.0.0.1:9008
[2025-12-30 08:10:51.735634] [INFO] [38851:140305556833984] [140305556833984] [event_loop.cc:112:Run] Event loop starting
[2025-12-30 08:10:51.737601] [ERROR] [38851:140305556833984] [140305556833984] [connection.cc:341:HandleError] Connection error on fd 5
[2025-12-30 08:10:56.737446] [ERROR] [38851:140305568155584] [140305568155584] [scenario_engine.cc:158:operator()] Timeout waiting for data
[2025-12-30 08:10:56.737492] [INFO] [38851:140305568155584] [140305568155584] [scenario_engine.cc:211:Execute] Performance: 1 messages in 5.00s (0.20 msg/sec)
[2025-12-30 08:10:56.737516] [INFO] [38851:140305568155584] [140305568155584] [event_loop.cc:151:Stop] Stopping event loop
[2025-12-30 08:10:56.737642] [INFO] [38851:140305556833984] [140305556833984] [event_loop.cc:147:Run] Event loop stopped
[2025-12-30 08:10:56.737898] [INFO] [38851:140305568155584] [140305568155584] [event_loop.cc:151:Stop] Stopping event loop
[2025-12-30 08:10:56.737972] [ERROR] [38851:140305568155584] [140305568155584] [main.cc:19:main] Scenario 'RPC Call' FAILED
[2025-12-30 08:10:56.737980] [INFO] [38851:140305568155584] [140305568155584] [scenario_engine.cc:57:Execute] Executing Scenario: QUIC Stream
[2025-12-30 08:10:56.738054] [INFO] [38851:140305568155584] [140305568155584] [client.cc:33:Client] Client initialized for quic mode
[2025-12-30 08:10:56.738082] [INFO] [38851:140305568155584] [140305568155584] [scenario_engine.cc:138:operator()] Action: Connect to 127.0.0.1:9010
[2025-12-30 08:10:56.738213] [INFO] [38851:140305556833984] [140305556833984] [event_loop.cc:112:Run] Event loop starting
[2025-12-30 08:10:56.738299] [INFO] [38851:140305568155584] [140305568155584] [connection.cc:105:Connect] Connected to 127.0.0.1:9010
[2025-12-30 08:10:56.739716] [ERROR] [38851:140305556833984] [140305556833984] [connection.cc:341:HandleError] Connection error on fd 5
[2025-12-30 08:11:01.738748] [ERROR] [38851:140305568155584] [140305568155584] [scenario_engine.cc:158:operator()] Timeout waiting for data
[2025-12-30 08:11:01.738798] [INFO] [38851:140305568155584] [140305568155584] [scenario_engine.cc:211:Execute] Performance: 1 messages in 5.00s (0.20 msg/sec)
[2025-12-30 08:11:01.738822] [INFO] [38851:140305568155584] [140305568155584] [event_loop.cc:151:Stop] Stopping event loop
[2025-12-30 08:11:01.738975] [INFO] [38851:140305556833984] [140305556833984] [event_loop.cc:147:Run] Event loop stopped
[2025-12-30 08:11:01.739162] [INFO] [38851:140305568155584] [140305568155584] [event_loop.cc:151:Stop] Stopping event loop
[2025-12-30 08:11:01.739238] [ERROR] [38851:140305568155584] [140305568155584] [main.cc:19:main] Scenario 'QUIC Stream' FAILED
[2025-12-30 08:11:01.739247] [INFO] [38851:140305568155584] [140305568155584] [scenario_engine.cc:57:Execute] Executing Scenario: Secure TCP Echo
[2025-12-30 08:11:01.739366] [INFO] [38851:140305568155584] [140305568155584] [client.cc:33:Client] Client initialized for tcp mode
[2025-12-30 08:11:01.739399] [INFO] [38851:140305568155584] [140305568155584] [scenario_engine.cc:138:operator()] Action: Connect to 127.0.0.1:9011
[2025-12-30 08:11:01.739547] [INFO] [38851:140305556833984] [140305556833984] [event_loop.cc:112:Run] Event loop starting

```
