# Windows Development Guide

This project uses **VS Code Dev Containers** to provide a consistent Linux-based development environment on Windows.

## One-Click Setup

1.  **Right-click** `scripts/bootstrap_windows.ps1` and select **Run with PowerShell**.
2.  Follow the prompts (accept admin privileges if asked).
3.  The script will:
    *   Install **Docker Desktop** (if missing).
    *   Install **VS Code** (if missing).
    *   Install the **Dev Containers** extension.
    *   Launch the project in VS Code.

## Manual Setup

If you prefer to set up manually:

1.  **Install Prerequisites:**
    *   [Docker Desktop](https://www.docker.com/products/docker-desktop)
    *   [Visual Studio Code](https://code.visualstudio.com/)
    *   [Dev Containers Extension](https://marketplace.visualstudio.com/items?itemName=ms-vscode-remote.remote-containers)

2.  **Start Coding:**
    *   Open the `network-library` folder in VS Code.
    *   Press `F1` and run: `Dev Containers: Reopen in Container`.
    *   Wait for the container to build (first time takes a few minutes).

## Debugging

Once inside the Dev Container:
1.  Open `core/src/server_main.cpp`.
2.  Set a breakpoint (F9).
3.  Press **F5** to start debugging.
