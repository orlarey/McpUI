# MCP Integration in Faust ca-qt Architecture

## Overview

This document describes the modifications made to the Faust `ca-qt.cpp` architecture file to integrate Model Context Protocol (MCP) server functionality. The integration allows Faust applications to expose their DSP parameters and controls through the MCP protocol, enabling interaction with MCP-compatible clients like Claude Desktop.

This guide can serve as a reference for integrating MCP server functionality into other Faust architecture files.

## Key Modifications

### 1. Header Includes

**Added threading support:**
```cpp
#include <thread>  // Line 40
```

**Added MCP includes (already present):**
```cpp
#include "faust/mcp/MCPUI.h"        // Line 49
#include "faust/mcp/mcp-protocol.h" // Line 50
```

### 2. Command Line Option

**Added `--mcp` flag support:**
- Added `--mcp` to help message (line 265)
- Added MCP option detection: `bool mcp = isopt(argv, "--mcp");` (line 261)

### 3. MCP Server Integration

**Conditional MCP server creation and execution:**
```cpp
// MCP operations (lines 352-358)
McpUI       myMcpUI;
std::thread mcpThread;

if (mcp) {
    DSP->buildUserInterface(&myMcpUI);
    mcpThread = std::thread([&myMcpUI]() { myMcpUI.run(); });
}
```

**Key features:**
- MCP server only runs when `--mcp` flag is provided
- Runs in a separate thread to avoid blocking the main Qt application
- Uses lambda function to capture and execute `myMcpUI.run()`

### 4. Protocol Hygiene

**Suppressed non-protocol output when MCP is active:**
```cpp
if (!mcp) {
    // If MCP we don't want a non protocol message to be sent
    cout << "ins " << audio.getNumInputs() << endl;
    cout << "outs " << audio.getNumOutputs() << endl;
}
```

This prevents contamination of the MCP JSON-RPC communication channel with debug messages.

### 5. Thread Management

**Clean thread termination:**
```cpp
// Force termination of the MCP thread if active (lines 457-459)
if (mcp && mcpThread.joinable()) {
    mcpThread.detach();
}
```

The MCP thread is detached rather than joined, allowing the application to terminate immediately without waiting for the MCP server to finish.


## Usage

To enable MCP functionality in a Faust application:

```bash
# Compile with MCP support
faust2caqt myDSP.dsp

# Run with MCP server enabled
./myDSP --mcp
```
