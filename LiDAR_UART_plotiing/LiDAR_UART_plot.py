import serial
import pygame
import math
import re
import threading

# Konfigurer UART
ser = serial.Serial('COM12', baudrate=115200, timeout=1)

# Initialiser Pygame
pygame.init()
screen = pygame.display.set_mode((800, 800))
pygame.display.set_caption("LiDAR Visualisering")
font = pygame.font.SysFont("Arial", 20)

# Fargar
BLACK = (0, 0, 0)
WHITE = (255, 255, 255)
GREEN = (0, 255, 0)
BLUE = (0, 0, 255)

# Sentrum av skjermen
center_x, center_y = 400, 400
scale_factor = 3

# Data og innstillinger
data_points = []
distance_limit = 100  # Maksimum avstand i cm
lock = threading.Lock()  # For å sikre trådtrygg dataoppdatering

# Oppdateringsfunksjon for UART
def read_uart():
    global data_points
    while True:
        try:
            raw_data = ser.readline()
            if raw_data:
                decoded_data = raw_data.decode('ascii', errors='replace').strip()
                if "[INFO]:" in decoded_data:
                    # Match vinkel og avstand med regex
                    match = re.search(r"Angle: ([\d\.]+), Distance: ([\d\.]+)", decoded_data)
                    if match:
                        angle = float(match.group(1))
                        distance = float(match.group(2))

                        # Print ut verdiane for feilsøking
                        print(f"INFO: Angle: {angle}, Distance: {distance}")

                        # Oppdater data for plotting om det er innan grensa
                        if distance <= distance_limit:
                            
                            # Beregn posisjon med justert vinkel
                            rad_angle = math.radians(angle + 90)
                            x = center_x + distance * math.cos(rad_angle) * scale_factor
                            y = center_y - distance * math.sin(rad_angle) * scale_factor

                            with lock:
                                data_points.append((x, y))
                else:
                    print(f"Ignored message: {decoded_data}")
        except Exception as e:
            print(f"Error reading UART: {e}")
            break

# Start UART-oppdatering i eigen tråd
threading.Thread(target=read_uart, daemon=True).start()

# Hovudløkka
running = True
while running:
    screen.fill(BLACK)

    # Tegn alle datapunkt
    with lock:
        for point in data_points:
            pygame.draw.circle(screen, GREEN, (int(point[0]), int(point[1])), 3)

    # Tegn kontrollar
    pygame.draw.rect(screen, BLUE, (10, 10, 100, 40))  # Reset-knapp
    pygame.draw.rect(screen, BLUE, (120, 10, 100, 40))  # Endre avstandsgrense
    screen.blit(font.render("Reset", True, WHITE), (20, 20))
    screen.blit(font.render(f"Limit: {distance_limit}", True, WHITE), (130, 20))

    # Eventhandtering
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            running = False
        elif event.type == pygame.MOUSEBUTTONDOWN:
            mouse_x, mouse_y = event.pos
            if 10 <= mouse_x <= 110 and 10 <= mouse_y <= 50:  # Reset-knappen
                with lock:
                    data_points = []
            elif 120 <= mouse_x <= 220 and 10 <= mouse_y <= 50:  # Endre avstandsgrense
                distance_limit = 40 if distance_limit == 100 else 100

    pygame.display.flip()

# Lukk programmet
ser.close()
pygame.quit()



'''
### THE OG
import serial
import matplotlib.pyplot as plt
import numpy as np
import re

# Configure the serial port
ser = serial.Serial('COM12', baudrate=115200, timeout=1)

# Initialize live plotting
plt.ion()
fig, ax = plt.subplots(subplot_kw={'projection': 'polar'})
scatter, = ax.plot([], [], 'bo')

angles = []
distances = []

print("Listening on COM12...")

try:
    while True:
        raw_data = ser.readline()  # Read a line from UART
        if raw_data:
            decoded_data = raw_data.decode('ascii', errors='replace').strip()
            if "[INFO]:" in decoded_data:
                # Match angle and distance using regex
                match = re.search(r"Angle: ([\d\.]+), Distance: ([\d\.]+)", decoded_data)
                if match:
                    angle = float(match.group(1))
                    distance = float(match.group(2))

                    # Print extracted values
                    print(f"INFO: Angle: {angle}, Distance: {distance}")

                    # Update data for plotting
                    angles.append(np.deg2rad(angle))
                    distances.append(distance)

                    # Update the plot
                    scatter.set_data(angles, distances)
                    ax.set_ylim(0, max(distances) + 10)
                    plt.pause(0.01)
                else:
                    print(f"Could not parse INFO message: {decoded_data}")
            else:
                print(f"Ignored message: {decoded_data}")
except KeyboardInterrupt:
    print("Exiting program.")
    ser.close()
    plt.ioff()
    plt.show()
'''