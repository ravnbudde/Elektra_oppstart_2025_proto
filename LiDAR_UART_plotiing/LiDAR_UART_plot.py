import serial
import pygame
import math
import re
import threading
from collections import deque
import numpy as np
from scipy.interpolate import CubicSpline
from scipy.interpolate import interp1d


# Klasse for Kalman-filter
class KalmanFilter:
    def __init__(self, process_variance, measurement_variance):
        self.process_variance = process_variance
        self.measurement_variance = measurement_variance
        self.estimate = 0
        self.uncertainty = 1

    def update(self, measurement):
        kalman_gain = self.uncertainty / (self.uncertainty + self.measurement_variance)
        self.estimate = self.estimate + kalman_gain * (measurement - self.estimate)
        self.uncertainty = (1 - kalman_gain) * self.uncertainty

    def predict(self, control_input=0):
        self.estimate += control_input
        self.uncertainty += self.process_variance

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
    def __init__(self, distance_limit=60, distance_offset=2, process_variance=0.1, measurement_variance=1.0):
        self.distance_limit = distance_limit
        self.distance_offset = distance_offset
        self.kalman_filters = {}
        self.data_points = deque(maxlen=500)
        self.filtered_points = {}
        self.lock = threading.Lock()
        self.process_variance = process_variance
        self.measurement_variance = measurement_variance

    def process_data(self, angle, distance):
        if distance <= self.distance_limit:
            rad_angle = math.radians(angle + 90)
            corrected_distance = distance + self.distance_offset
            x = 400 + corrected_distance * math.cos(rad_angle) * 5
            y = 400 - corrected_distance * math.sin(rad_angle) * 5

            with self.lock:
                self.data_points.append((x, y))

                if angle not in self.kalman_filters:
                    self.kalman_filters[angle] = KalmanFilter(self.process_variance, self.measurement_variance)

                self.kalman_filters[angle].update(distance)
                filtered_distance = self.kalman_filters[angle].estimate

                filtered_x = 400 + (filtered_distance + self.distance_offset) * math.cos(rad_angle) * 5
                filtered_y = 400 - (filtered_distance + self.distance_offset) * math.sin(rad_angle) * 5
                self.filtered_points[angle] = (filtered_x, filtered_y)

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
        self.distance_threshold = 10  # Maksimum avstand mellom punkt for å teikne linje
        
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



    # def compute_desired_path(self):
    #     # Finn void-regionar
    #     with self.filtering.lock:
    #         points = list(self.filtering.filtered_points.values())

    #     if len(points) < 3:
    #         return []  # For få punkt til å generere ein sti

    #     # Vel enkle kontrollpunkt for Bezier-kurve (midtpunkt i void)
    #     mid_x = 400
    #     mid_y = 200  # Enklare sti "rett fram"
    #     control_points = [(400, 400), (mid_x, mid_y), (400, 100)]

    #     # Generer Bezier-kurve manuelt
    #     bez_curve = []
    #     for t in np.linspace(0, 1, 100):
    #         x = (1 - t)**2 * control_points[0][0] + 2 * (1 - t) * t * control_points[1][0] + t**2 * control_points[2][0]
    #         y = (1 - t)**2 * control_points[0][1] + 2 * (1 - t) * t * control_points[1][1] + t**2 * control_points[2][1]
    #         bez_curve.append((x, y))

    #     return bez_curve

    def draw_points(self):
        with self.filtering.lock:
            # Teikn dei siste 100 punkta i blått
            for point in list(self.filtering.data_points)[-100:]:
                pygame.draw.circle(self.screen, self.colors["recent_points"], (int(point[0]), int(point[1])), 2)
            # Teikn eldre punkt i grønt
            for point in list(self.filtering.data_points)[:-100]:
                pygame.draw.circle(self.screen, self.colors["old_points"], (int(point[0]), int(point[1])), 2)

    def draw_lines(self):
        with self.filtering.lock:
            points = list(self.filtering.filtered_points.values())
            if len(points) > 1:
                # Center for berekning
                center = (400, 400)

                # Sorter punkta basert på vinkelen frå origo
                points.sort(key=lambda p: math.atan2(p[1] - center[1], p[0] - center[0]))
                
                # Ekstrahere x og y-koordinatar
                x_coords = [p[0] for p in points]
                y_coords = [p[1] for p in points]

                # Bruke numpy for interpolering
                t = np.linspace(0, 1, len(points))  # Parametrisk t verdiar
                t_interp = np.linspace(0, 1, len(points) * 10)  # Meir granular interpolering

                # Interpoler x og y
                x_interp = interp1d(t, x_coords, kind='cubic')(t_interp)
                y_interp = interp1d(t, y_coords, kind='cubic')(t_interp)

                # Kombiner interpolerte punkt
                smooth_points = list(zip(x_interp, y_interp))

                # Teikn smoothe linjer
                pygame.draw.lines(self.screen, self.colors["lines"], False, smooth_points, 2)



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
                        self.filtering.data_points.clear()
                        self.filtering.filtered_points.clear()
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