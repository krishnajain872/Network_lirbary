# Debugging Guide

## Prerequisites

*   **VSCode** installed.
*   **C/C++ Extension** (ms-vscode.cpptools) installed.
*   **CMake Tools** (ms-vscode.cmake-tools) installed.
*   **GDB** installed on your system (`sudo apt install gdb`).

## Setup

1.  Run the setup script:
    ```bash
    cd vscode
    ./setup_debug.sh
    ```

## Debugging

1.  Open the `network-library` folder in VSCode.
2.  Press **F5** (or go to the Run and Debug tab).
3.  Select the configuration you want to run:
    *   **(gdb) Launch Network Server**: Runs the main server with the basic config.
    *   **(gdb) Launch gRPC E2E Test**: Runs the end-to-end gRPC integration test.

## Troubleshooting

*   **Breakpoints not hit:** Ensure you compiled with `-DCMAKE_BUILD_TYPE=Debug` (the tasks.json handles this).
*   **Config not found:** Check the `cwd` in `launch.json` matches your build directory.

## Windows Development (Dev Containers)

Since this library uses Linux-specific APIs (`epoll`), you cannot compile the core engine natively on Windows (MSVC).
Use **VSCode Dev Containers** to develop inside a Linux environment on Windows.

1.  Install **Docker Desktop** and **VSCode**.
2.  Install the **Dev Containers** extension (`ms-vscode-remote.remote-containers`).
3.  Open the project folder.
4.  Click **Reopen in Container** (bottom right popup or F1 -> Dev Containers: Reopen in Container).

This will spin up a full Linux development environment with GDB, CMake, and all dependencies pre-installed.
