# Sandbox Module

This module provides a secure sandbox environment for analyzing web content using Firecracker micro-VMs, JavaScript instrumentation (Frida), and behavioral analysis.

## Components
- **firecracker_manager.py**: Manages Firecracker VMs, injects JS instrumentation, collects and analyzes behavioral data. Includes a CLI.
- **js_instrumentation.py**: Uses Frida to inject JavaScript into a running process for dynamic analysis.
- **frida_script.js**: Example Frida script that logs calls to `eval` and `document.write`.

## Architecture Overview
1. **Start a VM** for a given URL using Firecracker.
2. **Inject JS instrumentation** into the browser process inside the VM using Frida.
3. **Collect behavioral data** (syscalls, network, JS events) from the VM.
4. **Analyze** the collected data for suspicious or malicious behavior.

## CLI Usage

### Start a VM
```
python firecracker_manager.py start "https://example.com"
```

### Inject JS Instrumentation
```
python firecracker_manager.py inject <vm_id>
```

### Collect Behavioral Data
```
python firecracker_manager.py collect <vm_id> --output data.json
```

### Analyze Behavioral Data
```
python firecracker_manager.py analyze data.json
```

### Stop a VM
```
python firecracker_manager.py stop <vm_id>
```

## Frida JS Instrumentation

- **js_instrumentation.py**: Injects a Frida JS script into a running process (by name or PID).
- **frida_script.js**: Logs calls to `eval` and `document.write`.

Example:
```
python js_instrumentation.py <process_name_or_pid> frida_script.js
```

## Next Steps
- Integrate real Firecracker VM launch and teardown logic.
- Implement Frida injection into browser processes inside VMs.
- Collect real syscalls, network, and JS event data.
- Expand behavioral analysis logic. 