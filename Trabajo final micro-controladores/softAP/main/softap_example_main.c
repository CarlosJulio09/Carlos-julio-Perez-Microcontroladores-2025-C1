#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_mac.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_netif.h"
#include "esp_http_server.h"
#include "lwip/err.h"
#include "lwip/sys.h"

#define EXAMPLE_ESP_WIFI_SSID      "ESP32_Timer555"
#define EXAMPLE_ESP_WIFI_PASS      "12345678"
#define EXAMPLE_ESP_WIFI_CHANNEL   1
#define EXAMPLE_MAX_STA_CONN       4

static const char *TAG = "wifi softAP";

// ======= Código para iniciar SoftAP =======
static void wifi_event_handler(void* arg, esp_event_base_t event_base,
                                int32_t event_id, void* event_data) {
    if (event_id == WIFI_EVENT_AP_STACONNECTED) {
        wifi_event_ap_staconnected_t* event = (wifi_event_ap_staconnected_t*) event_data;
        ESP_LOGI(TAG, "station "MACSTR" join, AID=%d",
                 MAC2STR(event->mac), event->aid);
    } else if (event_id == WIFI_EVENT_AP_STADISCONNECTED) {
        wifi_event_ap_stadisconnected_t* event = (wifi_event_ap_stadisconnected_t*) event_data;
        ESP_LOGI(TAG, "station "MACSTR" leave, AID=%d, reason=%d",
                 MAC2STR(event->mac), event->aid, event->reason);
    }
}

void wifi_init_softap(void) {
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_ap();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                        ESP_EVENT_ANY_ID,
                                                        &wifi_event_handler,
                                                        NULL,
                                                        NULL));

    wifi_config_t wifi_config = {
        .ap = {
            .ssid = EXAMPLE_ESP_WIFI_SSID,
            .ssid_len = strlen(EXAMPLE_ESP_WIFI_SSID),
            .channel = EXAMPLE_ESP_WIFI_CHANNEL,
            .password = EXAMPLE_ESP_WIFI_PASS,
            .max_connection = EXAMPLE_MAX_STA_CONN,
            .authmode = WIFI_AUTH_WPA2_PSK,
            .pmf_cfg = {
                .required = true,
            },
        },
    };
    if (strlen(EXAMPLE_ESP_WIFI_PASS) == 0) {
        wifi_config.ap.authmode = WIFI_AUTH_OPEN;
    }

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG, "SoftAP iniciado. SSID:%s Password:%s Channel:%d",
             EXAMPLE_ESP_WIFI_SSID, EXAMPLE_ESP_WIFI_PASS, EXAMPLE_ESP_WIFI_CHANNEL);
}

// ======= Código para el servidor web =======
static esp_err_t page_get_handler(httpd_req_t *req) {
    const char* html = 
    "<!DOCTYPE html><html lang='es'><head><meta charset='UTF-8'><meta name='viewport' content='width=device-width, initial-scale=1.0'>"
    "<title>Simulador Timer 555</title>"
    "<style>"
    "body { font-family: Arial, sans-serif; background-color: #0b1e35; color: white; margin: 0; padding: 20px; }"
    ".container { max-width: 600px; margin: auto; }"
    ".section { background: #1e2d4f; padding: 20px; border-radius: 10px; margin-bottom: 20px; box-shadow: 0 0 10px rgba(0,0,0,0.5); }"
    "h2 { text-align: center; margin-bottom: 10px; }"
    "label { display: block; margin-top: 10px; }"
    "input, select { width: 100%; padding: 10px; margin-top: 5px; border-radius: 5px; border: none; }"
    ".btn { width: 100%; margin-top: 15px; padding: 10px; background-color: #007bff; border: none; border-radius: 5px; color: white; font-size: 16px; cursor: pointer; }"
    ".btn:hover { background-color: #0056b3; }"
    "</style>"
    "</head><body><div class='container'>"

    "<div class='section'><h2>Tipo Monoestable</h2>"
    "<label>Valor Resistencia (R1):</label><input type='number' id='r1_mono'>"
    "<select id='r1_unit_mono'><option value='1000'>kΩ</option><option value='1'>Ω</option></select>"
    "<label>Valor Capacitancia (C):</label><input type='number' id='c_mono'>"
    "<select id='c_unit_mono'><option value='1e-9'>nF</option><option value='1e-6'>μF</option></select>"
    "<button class='btn' onclick='calcularMonoestable()'>RESULTADO</button>"
    "<p id='resultado_mono'>Tiempo del Pulso (T): 0 segundos</p>"
    "</div>"

    "<div class='section'><h2>Tipo Astable</h2>"
    "<label>Valor Resistencia (R1):</label><input type='number' id='r1_astable'>"
    "<label>Valor Resistencia (R2):</label><input type='number' id='r2_astable'>"
    "<select id='r_unit_astable'><option value='1000'>kΩ</option><option value='1'>Ω</option></select>"
    "<label>Valor Capacitancia (C):</label><input type='number' id='c_astable'>"
    "<select id='c_unit_astable'><option value='1e-9'>nF</option><option value='1e-6'>μF</option></select>"
    "<button class='btn' onclick='calcularAstable()'>RESULTADO</button>"
    "<p id='resultado_astable'>Frecuencia (F): 0 Hz<br>Ciclo de Trabajo (D): 0 %</p>"
    "</div>"

    "</div>"

    "<script>"
    "function calcularMonoestable() {"
    "var r1 = parseFloat(document.getElementById('r1_mono').value) * parseFloat(document.getElementById('r1_unit_mono').value);"
    "var c = parseFloat(document.getElementById('c_mono').value) * parseFloat(document.getElementById('c_unit_mono').value);"
    "var t = 1.1 * r1 * c;"
    "document.getElementById('resultado_mono').innerText = 'Tiempo del Pulso (T): ' + t.toFixed(6) + ' segundos';"
    "}"
    "function calcularAstable() {"
    "var r1 = parseFloat(document.getElementById('r1_astable').value) * parseFloat(document.getElementById('r_unit_astable').value);"
    "var r2 = parseFloat(document.getElementById('r2_astable').value) * parseFloat(document.getElementById('r_unit_astable').value);"
    "var c = parseFloat(document.getElementById('c_astable').value) * parseFloat(document.getElementById('c_unit_astable').value);"
    "var frecuencia = 1.44 / ((r1 + 2 * r2) * c);"
    "var duty_cycle = ((r1 + r2) / (r1 + 2 * r2)) * 100;"
    "document.getElementById('resultado_astable').innerHTML = 'Frecuencia (F): ' + frecuencia.toFixed(2) + ' Hz<br>Ciclo de Trabajo (D): ' + duty_cycle.toFixed(2) + ' %';"
    "}"
    "</script>"

    "</body></html>";

    httpd_resp_send(req, html, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

static httpd_handle_t start_webserver(void) {
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    httpd_handle_t server = NULL;
    if (httpd_start(&server, &config) == ESP_OK) {
        httpd_uri_t page_uri = {
            .uri       = "/",
            .method    = HTTP_GET,
            .handler   = page_get_handler,
            .user_ctx  = NULL
        };
        httpd_register_uri_handler(server, &page_uri);
    }
    return server;
}

// ======= Función Principal =======
void app_main(void) {
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    wifi_init_softap();
    start_webserver();
}
