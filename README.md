Ce TP est à réaliser à deux groupes :
  Groupe 1 : Baptiste LEPIGEON et Tommy HERMOUET.
  Groupe 2 : Abdallah AIDARA et Kamel SEMMAR

## 1. Mise en place

### 1. Installer l'IDE VScode et les dépendances nécessaires

cf. [https://docs.espressif.com/projects/esp-idf/en/v5.4/esp32/get-started/index.html](https://docs.espressif.com/projects/esp-idf/en/v5.4/esp32/get-started/index.html)

Documentation de la board: [http://www.smartcomputerlab.org/](http://www.smartcomputerlab.org/)

### 2. Se connecter sur un point d'accès Wifi
Nous avons ajouter ce bout de code pour ce connecté au partage de connexion (iPhone de Karara) :

  #define WIFI_SSID "iPhone de Karara"
  #define WIFI_PASS "00000000"
  
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

![image](https://github.com/user-attachments/assets/bcfe8e92-cefd-4099-a2df-01343a88c049)

### 3. Envoyer un message mqtt sur le broker test.mosquitto.org au topic tp/alban
(Voir image : Capture)

## 2. LoRa

### 1. Définir des valeurs communes (à faire au tableau)
On a définit ces valeurs pour nos deux groupes :
  #define MQTT_TOPIC "kbssa"
  #define LORA_PASSWORD "kbssa123"

### 2. Communiquer via mqtt des données

Groupe 1: Envoyer un message mqtt donnant les valeurs nécessaires à une reception via LoRa.

Groupe 2. Ecouter les messages mqtt en en déduire les valeurs pour un envoi de données via Lora.

![Capture_envoi_msg](https://github.com/user-attachments/assets/5111768c-bac1-4f57-a8b9-8e413649851b)

### 3. Communiquer via LoRa des données

Groupe 1: Écouter les messages LoRa et les afficher.

Groupe 2. Envoyer un message LoRa contenant des données (potentiellement en utilisant un caoteur.

![Capture_envoi_msg2](https://github.com/user-attachments/assets/33501e86-b312-40f6-afc5-a7b300ca2905)

## 3. Inversion

Après avoir réussi une communication, la carte du groupe 1 et celle du groupe 2 inversent leurs rôles.

![image](https://github.com/user-attachments/assets/220ba524-9348-477d-886d-84e224c3bdaf)

## 5. I2C Basic
À la place de communiquer des messages, je récupère les données de mon capteur gy-49 et l'envoie en m'authentifiant avec le mot de passe.


## 5. Bluetooth (ou BLE)

À la place de communiquer les valeurs venant d'un capteur, continuer la discussion pour ouvrir un canal Bluetooth.


