Ce TP est à réaliser à deux groupes :
  - Groupe 1 : Baptiste LEPIGEON et Tommy HERMOUET.
  - Groupe 2 : Abdallah AIDARA et Kamel SEMMAR

## 1. Mise en place

### 1. Installer l'IDE VScode et les dépendances nécessaires

cf. [https://docs.espressif.com/projects/esp-idf/en/v5.4/esp32/get-started/index.html](https://docs.espressif.com/projects/esp-idf/en/v5.4/esp32/get-started/index.html)

Documentation de la board: [http://www.smartcomputerlab.org/](http://www.smartcomputerlab.org/)

ESP-IDF (Espressif IoT Development Framework) est le SDK officiel permettant de programmer les ESP32. Il fournit des bibliothèques pour :

Le WiFi (mode station et access point),
Le protocole MQTT,
Le support de LoRa, I2C, et Bluetooth,
L’interface avec des capteurs et périphériques.

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

- MQTT ( (Message Queuing Telemetry Transport) est un protocole de messagerie léger et efficace pour l'IoT.
Dans ce TP, un 'broker' (serveur central) MQTT est utilisé : test.mosquitto.org (c'est un service public pour tester MQTT).
Les messages sont envoyés dans des 'topics', qui désigne un canal de communication entre appareils MQTT (plus précisément, c'est une chaîne de caractère que l'on désigne comme 'nom du canal'.
MQTT est utilisé pour transmettre des données entre les deux groupes avant de passer sur LoRa.

MQTT 5.0 apporte : 
- Des topics structurés,
- Un système de QoS (Quality of Service),
- Un support amélioré de la sécurité et des options avancées de publication/souscription.

## 2. LoRa
LoRa est une technologie radio LPWAN (Low Power Wide Area Network) permettant la transmission de données à longue portée avec une faible consommation.

Dans le TP, nous devons utiliser LoRa pour envoyer des données sans fil.
Le Groupe 1 écoute les messages LoRa et les affiche.
Le Groupe 2 envoie des messages LoRa via la board ESP32.

Fréquence : 868 MHz.
Sf = 7
sb : 125k

Pour faire fonctionner LoRa dans ESP-IDF, il faut installer dans idf_component.yml le composant [esp-idf-sx127x](https://github.com/nopnop2002/esp-idf-sx127x) puis include lora.h au début du code.

### 1. Définir des valeurs communes (à faire au tableau)
On a définit ces valeurs pour nos deux groupes :
  #define MQTT_TOPIC "kbssa"
  #define LORA_PASSWORD "kbssa123"

### 2. Communiquer via mqtt des données

Groupe 1: Envoyer un message mqtt donnant les valeurs nécessaires à une reception via LoRa.

Groupe 2. Ecouter les messages mqtt en en déduire les valeurs pour un envoi de données via Lora.

![Capture_envoi_msg](https://github.com/user-attachments/assets/5111768c-bac1-4f57-a8b9-8e413649851b)

On peut voir que les parties du code réception et transmission fonctionnenent bien.

### 3. Communiquer via LoRa des données

Groupe 1: Écouter les messages LoRa et les afficher.

Groupe 2 : Envoyer un message LoRa contenant des données (potentiellement en utilisant un capteur).

![Capture_envoi_msg2](https://github.com/user-attachments/assets/33501e86-b312-40f6-afc5-a7b300ca2905)

Ici la transmission/réception LoRa fonctionne bien.

## 4. Inversion

Après avoir réussi une communication, la carte du groupe 1 et celle du groupe 2 inversent leurs rôles.

![image](https://github.com/user-attachments/assets/220ba524-9348-477d-886d-84e224c3bdaf)

## 5. I2C Basic
À la place de communiquer des messages, je récupère les données de mon capteur gy-49 et l'envoie en m'authentifiant avec le mot de passe.


## 6. Bluetooth (ou BLE)

À la place de communiquer les valeurs venant d'un capteur, continuer la discussion pour ouvrir un canal Bluetooth.


