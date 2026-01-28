import frida
import sys


def on_message(message, data):
    if message["type"] == "send":
        print(f"[frida] {message['payload']}")
    else:
        print(f"[frida][{message['type']}] {message}")


def main():
    if len(sys.argv) != 3:
        print(f"Usage: python {sys.argv[0]} <process_name_or_pid> <frida_script.js>")
        sys.exit(1)

    target = sys.argv[1]
    script_path = sys.argv[2]

    try:
        with open(script_path) as f:
            script_code = f.read()
    except Exception as e:
        print(f"Error reading script: {e}")
        sys.exit(1)

    try:
        if target.isdigit():
            session = frida.attach(int(target))
        else:
            session = frida.attach(target)
        script = session.create_script(script_code)
        script.on('message', on_message)
        script.load()
        print(f"[frida] Script loaded. Press Ctrl+C to exit.")
        sys.stdin.read()
    except Exception as e:
        print(f"Error: {e}")
        sys.exit(1)


if __name__ == "__main__":
    main() 