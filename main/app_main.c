#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "esp_system.h"
#include "esp_wifi.h"
#include "nvs_flash.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "protocol_examples_common.h"
#include "esp_log.h"
#include "mqtt_client.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "lora.h"
#include "driver/i2c.h"

#define WIFI_SSID "iPhone de Karara"
#define WIFI_PASS "00000000"
#define MQTT_BROKER_URI "mqtt://test.mosquitto.org"
#define MQTT_TOPIC "kbssa"
#define LORA_PASSWORD "kbssa123"

// Configuration I2C
#define I2C_MASTER_SCL_IO 22
#define I2C_MASTER_SDA_IO 21
#define I2C_MASTER_NUM I2C_NUM_0
#define I2C_MASTER_FREQ_HZ 400000
#define I2C_MASTER_TX_BUF_DISABLE 0
#define I2C_MASTER_RX_BUF_DISABLE 0
#define I2C_MASTER_TIMEOUT_MS 1000

#define GY49_SENSOR_ADDR 0x4A // Adresse I2C du capteur GY-49
#define GY49_DATA_REG 0xAC   // Adresse du registre de données

static const char *TAG = "LoRa_MQTT";
static esp_mqtt_client_handle_t mqtt_client = NULL;

// 📡 Connexion au WiFi
esp_err_t wifi_init_sta(void) {
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    ESP_ERROR_CHECK(example_connect());
    return ESP_OK;
}

// 📡 Gestion MQTT
static void mqtt_app_start(void) {
    esp_mqtt_client_config_t mqtt_cfg = {
        .broker.address.uri = MQTT_BROKER_URI,
    };
    mqtt_client = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_start(mqtt_client);
}

// 📡 Initialisation I2C
static esp_err_t i2c_master_init() {
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = I2C_MASTER_SDA_IO,
        .scl_io_num = I2C_MASTER_SCL_IO,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = I2C_MASTER_FREQ_HZ
    };
    ESP_ERROR_CHECK(i2c_param_config(I2C_MASTER_NUM, &conf));
    return i2c_driver_install(I2C_MASTER_NUM, I2C_MODE_MASTER, I2C_MASTER_RX_BUF_DISABLE, I2C_MASTER_TX_BUF_DISABLE, 0);
}

// 📡 Lecture du capteur GY-49
static uint16_t gy49_read_data() {
    uint8_t data[2] = {0};
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (GY49_SENSOR_ADDR << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, GY49_DATA_REG, true);
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (GY49_SENSOR_ADDR << 1) | I2C_MASTER_READ, true);
    i2c_master_read(cmd, data, sizeof(data), I2C_MASTER_LAST_NACK);
    i2c_master_stop(cmd);
    i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, I2C_MASTER_TIMEOUT_MS / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd);
    return ((uint16_t)data[0] << 8) | data[1];
}

// 📡 Transmission LoRa avec données du capteur
void task_tx(void *pvParameters) {
    ESP_LOGI(TAG, "LORA Transmission démarrée");
    while (1) {
        uint16_t sensor_data = gy49_read_data();
        char message[50];
        snprintf(message, sizeof(message), "[%s] Luminosité: %d", LORA_PASSWORD, sensor_data);
        lora_send_packet((uint8_t *)message, strlen(message));
        ESP_LOGI(TAG, "LORA Envoyé: %s", message);
        vTaskDelay(pdMS_TO_TICKS(5000)); // Envoi toutes les 5 secondes
    }
}

// 📡 Fonction principale
void app_main() {
    ESP_ERROR_CHECK(wifi_init_sta());  
    mqtt_app_start();
    ESP_ERROR_CHECK(i2c_master_init()); // Initialisation I2C
    
    if (lora_init() == 0) {
        ESP_LOGE(TAG, "LORA Erreur: Module non reconnu !");
        while(1) { vTaskDelay(1); }
    }

    // 🎯 Configuration LoRa
    lora_set_frequency(866e6);
    lora_enable_crc();
    lora_set_coding_rate(1);
    lora_set_bandwidth(7);
    lora_set_spreading_factor(7);

    // 📡 Lancement de la tâche de transmission
    xTaskCreate(&task_tx, "TX", 1024*3, NULL, 5, NULL);
}
