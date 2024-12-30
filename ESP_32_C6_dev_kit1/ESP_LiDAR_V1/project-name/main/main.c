#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "driver/gpio.h"
#include "driver/uart.h"
#include "esp_log.h"
#include "esp_timer.h"

static const char *TAG = "LIDAR";

// Aktiver eller deaktiver debug mode
#define DEBUG_MODE // Kommenter ut for å deaktivere debug logging

#ifdef DEBUG_MODE
    #define DEBUG_LOGI(tag, format, ...) ESP_LOGI(tag, "[INFO]: " format, ##__VA_ARGS__)
    #define DEBUG_LOGW(tag, format, ...) ESP_LOGW(tag, "[WARN]: " format, ##__VA_ARGS__)
    #define DEBUG_LOGE(tag, format, ...) ESP_LOGE(tag, "[ERROR]: " format, ##__VA_ARGS__)
#else
    #define DEBUG_LOGI(tag, format, ...) // Ingen logging
    #define DEBUG_LOGW(tag, format, ...) // Ingen logging
    #define DEBUG_LOGE(tag, format, ...) // Ingen logging
#endif

// Definer pinnar for ultralydsensoren og motoren
#define MOTOR_CONTROL_PIN GPIO_NUM_11
#define TRIG_PIN GPIO_NUM_6 //Ornage, GND blue
#define ECHO_PIN GPIO_NUM_7 //green, VCC yellow
#define PULSE_PIN GPIO_NUM_2
#define CENTER_SWITCH_PIN GPIO_NUM_0 // brown
#define DEGREES_PER_TICK 0.3515625*8

#define UART_NUM UART_NUM_1
#define BUF_SIZE 1024

// Globale variablar
volatile float motor_position = 0.0; // Motorens nåværende posisjon i grader
volatile bool centered = false; // Indikerer om senter er detektert
volatile bool motor_state = false; // Holder styr på motorens status (på/av)

SemaphoreHandle_t motorSemaphore;

// Funksjon for å måle avstand med ultralydsensoren
float measureDistance() {
    gpio_set_level(TRIG_PIN, 0);
    vTaskDelay(2 / portTICK_PERIOD_MS);
    gpio_set_level(TRIG_PIN, 1);
    vTaskDelay(10 / portTICK_PERIOD_MS);
    gpio_set_level(TRIG_PIN, 0);

    // Mål echo-tid
    int64_t start_time = esp_timer_get_time();
    while (gpio_get_level(ECHO_PIN) == 0) {
        if ((esp_timer_get_time() - start_time) > 20000) {
            return -1; // Timeout
        }
    }

    start_time = esp_timer_get_time();
    while (gpio_get_level(ECHO_PIN) == 1) {
        if ((esp_timer_get_time() - start_time) > 20000) {
            return -1; // Timeout
        }
    }

    int64_t duration = esp_timer_get_time() - start_time;

    // Beregn avstand i cm
    float distance = (duration * 0.034) / 2;
    return distance;
}

// Avbrotsrutinene
void IRAM_ATTR onPulse() {
    // Motoren flyttar seg x grader per puls
    motor_position += DEGREES_PER_TICK;
    if (motor_position >= 360.0) {
        motor_position -= 360.0;
    }
}

void IRAM_ATTR onCenterSwitch() {
    centered = true;
    motor_position = 0.0;
}

// Funksjon for å kalibrere motoren
void calibrate_motor() {
    DEBUG_LOGI(TAG, "Kalibrerer motor...");

    // Start motoren
    gpio_set_level(MOTOR_CONTROL_PIN, 0);
    vTaskDelay(50 / portTICK_PERIOD_MS);
    gpio_set_level(MOTOR_CONTROL_PIN, 1);

    while (!centered) {
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }

    // Stop motoren
    gpio_set_level(MOTOR_CONTROL_PIN, 0);
    vTaskDelay(50 / portTICK_PERIOD_MS);
    gpio_set_level(MOTOR_CONTROL_PIN, 1);

    DEBUG_LOGI(TAG, "Kalibrering fullført. Motor posisjon satt til 0.");
    motor_position = 0.0;
}

// Funksjon for å starte motoren
void start_motor() {
    DEBUG_LOGI(TAG, "Starter motor...");

    if (!motor_state) {
        gpio_set_level(MOTOR_CONTROL_PIN, 0);
        vTaskDelay(50 / portTICK_PERIOD_MS);
        gpio_set_level(MOTOR_CONTROL_PIN, 1);

        bool motor_running = false;
        for (int i = 0; i < 10; i++) {
            if (gpio_get_level(PULSE_PIN)) {
                motor_running = true;
                break;
            }
            vTaskDelay(10 / portTICK_PERIOD_MS);
        }

        if (motor_running) {
            DEBUG_LOGI(TAG, "Motoren starta korrekt.");
            motor_state = true;
        } else {
            DEBUG_LOGW(TAG, "Motoren starta ikkje.");
        }
    } else {
        DEBUG_LOGW(TAG, "Motoren er allerede på.");
    }
}

// Funksjon for å stoppe motoren
void stop_motor() {
    DEBUG_LOGI(TAG, "Stoppar motor...");

    if (motor_state) {
        gpio_set_level(MOTOR_CONTROL_PIN, 0);
        vTaskDelay(50 / portTICK_PERIOD_MS);
        gpio_set_level(MOTOR_CONTROL_PIN, 1);

        bool motor_stopped = true;
        for (int i = 0; i < 10; i++) {
            if (gpio_get_level(PULSE_PIN)) {
                motor_stopped = false;
                break;
            }
            vTaskDelay(10 / portTICK_PERIOD_MS);
        }

        if (motor_stopped) {
            DEBUG_LOGI(TAG, "Motoren stoppa korrekt.");
            motor_state = false;
        } else {
            DEBUG_LOGW(TAG, "Motoren stoppa ikkje.");
        }
    } else {
        DEBUG_LOGW(TAG, "Motoren er allerede av.");
    }
}

// Oppgave for avstandsmåling
void ultrasonicTask(void *pvParameters) {
    while (1) {
        float distance = measureDistance();
        if (distance < 0) {
            DEBUG_LOGW(TAG, "Timeout during distance measurement");
            continue;
        }

        xSemaphoreTake(motorSemaphore, portMAX_DELAY);
        float position = motor_position;
        xSemaphoreGive(motorSemaphore);

        DEBUG_LOGI(TAG, "Angle: %.2f, Distance: %.2f", position, distance);

        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}
// Funksjon for å lytte etter kommandoer over UART
void uartCommandTask(void *pvParameters) {
    uint8_t data[BUF_SIZE];
    while (1) {
        int length = uart_read_bytes(UART_NUM, data, BUF_SIZE, 20 / portTICK_RATE_MS);
        if (length > 0) {
            data[length] = '\0'; // Null-terminer stringen
            if (strstr((char *)data, "START") != NULL) {
                DEBUG_LOGI(TAG, "Mottok kommando: START");
                start_motor();
            } else if (strstr((char *)data, "STOP") != NULL) {
                DEBUG_LOGI(TAG, "Mottok kommando: STOP");
                stop_motor();
            } else if (strcmp((char *)data, "\n") == 0) {
                if (motor_state) {
                    DEBUG_LOGI(TAG, "Mottok kommando: STOP via Enter");
                    stop_motor();
                } else {
                    DEBUG_LOGI(TAG, "Mottok kommando: START via Enter");
                    start_motor();
                }
            } else {
                DEBUG_LOGW(TAG, "Ukjent kommando mottatt: %s", data);
            }
        }
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}

void app_main(void) {
    // UART-konfigurasjon
    const uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
    };
    uart_param_config(UART_NUM, &uart_config);
    uart_driver_install(UART_NUM, BUF_SIZE, BUF_SIZE, 0, NULL, 0);

    gpio_reset_pin(TRIG_PIN);
    gpio_reset_pin(ECHO_PIN);
    gpio_reset_pin(MOTOR_CONTROL_PIN);
    gpio_reset_pin(PULSE_PIN);
    gpio_reset_pin(CENTER_SWITCH_PIN);

    gpio_set_direction(TRIG_PIN, GPIO_MODE_OUTPUT);
    gpio_set_direction(ECHO_PIN, GPIO_MODE_INPUT);
    gpio_set_direction(MOTOR_CONTROL_PIN, GPIO_MODE_OUTPUT);
    gpio_set_direction(PULSE_PIN, GPIO_MODE_INPUT);
    gpio_set_direction(CENTER_SWITCH_PIN, GPIO_MODE_INPUT);

    gpio_pullup_en(PULSE_PIN);
    gpio_pullup_en(CENTER_SWITCH_PIN);

    gpio_set_intr_type(PULSE_PIN, GPIO_INTR_NEGEDGE);
    gpio_set_intr_type(CENTER_SWITCH_PIN, GPIO_INTR_NEGEDGE);

    DEBUG_LOGI(TAG, "System initialized");

    motorSemaphore = xSemaphoreCreateMutex();

    gpio_install_isr_service(0);
    gpio_isr_handler_add(PULSE_PIN, onPulse, NULL);
    gpio_isr_handler_add(CENTER_SWITCH_PIN, onCenterSwitch, NULL);

    calibrate_motor();

    start_motor();

    xTaskCreate(ultrasonicTask, "Ultrasonic Task", 2048, NULL, 1, NULL);
    xTaskCreate(uartCommandTask, "UART com Task", 2048, NULL, 1, NULL);
}

/* Python script for extracting and printing INFO logs from UART
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
scaling_factor = 10

# Data og innstillinger
data_points = deque(maxlen=500)  # Lagrer opp til 500 punkt
filtered_points = {}  # Lagrer filtrerte punkt
distance_limit = 100  # Maksimum avstand i cm
alpha = 0.7  # Filterparameter
lock = threading.Lock()  # For å sikre trådtrygg dataoppdatering
show_points = True  # Bytt mellom punkter og linjer

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
                            rad_angle = math.radians(angle + 90)
                            x = center_x + distance * math.cos(rad_angle) * scaling_factor  # Skaler for skjerm
                            y = center_y - distance * math.sin(rad_angle) * scaling_factor
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
    screen.blit(font.render("Reset", True, WHITE), (20, 20))
    screen.blit(font.render("Toggle View", True, WHITE), (130, 20))

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

    pygame.display.flip()

# Lukk programmet
ser.close()
pygame.quit()

*/
