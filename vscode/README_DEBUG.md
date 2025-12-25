# Debugging Guide

## Prerequisites

*   **VSCode** installed.
*   **C/C++ Extension** (ms-vscode.cpptools) installed.
*   **CMake Tools** (ms-vscode.cmake-tools) installed.
*   **GDB** installed on your system (`sudo apt install gdb`).

> **Note for Windows Users:** You must be running VS Code inside the **Dev Container** or connected via **Remote SSH** to the Docker container for these steps to work.

## Setup

1.  Open the project in the Dev Container (or connect via SSH).
2.  VS Code should automatically configure the project. If not, run the `CMake: Configure (Debug)` task from the command palette.

## Debugging

### Option 1: Start in Debug Mode (Launch)

This is the easiest way to debug. VS Code will build the project and start the executable with GDB attached.

1.  Press **F5** or click the **Run and Debug** icon in the Activity Bar (left side).
2.  Select **(gdb) Launch Network Server** from the dropdown at the top.
3.  Click the **Play** button.
    *   VS Code will run the "Build All" task first.
    *   The server will start, and you can see output in the **Debug Console** or **Terminal**.
    *   Breakpoints will be hit automatically.

### Option 2: Attach to a Running Process

Use this if the application is already running (e.g., started manually in a terminal).

1.  Start the application manually in the terminal:
    ```bash
    # Example: Start the server in the background
    ./build/network-server &
    ```
2.  In VS Code, go to **Run and Debug**.
3.  Select **(gdb) Attach to Process**.
4.  Click the **Play** button.
5.  A dropdown will appear asking you to select the process. Type `network-server` to filter and select the running process.
6.  Enter your password if prompted (default is `root` in the dev container).

## Configuration Details

The debugging setup is defined in `.vscode/launch.json`.

*   **Launch Network Server:** Starts `build/network-server`.
*   **Launch gRPC E2E Test:** Starts `build/test_grpc_e2e`.
*   **Attach to Process:** Connects to any running PID.

## Troubleshooting

*   **"ptrace: Operation not permitted":**
    *   This means Docker is blocking the debugger.
    *   Ensure the container was started with `--cap-add=SYS_PTRACE` and `--security-opt seccomp=unconfined`.
    *   If you used `scripts/setup_dev_env.ps1` or the Dev Container, this is already handled.
*   **Breakpoints not hit:**
    *   Ensure the build type is Debug. Run the **CMake: Configure (Debug)** task.
