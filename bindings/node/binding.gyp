{
  "targets": [
    {
      "target_name": "networklib_node",
      "cflags!": [ "-fno-exceptions" ],
      "cflags_cc!": [ "-fno-exceptions" ],
      "sources": [ "src/networklib_node.cpp" ],
      "include_dirs": [
        "<!@(node -p \"require('node-addon-api').include\")",
        "../../core/include",
        "../../build"
      ],
      "defines": [ "NAPI_CPP_EXCEPTIONS" ],
      "libraries": [
        "-L<(module_root_dir)/../../build/lib",
        "-lnetworklib",
        "-Wl,-rpath,<(module_root_dir)/../../build/lib"
      ]
    }
  ]
}
