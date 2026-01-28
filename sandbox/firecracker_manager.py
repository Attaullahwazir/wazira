# sandbox/firecracker_manager.py
"""
Firecracker Manager & Analyzer for Sandbox Module
- Spins up Firecracker micro-VMs per URL
- Injects JS instrumentation via Frida
- Collects syscalls, network calls, and performs behavioral analysis
"""

import subprocess
import uuid
import json
from flask import Flask, request, jsonify
import threading

class FirecrackerManager:
    def __init__(self):
        self.vms = {}  # Map vm_id to process info

    def start_vm(self, url: str) -> str:
        """Spin up a Firecracker micro-VM for the given URL."""
        vm_id = str(uuid.uuid4())
        # TODO: Launch Firecracker VM with proper networking and snapshot
        # Example: subprocess.Popen(["firecracker", ...])
        self.vms[vm_id] = None  # Store process info
        print(f"[sandbox] Started VM {vm_id} for {url}")
        return vm_id

    def inject_js_instrumentation(self, vm_id: str):
        """Inject JS instrumentation via Frida into the VM."""
        # TODO: Use Frida to inject JS into the browser in the VM
        print(f"[sandbox] Injected JS instrumentation into VM {vm_id}")

    def collect_behavioral_data(self, vm_id: str) -> dict:
        """Collect syscalls, network calls, and other behavioral data."""
        # TODO: Collect logs, syscalls, and network traces from the VM
        print(f"[sandbox] Collected behavioral data from VM {vm_id}")
        data = {"syscalls": [], "network": [], "js_events": []}
        return data

    def save_behavioral_data(self, vm_id: str, data: dict, path: str):
        """Save behavioral data to a JSON file."""
        with open(path, "w") as f:
            json.dump(data, f, indent=2)
        print(f"[sandbox] Saved behavioral data for VM {vm_id} to {path}")

    def analyze_behavior(self, data: dict) -> str:
        """Classify as Safe/Suspicious/Danger based on behavioral data."""
        # Simple heuristic: mark as Danger if 'exec' syscall or suspicious domain, Suspicious if any syscalls or network, else Safe
        suspicious_syscalls = {'exec', 'ptrace', 'mprotect'}
        suspicious_domains = {'malware.com', 'phishing.com'}
        if any(s in suspicious_syscalls for s in data.get('syscalls', [])):
            return "Danger"
        if any(d in suspicious_domains for d in data.get('network', [])):
            return "Danger"
        if data.get('syscalls') or data.get('network') or data.get('js_events'):
            return "Suspicious"
        return "Safe"

    def stop_vm(self, vm_id: str):
        """Stop and clean up the Firecracker VM."""
        proc = self.vms.get(vm_id)
        if proc:
            try:
                proc.terminate()
                proc.wait(timeout=5)
                print(f"[sandbox] Terminated VM process for {vm_id}")
            except Exception as e:
                print(f"[sandbox] Error terminating VM {vm_id}: {e}")
        else:
            print(f"[sandbox] No running process for VM {vm_id}")
        self.vms.pop(vm_id, None)
        # TODO: Clean up any VM resources (disk, network, etc.)
        print(f"[sandbox] Stopped VM {vm_id}")

# TODO: Add CLI or API for managing VMs and querying analysis results 

# --- Flask API for JSON report (demo) ---
app = Flask(__name__)
manager = FirecrackerManager()

@app.route('/sandbox/report/<vm_id>', methods=['GET'])
def get_report(vm_id):
    # For demo, collect dummy data and analyze
    data = manager.collect_behavioral_data(vm_id)
    verdict = manager.analyze_behavior(data)
    return jsonify({"vm_id": vm_id, "behavior": data, "verdict": verdict})

# Run Flask app in a thread if this file is run as main
if __name__ == "__main__":
    def run_flask():
        app.run(port=5005)
    threading.Thread(target=run_flask, daemon=True).start()
    # Also run CLI as before
    import argparse

    parser = argparse.ArgumentParser(description="Firecracker Sandbox Manager CLI")
    subparsers = parser.add_subparsers(dest="command", required=True)

    # Start VM
    parser_start = subparsers.add_parser("start", help="Start a Firecracker VM for a URL")
    parser_start.add_argument("url", type=str, help="URL to analyze in the VM")

    # Inject JS Instrumentation
    parser_inject = subparsers.add_parser("inject", help="Inject JS instrumentation into a VM")
    parser_inject.add_argument("vm_id", type=str, help="VM ID to inject JS into")

    # Collect Behavioral Data
    parser_collect = subparsers.add_parser("collect", help="Collect behavioral data from a VM")
    parser_collect.add_argument("vm_id", type=str, help="VM ID to collect data from")
    parser_collect.add_argument("--output", type=str, default=None, help="Output file to save data (JSON)")

    # Analyze Behavior
    parser_analyze = subparsers.add_parser("analyze", help="Analyze behavioral data (JSON file)")
    parser_analyze.add_argument("data_file", type=str, help="Path to JSON file with behavioral data")

    # Stop VM
    parser_stop = subparsers.add_parser("stop", help="Stop a Firecracker VM")
    parser_stop.add_argument("vm_id", type=str, help="VM ID to stop")

    args = parser.parse_args()
    manager = FirecrackerManager()

    if args.command == "start":
        vm_id = manager.start_vm(args.url)
        print(f"Started VM: {vm_id}")
    elif args.command == "inject":
        manager.inject_js_instrumentation(args.vm_id)
    elif args.command == "collect":
        data = manager.collect_behavioral_data(args.vm_id)
        if args.output:
            manager.save_behavioral_data(args.vm_id, data, args.output)
        else:
            print(json.dumps(data, indent=2))
    elif args.command == "analyze":
        import json
        with open(args.data_file, "r") as f:
            data = json.load(f)
        verdict = manager.analyze_behavior(data)
        print(f"Analysis verdict: {verdict}")
    elif args.command == "stop":
        manager.stop_vm(args.vm_id) 