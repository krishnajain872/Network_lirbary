#!/usr/bin/env bash
set -e

echo "===================================="
echo " VS Code Full Productivity Setup"
echo "===================================="

# Detect distro
if [ -f /etc/debian_version ]; then
    DISTRO="debian"
elif [ -f /etc/redhat-release ]; then
    DISTRO="redhat"
else
    echo "Unsupported distro"
    exit 1
fi

# -----------------------------
# Install VS Code
# -----------------------------
if ! command -v code >/dev/null 2>&1; then
    echo "[+] Installing VS Code"

    if [ "$DISTRO" = "debian" ]; then
        sudo apt update
        sudo apt install -y wget gpg
        wget -qO- https://packages.microsoft.com/keys/microsoft.asc | gpg --dearmor > packages.microsoft.gpg
        sudo install -o root -g root -m 644 packages.microsoft.gpg /etc/apt/trusted.gpg.d/
        sudo sh -c 'echo "deb [arch=amd64] https://packages.microsoft.com/repos/code stable main" > /etc/apt/sources.list.d/vscode.list'
        sudo apt update
        sudo apt install -y code
    else
        sudo rpm --import https://packages.microsoft.com/keys/microsoft.asc
        sudo sh -c 'echo -e "[code]\nname=Visual Studio Code\nbaseurl=https://packages.microsoft.com/yumrepos/vscode\nenabled=1\ngpgcheck=1\ngpgkey=https://packages.microsoft.com/keys/microsoft.asc" > /etc/yum.repos.d/vscode.repo'
        sudo dnf install -y code
    fi
else
    echo "[✓] VS Code already installed"
fi

# -----------------------------
# Install Extensions
# -----------------------------
echo "[+] Installing Extensions"

EXTENSIONS=(
    ms-vscode.cpptools
    ms-vscode.cpptools-extension-pack
    ms-python.python
    ms-python.vscode-pylance
    ms-vscode.cmake-tools
    twxs.cmake
    eamodio.gitlens
    ms-azuretools.vscode-docker
    redhat.vscode-yaml
    streetsidesoftware.code-spell-checker
    vscode-icons-team.vscode-icons
)

for ext in "${EXTENSIONS[@]}"; do
    code --install-extension "$ext" || true
done

# -----------------------------
# Create VS Code config dirs
# -----------------------------
VSCODE_DIR="$HOME/.config/Code/User"
mkdir -p "$VSCODE_DIR"

# -----------------------------
# settings.json
# -----------------------------
cat > "$VSCODE_DIR/settings.json" << 'EOF'
{
    // ---------- Editor ----------
    "editor.fontFamily": "Fira Code, JetBrains Mono, monospace",
    "editor.fontLigatures": true,
    "editor.fontSize": 14,
    "editor.lineHeight": 24,
    "editor.tabSize": 4,
    "editor.insertSpaces": true,
    "editor.rulers": [100, 120],
    "editor.renderWhitespace": "boundary",
    "editor.bracketPairColorization.enabled": true,
    "editor.guides.bracketPairs": true,
    "editor.minimap.enabled": true,

    // ---------- Navigation ----------
    "editor.gotoLocation.multipleDefinitions": "peek",
    "editor.gotoLocation.multipleDeclarations": "peek",
    "editor.gotoLocation.multipleImplementations": "peek",
    "editor.gotoLocation.multipleReferences": "peek",

    "breadcrumbs.enabled": true,
    "breadcrumbs.symbolPath": "on",
    "outline.showVariables": true,

    // ---------- IntelliSense ----------
    "editor.quickSuggestions": {
        "other": true,
        "comments": false,
        "strings": true
    },
    "editor.suggestSelection": "first",
    "editor.parameterHints.enabled": true,

    // ---------- Search ----------
    "search.smartCase": true,
    "search.useGlobalIgnoreFiles": true,

    // ---------- C / C++ ----------
    "C_Cpp.intelliSenseEngine": "default",
    "C_Cpp.autocomplete": "default",
    "C_Cpp.enhancedColorization": "enabled",

    // ---------- Git ----------
    "git.enableSmartCommit": true,
    "git.confirmSync": false,

    // ---------- UI ----------
    "workbench.iconTheme": "vscode-icons",
    "workbench.startupEditor": "none",
    "workbench.editor.enablePreview": false,

    // ---------- Files ----------
    "files.autoSave": "afterDelay",
    "files.autoSaveDelay": 1000
}
EOF

# -----------------------------
# keybindings.json
# -----------------------------
cat > "$VSCODE_DIR/keybindings.json" << 'EOF'
[
    {
        "key": "f12",
        "command": "editor.action.revealDefinition"
    },
    {
        "key": "alt+f12",
        "command": "editor.action.peekDefinition"
    },
    {
        "key": "shift+f12",
        "command": "editor.action.referenceSearch.trigger"
    },
    {
        "key": "ctrl+shift+o",
        "command": "workbench.action.gotoSymbol"
    },
    {
        "key": "ctrl+t",
        "command": "workbench.action.showAllSymbols"
    },
    {
        "key": "ctrl+p",
        "command": "workbench.action.quickOpen"
    },
    {
        "key": "ctrl+shift+p",
        "command": "workbench.action.showCommands"
    },
    {
        "key": "ctrl+`",
        "command": "workbench.action.terminal.toggleTerminal"
    }
]
EOF

echo "===================================="
echo " VS Code setup completed successfully"
echo " Restart VS Code to apply settings"
echo "===================================="
