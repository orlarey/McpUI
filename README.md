# FAUST MCP Server Architecture

A prototype FAUST architecture that enables DSP programs to be controlled via the Model Context Protocol (MCP), making them accessible to AI assistants like Claude Desktop.

## What is the Model Context Protocol (MCP)?

The **Model Context Protocol (MCP)** [https://docs.anthropic.com/en/docs/mcp](https://docs.anthropic.com/en/docs/mcp) is an open standard introduced by Anthropic in November 2024 for enabling AI assistants to connect with external data sources and tools. It defines a client-server architecture using JSON-RPC 2.0 communication over stdio or HTTP.

### MCP Components

* **Tools**: Functions that can be invoked by AI clients (parameters and return values)
* **Resources**: Data sources that can be read by AI clients (files, APIs, databases)
* **Prompts**: Reusable templates for AI interactions
* **Servers**: Applications that expose tools/resources via MCP protocol

### MCP and Audio Software

This FAUST architecture leverages MCP's tool system to expose DSP parameters as callable functions. When integrated with MCP-compatible AI assistants, this enables parameter control through natural language interfaces.

**Technical approach:**

* Each FAUST widget becomes an MCP tool with defined input schema
* Tool names follow hierarchical naming based on FAUST UI structure
* Parameter validation and range clamping handled automatically
* JSON-RPC communication over stdin/stdout

### Reference Documentation

* **MCP Specification**: <https://spec.modelcontextprotocol.io/>
* **Implementation Guide**: <https://docs.anthropic.com/en/docs/build-with-claude/computer-use>
* **Protocol Repository**: <https://github.com/modelcontextprotocol/specification>

***

## Overview

This project provides a **proof-of-concept FAUST architecture** for MCP integration. It consists of two header files that will eventually be integrated into the official FAUST distribution:

* `mcp-protocol.h` - Generic MCP protocol implementation
* `McpUI.h` - FAUST-specific UI integration that implements the `UI` interface

### Current Status: Prototype Phase

**What this is:**

* A standalone prototype demonstrating MCP integration with FAUST
* `main.cpp` simulates what `dsp::buildUserInterface()` would do in a compiled FAUST program
* Each FAUST widget automatically becomes an MCP tool

**What this will become:**

* Native FAUST architecture integrated in the official distribution
* Commands like `faust2mcp myprogram.dsp` will generate MCP servers automatically
* Options like `faust2caqt -mcp myprogram.dsp` will add MCP support to existing architectures

### FAUST Architecture Integration

In the FAUST ecosystem, **architectures** provide the bridge between DSP code and external systems (audio drivers, GUIs, protocols). This MCP architecture follows the same pattern as existing architectures like:

* **OSC Architecture** - Control via OSC messages
* **HTTP Architecture** - Web-based interfaces
* **Jack Architecture** - Professional audio routing
* **Qt Architecture** - Desktop GUI applications

The MCP architecture adds **AI assistant integration** to this list, enabling natural language control of DSP parameters.

## Quick Start (Prototype Testing)

> **Note**: This shows how to test the MCP architecture concept. In the future, FAUST compilation tools will handle this automatically.

### 1. Include the Headers

```C++
#include "McpUI.h"

int main() {
    // Create MCP-enabled UI (simulates FAUST architecture)
    McpUI myUI;
    FAUSTFLOAT level = 0.5f;
    FAUSTFLOAT play = 0.0f;
    
    // Simulate dsp::buildUserInterface() call
    // In real FAUST programs, this code is generated automatically
    myUI.openVerticalBox("Mixer");
    myUI.openVerticalBox("Channel 3");
    myUI.addHorizontalSlider("Level", &level, 0.0f, 0.0f, 1.0f, 0.01f);
    myUI.addButton("play", &play);
    myUI.closeBox();
    myUI.closeBox();
    
    // Start the MCP server (replaces audio loop in normal architectures)
    myUI.run();
    
    return 0;
}
```

### 2. Compile

```Shell
g++ -std=c++20 -I/path/to/faust/architecture main.cpp -o my-mcp-server
```

### 3. Configure Claude Desktop

Add to your Claude Desktop MCP configuration:

```JSON
{
  "mcpServers": {
    "my-audio-app": {
      "command": "/path/to/my-mcp-server"
    }
  }
}
```

### 4. Test with Claude

Claude can now control your simulated FAUST program:

* **"Set the mixer level to 0.8"** → Calls `Level_Channel_3_Mixer` tool
* **"Press the play button"** → Calls `play_Channel_3_Mixer` tool

## How the MCP Architecture Works

### Automatic Tool Generation

The library automatically converts FAUST widgets into MCP tools:

| FAUST Widget                         | MCP Tool Name           | Description              |
| ------------------------------------ | ----------------------- | ------------------------ |
| `addHorizontalSlider("Volume", ...)` | `Volume_Mixer`          | Control volume parameter |
| `addButton("Play", ...)`             | `Play_Mixer`            | Toggle play button       |
| `addVerticalSlider("Freq", ...)`     | `Freq_Oscillator_Mixer` | Control frequency        |

### Tool Naming Convention

Tools are named using the pattern: `{WidgetName}_{Group1}_{Group2}_{...}`

* Widget names and group names are sanitized (alphanumeric + underscore)
* Groups are added from most specific to most general
* Names are limited to 50 characters

### Server Identification

The MCP server automatically identifies itself:

* **Default name**: "AudioApp"
* **Auto-naming**: Uses the root group name (first `openXXXBox()`)
* **Version**: "1.0.0" (customizable)

## API Reference

### McpUI Class

Inherits from FAUST's `UI` class and provides MCP server integration.

#### Methods

```C++
// Start the MCP server (call after building interface)
void run();

// Standard FAUST UI methods
void openVerticalBox(const char* label);
void openHorizontalBox(const char* label);
void openTabBox(const char* label);
void closeBox();

void addButton(const char* label, FAUSTFLOAT* zone);
void addHorizontalSlider(const char* label, FAUSTFLOAT* zone, 
                        FAUSTFLOAT init, FAUSTFLOAT min, FAUSTFLOAT max, FAUSTFLOAT step);
void addVerticalSlider(const char* label, FAUSTFLOAT* zone,
                      FAUSTFLOAT init, FAUSTFLOAT min, FAUSTFLOAT max, FAUSTFLOAT step);
void addNumEntry(const char* label, FAUSTFLOAT* zone,
                FAUSTFLOAT init, FAUSTFLOAT min, FAUSTFLOAT max, FAUSTFLOAT step);
```

### SimpleMCPServer Class

Low-level MCP server (usually not used directly).

```C++
// Set server identification
void setServerName(const std::string& name);
void setServerVersion(const std::string& version);

// Register custom tools
void registerTool(std::unique_ptr<McpTool> tool);
```

## Advanced Usage

### Custom Server Name and Version

```C++
McpUI myUI;
// The first group name becomes the server name
myUI.openVerticalBox("MySynthesizer");  // Server name: "MySynthesizer"

// Or set manually:
// myUI.fServer.setServerName("CustomName");
// myUI.fServer.setServerVersion("2.1.0");
```

### Creating Custom Tools

```C++
class MyCustomTool : public McpTool {
public:
    std::string name() const override { return "my_custom_tool"; }
    
    std::string describe() const override {
        return R"({
            "name": "my_custom_tool",
            "description": "Does something custom",
            "inputSchema": {
                "type": "object",
                "properties": {
                    "value": {"type": "string"}
                },
                "required": ["value"]
            }
        })";
    }
    
    std::string call(const std::string& arguments) override {
        // Handle the tool call
        return "\"Custom action completed\"";
    }
};

// Register with server
auto tool = std::make_unique<MyCustomTool>();
myUI.fServer.registerTool(std::move(tool));
```

## Debugging

### Enable Logging

Uncomment the logging lines in `mcp-protocol.h`:

```C++
// Uncomment these lines for debugging:
static std::ofstream logFile;  // Line ~15
// logFile << "SENDING:" << ...  // Lines in sendResponse/sendError
// logFile.open("debug.log");    // Line in run()
```

### Test MCP Communication

```Shell
echo '{"jsonrpc": "2.0", "id": "1", "method": "tools/list"}' | ./my-mcp-server
```

## Examples

### Simple Oscillator

```C++
#include "McpUI.h"

int main() {
    McpUI ui;
    FAUSTFLOAT freq = 440.0f;
    FAUSTFLOAT gain = 0.5f;
    FAUSTFLOAT gate = 0.0f;
    
    ui.openVerticalBox("Oscillator");
    ui.addHorizontalSlider("Frequency", &freq, 440.0f, 20.0f, 20000.0f, 1.0f);
    ui.addHorizontalSlider("Gain", &gain, 0.5f, 0.0f, 1.0f, 0.01f);
    ui.addButton("Gate", &gate);
    ui.closeBox();
    
    ui.run();
    return 0;
}
```

**Generated MCP Tools:**

* `Frequency_Oscillator` - Control frequency (20-20000 Hz)
* `Gain_Oscillator` - Control volume (0.0-1.0)
* `Gate_Oscillator` - Toggle gate on/off

### Multi-Channel Mixer

```C++
#include "McpUI.h"

int main() {
    McpUI ui;
    FAUSTFLOAT ch1_level = 0.8f, ch2_level = 0.6f;
    FAUSTFLOAT ch1_mute = 0.0f, ch2_mute = 0.0f;
    
    ui.openVerticalBox("Mixer");
    
    ui.openVerticalBox("Channel 1");
    ui.addHorizontalSlider("Level", &ch1_level, 0.8f, 0.0f, 1.0f, 0.01f);
    ui.addButton("Mute", &ch1_mute);
    ui.closeBox();
    
    ui.openVerticalBox("Channel 2");
    ui.addHorizontalSlider("Level", &ch2_level, 0.6f, 0.0f, 1.0f, 0.01f);
    ui.addButton("Mute", &ch2_mute);
    ui.closeBox();
    
    ui.closeBox();
    
    ui.run();
    return 0;
}
```

**Generated MCP Tools:**

* `Level_Channel_1_Mixer` - Channel 1 volume
* `Mute_Channel_1_Mixer` - Channel 1 mute
* `Level_Channel_2_Mixer` - Channel 2 volume
* `Mute_Channel_2_Mixer` - Channel 2 mute

## Requirements

* **C++20** compiler
* **FAUST** headers (`faust/gui/UI.h`)
* **MCP client** (Claude Desktop, or custom client)

## Limitations

* **Header-only**: No external dependencies, but increases compile time
* **JSON parsing**: Basic implementation, not a full JSON parser
* **Single-threaded**: MCP server runs in main thread
* **No authentication**: MCP communication is unencrypted

## Contributing

This is a prototype architecture for eventual FAUST integration. To contribute:

### Prototype Phase (Current)

1. **Test with different FAUST programs** - Simulate various `buildUserInterface()` patterns
2. **Improve MCP protocol implementation** - Enhance `mcp-protocol.h`
3. **Enhance FAUST integration** - Improve `McpUI.h` features
4. **Report integration issues** - Document architecture compatibility

### Integration Phase (Future)

1. **FAUST core integration** - Work with GRAME team for official inclusion
2. **Template development** - Create `faust2mcp` and architecture templates
3. **Cross-platform testing** - Ensure compatibility across FAUST targets
4. **Documentation** - Integrate with official FAUST documentation

### Testing Guidelines

Test the architecture with different FAUST patterns:

```C++
// Test hierarchical grouping
ui.openTabBox("Main");
ui.openHorizontalBox("Section1");
// ... widgets ...

// Test parameter ranges
ui.addHorizontalSlider("Freq", &freq, 440.0f, 20.0f, 20000.0f, 1.0f);

// Test naming edge cases  
ui.addButton("Play/Pause", &play);  // Special characters
ui.addSlider("Level (dB)", &level); // Parentheses
```

## Related FAUST Architectures

This MCP architecture complements existing FAUST architectures:

| Architecture | Protocol               | Use Case                           | Status       |
| ------------ | ---------------------- | ---------------------------------- | ------------ |
| **OSC**      | Open Sound Control     | Real-time control, DAW integration | ✅ Official   |
| **HTTP**     | REST/WebSocket         | Web interfaces, remote control     | ✅ Official   |
| **MCP**      | Model Context Protocol | AI assistant integration           | 🔄 Prototype |
| **MIDI**     | MIDI CC                | Hardware controllers               | ✅ Official   |
| **Qt/GTK**   | Native GUI             | Desktop applications               | ✅ Official   |

### Comparison with OSC Architecture

Both OSC and MCP architectures enable remote control, but serve different purposes:

**OSC Architecture:**

* Real-time, low-latency control
* Designed for musical performance and DAW integration
* Binary protocol optimized for audio applications
* Mature ecosystem of OSC controllers

**MCP Architecture:**

* Human-language control via AI assistants
* Designed for accessibility and natural interaction
* JSON-RPC protocol optimized for structured communication
* Emerging ecosystem of AI-powered tools

## Requirements

* **C++20** compiler
* **FAUST** headers (`faust/gui/UI.h`)
* **MCP client** (Claude Desktop, or custom implementation)

## Limitations

### Current Prototype Limitations

* **No audio processing** - Only parameter control (by design)
* **Single-threaded** - MCP server runs in main thread
* **Basic JSON parsing** - Lightweight implementation, not full JSON parser
* **No authentication** - MCP communication is unencrypted

### Architecture Design Limitations

* **Parameter-only control** - Cannot trigger DSP recompilation or structural changes
* **One-way communication** - Cannot send parameter updates back to MCP client
* **Static interface** - UI structure determined at buildUserInterface() time

These limitations are by design and align with FAUST's architecture philosophy.

## Acknowledgments

Built upon the FAUST architecture system designed by Dominique Fober, Yann Orlarey, and Stéphane Letz at GRAME.
