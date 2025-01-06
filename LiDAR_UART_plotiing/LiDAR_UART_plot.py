import serial
import pygame
import math
import re
import threading
from collections import deque
import numpy as np
from scipy.interpolate import CubicSpline
from scipy.interpolate import interp1d

# Klasse for UART-kommunikasjon
class UARTReader:
    def __init__(self, port, baudrate, timeout):
        self.serial = serial.Serial(port, baudrate=baudrate, timeout=timeout)
        self.raw_data_queue = deque()
        self.running = True

    def read_uart(self):
        while self.running:
            try:
                raw_data = self.serial.readline()
                if raw_data:
                    decoded_data = raw_data.decode('ascii', errors='replace').strip()
                    if "[INFO]:" in decoded_data:
                        match = re.search(r"Angle: ([\d\.]+), Distance: ([\d\.]+)", decoded_data)
                        if match:
                            angle = float(match.group(1))
                            distance = float(match.group(2))
                            # print(f"INFO: Angle: {angle}, Distance: {distance}")
                            self.raw_data_queue.append((angle, distance))
                        else:
                            print(f"Ignored message: {decoded_data}")
            except Exception as e:
                print(f"Error reading UART: {e}")

    def stop(self):
        self.running = False
        self.serial.close()

# Klasse for LiDAR-filtrering
class LiDARFiltering:
    def __init__(self, distance_limit=250, distance_offset=2, alpha=0.5):
        self.distance_limit = distance_limit
        self.distance_offset = distance_offset
        self.lock = threading.Lock()
        self.alpha = alpha
        self.distances = [None] * 360  # Array for kvar grad

    def process_data(self, angle, distance):
        if distance > self.distance_limit:
            return
        index = int(angle) % 360  # Sørg for at vinkelen er mellom 0 og 359
        with self.lock:
            if self.distances[index] is None:
                self.distances[index] = distance + self.distance_offset
            else:
                self.distances[index] = self.alpha * (distance + self.distance_offset) + (1 - self.alpha) * self.distances[index]

# Klasse for visualisering
class LiDARVisualizer:
    def __init__(self, filtering):
        pygame.init()
        self.screen = pygame.display.set_mode((800, 800))
        pygame.display.set_caption("LiDAR Visualisering")
        self.font = pygame.font.SysFont("Arial", 20)
        self.filtering = filtering
        self.show_points = True
        self.running = True
        self.scaling = 5
        self.threshold = 3  # Maks antall tomme punkt mellom linjer
       
        # Fargar for visualisering
        self.colors = {
            "recent_points": (0, 0, 255),  # Blå for siste 100 punkt
            "old_points": (0, 255, 0),    # Grøn for eldre punkt
            "lines": (255, 255, 0),       # Gul for linjer mellom punkt
            "ui_background": (0, 0, 255), # Blå for UI-element
            "ui_text": (255, 255, 255),   # Kvit for tekst
            "car": (255, 0, 0),           # Raud bil
            "path": (0, 255, 255)         # Cyan for ønsket sti
        }

    def draw_car(self):
        # Bilens posisjon og dimensjonar
        car_center = (400, 400)
        car_width = 50
        car_height = 20
        wheel_radius = 8
        wheel_offset = 15

        # Teikn bilens karosseri (rektangel)
        car_rect = pygame.Rect(
            car_center[0] - car_width // 2, 
            car_center[1] - car_height // 2, 
            car_width, 
            car_height
        )
        pygame.draw.rect(self.screen, self.colors["car"], car_rect)

        # Teikn hjulene (to sirklar)
        wheel_color = (200, 200, 200)  # Svart farge for hjulene
        pygame.draw.circle(self.screen, wheel_color, 
                        (car_center[0] - car_width // 2 + wheel_offset, car_center[1] - car_height // 2), 
                        wheel_radius)
        pygame.draw.circle(self.screen, wheel_color, 
                        (car_center[0] + car_width // 2 - wheel_offset, car_center[1] - car_height // 2), 
                        wheel_radius)

    def draw_points(self):
        with self.filtering.lock:
            # Iterer gjennom arrayet med 360 avstandar
            for i in range(360):
                distance = self.filtering.distances[i]
                if distance is None:
                    continue

                # Legg til 90 grader for å justere 0 grader til rett fram
                adjusted_angle = (i + 90) % 360

                # Konverter polar til kartesiske koordinatar
                x = 400 + distance * math.cos(math.radians(adjusted_angle)) * self.scaling
                y = 400 - distance * math.sin(math.radians(adjusted_angle)) * self.scaling

                # Vel farge basert på om punktet er blant dei siste 100 målingane
                if i in range(360)[-100:]:
                    color = self.colors["recent_points"]  # Blå for siste 100 punkt
                else:
                    color = self.colors["old_points"]    # Grøn for eldre punkt

                # Teikn punktet
                pygame.draw.circle(self.screen, color, (int(x), int(y)), 2)

    def draw_lines(self):
        with self.filtering.lock:
            distances = self.filtering.distances
            prev_index = None
            max_distance = 50  # Maksimum avstand mellom punkt for å teikne linje
            smoothed_distances = [None] * 360  # Array for lagring av lavpassfiltrerte avstandar
            
            # Utfør lavpassfiltrering på distanse-arrayet
            for i in range(360):
                if distances[i] is None:
                    continue
                # Bruk naboar til å glatte ut verdien
                neighbors = [
                    distances[(i - 1) % 360], distances[i], distances[(i + 1) % 360]
                ]
                valid_neighbors = [d for d in neighbors if d is not None]
                if valid_neighbors:
                    smoothed_distances[i] = sum(valid_neighbors) / len(valid_neighbors)
                else:
                    smoothed_distances[i] = distances[i]

            # Teikn linjer basert på smoothet distansar og avstandssjekk
            for i in range(360):
                if smoothed_distances[i] is None:
                    continue

                # Juster vinkel og berekn koordinatar
                adjusted_angle = (i + 90) % 360
                x1 = 400 + smoothed_distances[i] * math.cos(math.radians(adjusted_angle)) * self.scaling
                y1 = 400 - smoothed_distances[i] * math.sin(math.radians(adjusted_angle)) * self.scaling

                if prev_index is not None:
                    # Kalkuler avstand mellom dette punktet og forrige punkt
                    adjusted_prev_angle = (prev_index + 90) % 360
                    x2 = 400 + smoothed_distances[prev_index] * math.cos(math.radians(adjusted_prev_angle)) * self.scaling
                    y2 = 400 - smoothed_distances[prev_index] * math.sin(math.radians(adjusted_prev_angle)) * self.scaling
                    
                    distance_between_points = math.sqrt((x2 - x1) ** 2 + (y2 - y1) ** 2)
                    if distance_between_points <= max_distance * self.scaling:
                        # Teikn linje berre dersom avstanden er mindre enn maks
                        pygame.draw.line(self.screen, self.colors["lines"], (x1, y1), (x2, y2), 2)

                prev_index = i



    def draw_path(self, path):
        for i in range(len(path) - 1):
            pygame.draw.line(self.screen, self.colors["path"], path[i], path[i + 1], 2)

    def draw_ui(self):
        pygame.draw.rect(self.screen, self.colors["ui_background"], (10, 10, 100, 40))
        pygame.draw.rect(self.screen, self.colors["ui_background"], (120, 10, 140, 40))
        self.screen.blit(self.font.render("Reset", True, self.colors["ui_text"]), (20, 20))
        self.screen.blit(self.font.render("Toggle View", True, self.colors["ui_text"]), (130, 20))

    def handle_events(self):
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                self.running = False
            elif event.type == pygame.MOUSEBUTTONDOWN:
                mouse_x, mouse_y = event.pos
                if 10 <= mouse_x <= 110 and 10 <= mouse_y <= 50:
                    with self.filtering.lock:
                        self.filtering.distances = [None] * 360
                elif 120 <= mouse_x <= 260 and 10 <= mouse_y <= 50:
                    self.show_points = not self.show_points

    def run(self):
        while self.running:
            self.screen.fill((0, 0, 0))
            self.handle_events()
            self.draw_car()
            # path = self.compute_desired_path()
            # if path:
            #     self.draw_path(path)
            if self.show_points:
                self.draw_points()
            else:
                self.draw_lines()
            self.draw_ui()
            pygame.display.flip()
        pygame.quit()

# Hovudprogram
if __name__ == "__main__":
    uart_reader = UARTReader('COM12', 115200, 1)
    lidar_filtering = LiDARFiltering()
    threading.Thread(target=uart_reader.read_uart, daemon=True).start()

    def process_uart_data():
        while uart_reader.running:
            if uart_reader.raw_data_queue:
                angle, distance = uart_reader.raw_data_queue.popleft()
                lidar_filtering.process_data(angle, distance)

    threading.Thread(target=process_uart_data, daemon=True).start()

    visualizer = LiDARVisualizer(lidar_filtering)
    visualizer.run()

    uart_reader.stop()