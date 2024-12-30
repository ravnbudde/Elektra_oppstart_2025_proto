import serial
import pygame
import math
import re
import threading
from collections import deque

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
YELLOW = (255, 255, 0)

# Sentrum av skjermen
center_x, center_y = 400, 400
scaling_factor = 5

# Data og innstillinger
data_points = deque(maxlen=500)  # Lagrer opp til 500 punkt
filtered_points = {}  # Lagrer filtrerte punkt
distance_limit = 60  # Maksimum avstand i cm
distance_offset = 2  # Offset mellom sensor og rotasjonssenter
alpha = 0.7  # Filterparameter
lock = threading.Lock()  # For å sikre trådtrygg dataoppdatering
show_points = True  # Bytt mellom punkter og linjer
motor_running = False  # Status for motoren

# Send kommando til ESP
def send_command(command):
    global motor_running
    try:
        ser.write(f"{command}\n".encode())  # Send kommando
        print(f"Sent command: {command}")
        response = ser.readline().decode('ascii', errors='replace').strip()  # Les respons
        print(response)
        if response:
            print(f"ESP response: {response}")
            if "START" in command:
                motor_running = True
                print("ESP bekreftet: Motor startet.")
            elif "STOP" in command:
                motor_running = False
                print("ESP bekreftet: Motor stoppet.")
        else:
            print("Ingen respons mottatt fra ESP.")
    except Exception as e:
        print(f"Error sending command: {e}")

# Oppdateringsfunksjon for UART
def read_uart():
    global data_points, filtered_points
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
                            # Beregn vinkelen i radianer og legg til offset
                            rad_angle = math.radians(angle + 90)
                            corrected_distance = distance + distance_offset  # Legg til offset til avstanden
                            # Beregn posisjon med korrigert avstand
                            x = center_x + corrected_distance * math.cos(rad_angle) * scaling_factor
                            y = center_y - corrected_distance * math.sin(rad_angle) * scaling_factor
                            with lock:
                                # Legg til punkt i rådata
                                data_points.append((x, y))

                                # Filtrering
                                if angle in filtered_points:
                                    prev_x, prev_y = filtered_points[angle]
                                    filtered_x = alpha * prev_x + (1 - alpha) * x
                                    filtered_y = alpha * prev_y + (1 - alpha) * y
                                    filtered_points[angle] = (filtered_x, filtered_y)
                                else:
                                    filtered_points[angle] = (x, y)
                    # else:
                        # print(f"Ignored message: {decoded_data}")
        except Exception as e:
            print(f"Error reading UART: {e}")
            break

# Start UART-oppdatering i eigen tråd
threading.Thread(target=read_uart, daemon=True).start()

# Hovudløkka
running = True
while running:
    screen.fill(BLACK)

    with lock:
        if show_points:
            # Tegn siste 100 punkt som blå
            for point in list(data_points)[-100:]:
                pygame.draw.circle(screen, BLUE, (int(point[0]), int(point[1])), 3)

            # Tegn alle punkt som grøn
            for point in list(data_points)[:-100]:
                pygame.draw.circle(screen, GREEN, (int(point[0]), int(point[1])), 2)
        else:
            # Tegn linjer mellom filtrerte punkt
            sorted_angles = sorted(filtered_points.keys())
            for i in range(len(sorted_angles) - 1):
                angle1 = sorted_angles[i]
                angle2 = sorted_angles[i + 1]
                x1, y1 = filtered_points[angle1]
                x2, y2 = filtered_points[angle2]
                pygame.draw.line(screen, YELLOW, (int(x1), int(y1)), (int(x2), int(y2)), 2)

    # Tegn kontrollar
    pygame.draw.rect(screen, BLUE, (10, 10, 100, 40))  # Reset-knapp
    pygame.draw.rect(screen, BLUE, (120, 10, 140, 40))  # Bytt mellom punkter og linjer
    pygame.draw.rect(screen, BLUE, (270, 10, 100, 40))  # Start/stopp motor
    screen.blit(font.render("Reset", True, WHITE), (20, 20))
    screen.blit(font.render("Toggle View", True, WHITE), (130, 20))
    screen.blit(font.render("Start/Stop", True, WHITE), (280, 20))

    # Eventhandtering
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            running = False
        elif event.type == pygame.MOUSEBUTTONDOWN:
            mouse_x, mouse_y = event.pos
            if 10 <= mouse_x <= 110 and 10 <= mouse_y <= 50:  # Reset-knappen
                with lock:
                    data_points.clear()
                    filtered_points.clear()
            elif 120 <= mouse_x <= 260 and 10 <= mouse_y <= 50:  # Bytt mellom punkter og linjer
                show_points = not show_points
            elif 270 <= mouse_x <= 370 and 10 <= mouse_y <= 50:  # Start/stopp motor
                if motor_running:
                    send_command("STOP")
                else:
                    send_command("START")

    pygame.display.flip()

# Lukk programmet
ser.close()
pygame.quit()
