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
            } else {
                DEBUG_LOGW(TAG, "Ukjent kommando mottatt: %s", data);
            }
        }
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}

// Legg til i app_main:
xTaskCreate(uartCommandTask, "UART Command Task", 2048, NULL, 1, NULL);
