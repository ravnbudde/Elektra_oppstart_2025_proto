#include <stdio.h>
#include <string.h>
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

// Definer pinnar for ultralydsensoren, motoren og debug-LED
#define MOTOR_CONTROL_PIN GPIO_NUM_11
#define TRIG_PIN GPIO_NUM_6 //Ornage, GND blue
#define ECHO_PIN GPIO_NUM_7 //green, VCC yellow
#define PULSE_PIN GPIO_NUM_2
#define CENTER_SWITCH_PIN GPIO_NUM_1 // brown
#define DEBUG_LED_PIN GPIO_NUM_20
#define DEGREES_PER_TICK 0.3515625*8

#define UART_NUM UART_NUM_1
#define BUF_SIZE 1024

// Globale variablar
volatile float motor_position = 0.0; // Motorens nåværende posisjon i grader
volatile bool centered = false; // Indikerer om senter er detektert
volatile bool motor_state = false; // Holder styr på motorens status (på/av)
SemaphoreHandle_t motorSemaphore;

// Funksjon for å toggle debug-LED
void toggle_debug_led() {
    int current_level = gpio_get_level(DEBUG_LED_PIN);
    gpio_set_level(DEBUG_LED_PIN, !current_level);
}

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

        // Toggle debug-LED for hver måling
        toggle_debug_led();

        vTaskDelay(100 / portTICK_PERIOD_MS);
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
    gpio_reset_pin(DEBUG_LED_PIN);

    gpio_set_direction(TRIG_PIN, GPIO_MODE_OUTPUT);
    gpio_set_direction(ECHO_PIN, GPIO_MODE_INPUT);
    gpio_set_direction(MOTOR_CONTROL_PIN, GPIO_MODE_OUTPUT);
    gpio_set_direction(PULSE_PIN, GPIO_MODE_INPUT);
    gpio_set_direction(CENTER_SWITCH_PIN, GPIO_MODE_INPUT);
    gpio_set_direction(DEBUG_LED_PIN, GPIO_MODE_OUTPUT);

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
}
