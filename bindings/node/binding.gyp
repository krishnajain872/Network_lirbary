{
  "targets": [
    {
      "target_name": "networklib_node",
      "cflags!": [ "-fno-exceptions" ],
      "cflags_cc!": [ "-fno-exceptions" ],
      "sources": [ "networklib_node.cpp" ],
      "include_dirs": [
        "<!@(node -p \"require('node-addon-api').include\")",
        "../../core/include"
      ],
      "defines": [ "NAPI_DISABLE_CPP_EXCEPTIONS" ],
      "libraries": [
        "-L../../build/core", "-lnetwork-server" 
      ]
    }
  ]
}
