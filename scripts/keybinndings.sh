#!/usr/bin/env bash

set -e

# Detect VS Code keybindings location (local or devcontainer)
if [ -d "$HOME/.vscode-server" ]; then
    KEYBINDINGS_DIR="$HOME/.vscode-server/data/Machine"
else
    KEYBINDINGS_DIR="$HOME/.config/Code/User"
fi

mkdir -p "$KEYBINDINGS_DIR"

KEYBINDINGS_FILE="$KEYBINDINGS_DIR/keybindings.json"

echo "Writing VS Code keybindings to:"
echo "  $KEYBINDINGS_FILE"

cat > "$KEYBINDINGS_FILE" << 'EOF'
[
    /* =========================
       CODE NAVIGATION
       ========================= */

    {
        "key": "f12",
        "command": "editor.action.revealDefinition"
    },
    {
        "key": "ctrl+f12",
        "command": "editor.action.goToImplementation"
    },
    {
        "key": "alt+f12",
        "command": "editor.action.peekDefinition"
    },
    {
        "key": "shift+f12",
        "command": "editor.action.referenceSearch.trigger"
    },

    /* Navigate back / forward */
    {
        "key": "alt+left",
        "command": "workbench.action.navigateBack"
    },
    {
        "key": "alt+right",
        "command": "workbench.action.navigateForward"
    },

    /* =========================
       SYMBOL & FILE NAVIGATION
       ========================= */

    {
        "key": "ctrl+shift+o",
        "command": "workbench.action.gotoSymbol"
    },
    {
        "key": "ctrl+p",
        "command": "workbench.action.quickOpen"
    },
    {
        "key": "ctrl+shift+p",
        "command": "workbench.action.showCommands"
    },

    /* Search across codebase */
    {
        "key": "ctrl+shift+f",
        "command": "workbench.action.findInFiles"
    },

    /* =========================
       COMMENT / UNCOMMENT
       ========================= */

    {
        "key": "ctrl+/",
        "command": "editor.action.commentLine",
        "when": "editorTextFocus"
    },
    {
        "key": "ctrl+shift+a",
        "command": "editor.action.blockComment",
        "when": "editorTextFocus"
    },

    /* =========================
       SELECTION & EDITING
       ========================= */

    {
        "key": "ctrl+d",
        "command": "editor.action.addSelectionToNextFindMatch"
    },
    {
        "key": "ctrl+shift+l",
        "command": "editor.action.selectHighlights"
    },

    /* =========================
       DEBUGGING HELPERS
       ========================= */

    {
        "key": "f9",
        "command": "editor.debug.action.toggleBreakpoint"
    },
    {
        "key": "f10",
        "command": "workbench.action.debug.stepOver"
    },
    {
        "key": "f11",
        "command": "workbench.action.debug.stepInto"
    },
    {
        "key": "shift+f11",
        "command": "workbench.action.debug.stepOut"
    }
]
EOF

echo "✅ VS Code keybindings installed successfully"
echo "🔄 Restart VS Code for changes to take effect"
