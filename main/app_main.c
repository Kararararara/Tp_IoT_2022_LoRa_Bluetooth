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

#define WIFI_SSID "iPhone de Karara"
#define WIFI_PASS "00000000"
#define MQTT_BROKER_URI "mqtt://test.mosquitto.org"
#define MQTT_TOPIC "kbssa"
#define LORA_PASSWORD "kbssa123"  // 🔐 Mot de passe LoRa

static const char *TAG = "LoRa_Receiver";

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
static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data) {
    esp_mqtt_event_handle_t event = event_data;
    esp_mqtt_client_handle_t client = event->client;

    switch ((esp_mqtt_event_id_t)event_id) {
    case MQTT_EVENT_CONNECTED:
        ESP_LOGI(TAG, "✅ Connecté à MQTT");
        esp_mqtt_client_subscribe(client, MQTT_TOPIC, 1);
        ESP_LOGI(TAG, "📡 Abonné au topic: %s", MQTT_TOPIC);
        break;

    case MQTT_EVENT_DATA:
        ESP_LOGI(TAG, "📥 Message MQTT reçu !");
        ESP_LOGI(TAG, "TOPIC: %.*s", event->topic_len, event->topic);
        ESP_LOGI(TAG, "DATA: %.*s", event->data_len, event->data);

        // 🔐 Ajouter le mot de passe avant d'envoyer sur LoRa
        char lora_msg[256];
        snprintf(lora_msg, sizeof(lora_msg), "[%s] %.*s", LORA_PASSWORD, event->data_len, event->data);

        // 📡 Envoi sur LoRa
        lora_send_packet((uint8_t *)lora_msg, strlen(lora_msg));
        ESP_LOGI(TAG, "📡 Message envoyé sur LoRa: %s", lora_msg);
        break;

    default:
        break;
    }
}

// 📡 Démarrage MQTT
static void mqtt_app_start(void) {
    esp_mqtt_client_config_t mqtt_cfg = {
        .broker.address.uri = MQTT_BROKER_URI,
    };
    esp_mqtt_client_handle_t client = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL);
    esp_mqtt_client_start(client);
}

// 📡 Réception LoRa
void task_rx(void *pvParameters) {
    ESP_LOGI(TAG, "📡 LoRa Receiver démarré");

    uint8_t buf[256];  
    while(1) {
        lora_receive(); 
        if (lora_received()) {
            int rxLen = lora_receive_packet(buf, sizeof(buf));
            buf[rxLen] = '\0';  // Assurer la fin de chaîne
            ESP_LOGI(TAG, "📥 Message reçu sur LoRa: %s", buf);

            // 🔐 Vérification du mot de passe
            if (strncmp((char *)buf, "[" LORA_PASSWORD "]", strlen(LORA_PASSWORD) + 2) == 0) {
                // 🔽 Supprimer le mot de passe avant d'afficher
                char *message = (char *)buf + strlen(LORA_PASSWORD) + 3;
                ESP_LOGI(TAG, "✅ Message authentifié: %s", message);
            } else {
                ESP_LOGW(TAG, "🚨 Message rejeté (mot de passe incorrect)");
            }
        }
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

// 📡 Fonction principale
void app_main() {
    ESP_ERROR_CHECK(wifi_init_sta());  
    mqtt_app_start();

    if (lora_init() == 0) {
        ESP_LOGE(TAG, "❌ Module LoRa non reconnu !");
        while(1) { vTaskDelay(1); }
    }

    // 🎯 Configuration LoRa
    lora_set_frequency(866e6); 
    lora_enable_crc();
    lora_set_coding_rate(1);
    lora_set_bandwidth(7);
    lora_set_spreading_factor(7);

    // 📡 Lancement du receveur LoRa
    xTaskCreate(&task_rx, "RX", 1024*3, NULL, 5, NULL);
}
