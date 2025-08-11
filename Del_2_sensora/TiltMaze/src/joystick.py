# joystick_udp_headless.py

import os
# Use the “dummy” video driver so no window is created
os.environ["SDL_VIDEODRIVER"] = "dummy"

import pygame
import socket
import struct
import time
import sys

# ─── Configuration ────────────────────────────────
UDP_IP    = "127.0.0.1"
UDP_PORT  = 5005
SEND_RATE = 30          # Hz
# ──────────────────────────────────────────────────

def main():
    # Initialize Pygame with only joystick support
    pygame.init()
    pygame.joystick.init()

    if pygame.joystick.get_count() == 0:
        print("No joystick detected.", file=sys.stderr)
        sys.exit(1)

    joy = pygame.joystick.Joystick(0)
    joy.init()
    print(f"Joystick detected: {joy.get_name()}", file=sys.stderr)

    # Set up UDP socket
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    delay = 1.0 / SEND_RATE

    try:
        while True:
            # Pump events so joystick state updates
            pygame.event.pump()

            # Read axes
            x = joy.get_axis(0)
            y = joy.get_axis(1)

            # Pack and send raw floats
            msg = struct.pack("!ff", x, y)
            sock.sendto(msg, (UDP_IP, UDP_PORT))

            time.sleep(delay)
    except KeyboardInterrupt:
        pass
    finally:
        sock.close()
        joy.quit()
        pygame.quit()

if __name__ == "__main__":
    main()
