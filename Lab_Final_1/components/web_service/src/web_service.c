#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <dirent.h>

#include "esp_system.h"
#include "esp_http_server.h"
#include "esp_log.h"
#include "esp_spiffs.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "web_service.h"
#include "audio_player.h"
#include "shared_lib.h"
#include "task_mqtt.h"
#include "wifi_APSTA.h"

#define TAG "WEB_SERVER"

extern SemaphoreHandle_t spiffs_mutex;
static httpd_handle_t server = NULL;

// Handler de favicon
static esp_err_t favicon_handler(httpd_req_t *req) {
    httpd_resp_send(req, NULL, 0);
    return ESP_OK;
}

static esp_err_t wav_upload_handler(httpd_req_t *req) {
    int i = 1;
    char path[64];
    FILE *f = NULL;

    if (xSemaphoreTake(spiffs_mutex, pdMS_TO_TICKS(5000)) != pdTRUE) {
        ESP_LOGE(TAG, "No se pudo obtener el mutex de SPIFFS");
        return httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "SPIFFS ocupado");
    }

    while (i < 1000) {
        snprintf(path, sizeof(path), "/spiffs/audio_%d.wav", i);
        f = fopen(path, "r");
        if (f == NULL) break;
        fclose(f);
        i++;
    }

    f = fopen(path, "wb");
    if (!f) {
        ESP_LOGE(TAG, "No se pudo abrir %s para escritura", path);
        xSemaphoreGive(spiffs_mutex);
        return httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Fallo al abrir archivo");
    }

    char buf[1024];
    int total = 0, len;
    bool started = false;

    while ((len = httpd_req_recv(req, buf, sizeof(buf))) > 0) {
        int offset = 0;
        if (!started) {
            for (int j = 0; j < len - 4; ++j) {
                if (memcmp(&buf[j], "RIFF", 4) == 0) {
                    offset = j;
                    started = true;
                    break;
                }
            }
            if (!started) continue;  // Aún no se detectó inicio válido
        }

        fwrite(&buf[offset], 1, len - offset, f);
        total += (len - offset);
        offset = 0;
    }

    fclose(f);
    ESP_LOGI(TAG, "Guardado: %s (%d bytes)", path, total);

    struct stat st;
    if (stat(path, &st) == 0) {
        ESP_LOGI(TAG, "Tamaño final en SPIFFS: %ld bytes", st.st_size);
    } else {
        ESP_LOGW(TAG, "No se pudo obtener tamaño de archivo");
    }

    xSemaphoreGive(spiffs_mutex);

    httpd_resp_sendstr(req, "Archivo subido correctamente.");
    return ESP_OK;
}

// Handler de estado del reproductor
static esp_err_t audio_status_handler(httpd_req_t *req) {
    const char *estado_str;
    audio_state_t estado = audio_player_get_state();

    switch (estado) {
        case PLAYER_PLAYING: estado_str = "playing"; break;
        case PLAYER_PAUSED:  estado_str = "paused";  break;
        case PLAYER_STOPPED: estado_str = "stopped"; break;
        default:             estado_str = "unknown"; break;
    }

    httpd_resp_set_type(req, "application/json");
    char buffer[64];
    snprintf(buffer, sizeof(buffer), "{\"estado\":\"%s\"}", estado_str);
    return httpd_resp_sendstr(req, buffer);
}

// Handler de reinicio desde la web
static esp_err_t reset_handler(httpd_req_t *req) {
    httpd_resp_sendstr(req, "Reiniciando dispositivo...");
    esp_restart();
    return ESP_OK;
}

// Handler de comandos de audio via GET
static esp_err_t audio_cmd_handler(httpd_req_t *req) {
    char buf[64];
    size_t len = httpd_req_get_url_query_len(req) + 1;

    if (len > 1 && httpd_req_get_url_query_str(req, buf, len) == ESP_OK) {
        char cmd_str[32];
        if (httpd_query_key_value(buf, "cmd", cmd_str, sizeof(cmd_str)) == ESP_OK) {
            audio_cmd_t cmd;
            if      (strcmp(cmd_str, "play")    == 0) cmd = CMD_PLAY;
            else if (strcmp(cmd_str, "pause")   == 0) cmd = CMD_PAUSE;
            else if (strcmp(cmd_str, "stop")    == 0) cmd = CMD_STOP;
            else if (strcmp(cmd_str, "next")    == 0) cmd = CMD_NEXT;
            else if (strcmp(cmd_str, "prev")    == 0) cmd = CMD_PREV;
            else if (strcmp(cmd_str, "volup")   == 0) cmd = CMD_VOL_UP;
            else if (strcmp(cmd_str, "voldown") == 0) cmd = CMD_VOL_DOWN;
            else return httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Comando invalido");

            audio_player_send_cmd(cmd);
            return httpd_resp_sendstr(req, "OK");
        }
    }
    return httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Faltan parametros");
}

// Handler de formulario POST: guardar WiFi y MQTT URL
esp_err_t guardar_wifi_handler(httpd_req_t *req) {
    ESP_LOGI(TAG, "Handler /guardar_wifi invocado");

    int total_len = req->content_len;
    ESP_LOGI(TAG, "Longitud del contenido recibido: %d bytes", total_len);
    if (total_len >= 512) {
        ESP_LOGE(TAG, "Contenido demasiado largo");
        httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Contenido demasiado largo");
        return ESP_FAIL;
    }

    char buf[256] = {0};
    int received = 0;
    while (received < total_len) {
        int ret = httpd_req_recv(req, buf + received, total_len - received);
        if (ret <= 0) {
            ESP_LOGE(TAG, "Fallo al recibir datos HTTP (%d)", ret);
            return ESP_FAIL;
        }
        received += ret;
    }

    ESP_LOGI(TAG, "Datos crudos recibidos: %s", buf);

    wifi_credentials_t cred = {0};
    char mqtt_url[128] = {0};
    int parsed = sscanf(buf, "ssid=%31[^&]&pass=%63[^&]&mqtt_url=%127s", cred.ssid, cred.password, mqtt_url);
    if (parsed < 3) {
        ESP_LOGE(TAG, "Fallo al parsear datos. Resultado sscanf: %d", parsed);
        httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Formato de datos inválido");
        return ESP_FAIL;
    }

    ESP_LOGI(TAG, "Parseado: ssid='%s', pass='%s', mqtt='%s'", cred.ssid, cred.password, mqtt_url);

    bool ok_ssid = wifi_credentials_validas(&cred);
    bool ok_wifi = wifi_credentials_guardar(&cred);
    esp_err_t err_mqtt = mqtt_guardar_url(mqtt_url);

    ESP_LOGI(TAG, "wifi_credentials_validas: %s", ok_ssid ? "OK" : "FAIL");
    ESP_LOGI(TAG, "wifi_credentials_guardar: %s", ok_wifi ? "OK" : "FAIL");
    ESP_LOGI(TAG, "mqtt_guardar_url: %s", esp_err_to_name(err_mqtt));

    if (ok_ssid && ok_wifi && err_mqtt == ESP_OK) {
        // Notificar a la FSM que hay nuevas credenciales
        wifi_fsm_notificar_nuevas_credenciales();

        httpd_resp_set_type(req, "text/html");
        httpd_resp_sendstr(req,
            "<html><head><meta http-equiv=\"refresh\" content=\"3;url=/\"></head>"
            "<body><h1>✅ Credenciales guardadas</h1>"
            "<p>Serás redirigido al reproductor automáticamente...</p>"
            "</body></html>");
    } else {
        ESP_LOGE(TAG, "Fallo en guardar alguna de las credenciales");
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Error al guardar datos");
    }

    return ESP_OK;
}

// HTML reducido desde SPIFFS
static esp_err_t root_get_handler(httpd_req_t *req) {
    FILE* f = fopen("/spiffs/index.html", "r");
    if (!f) {
        ESP_LOGE(TAG, "No se pudo abrir /spiffs/index.html");
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Archivo no encontrado");
        return ESP_FAIL;
    }

    httpd_resp_set_type(req, "text/html");

    char buffer[512];
    size_t read_bytes;

    while ((read_bytes = fread(buffer, 1, sizeof(buffer), f)) > 0) {
        esp_err_t err = httpd_resp_send_chunk(req, buffer, read_bytes);
        if (err != ESP_OK) {
            fclose(f);
            return err;
        }
    }

    fclose(f);
    return httpd_resp_send_chunk(req, NULL, 0);
}

static esp_err_t borrar_credenciales_handler(httpd_req_t *req) {
    ESP_LOGW(TAG, "Petición para borrar credenciales recibida");

    bool ok_wifi = wifi_credentials_borrar();  // Asumimos que esta función ya existe
    esp_err_t err_mqtt = mqtt_borrar_url();    // Debés implementar esto si no existe

    if (ok_wifi && err_mqtt == ESP_OK) {
        ESP_LOGI(TAG, "Credenciales borradas correctamente");

        httpd_resp_set_type(req, "text/html");
        httpd_resp_sendstr(req,
            "<html><head><meta http-equiv=\"refresh\" content=\"3;url=/\"></head>"
            "<body><h1>🗑️ Credenciales eliminadas</h1>"
            "<p>El dispositivo se reiniciará automáticamente...</p></body></html>");

        vTaskDelay(pdMS_TO_TICKS(500));
        esp_restart();
        return ESP_OK;
    }

    ESP_LOGE(TAG, "Fallo al borrar credenciales");
    return httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "No se pudo borrar");
}

static esp_err_t reload_playlist_handler(httpd_req_t *req) {
    load_playlist_from_spiffs();
    httpd_resp_sendstr(req, "Playlist recargada.");
    return ESP_OK;
}

static esp_err_t borrar_handler(httpd_req_t *req) {
    char archivo[64];
    if (httpd_req_get_url_query_len(req) < 1)
        return httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Sin parámetro");

    char query[128];
    httpd_req_get_url_query_str(req, query, sizeof(query));
    if (httpd_query_key_value(query, "archivo", archivo, sizeof(archivo)) != ESP_OK)
        return httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Falta parámetro archivo");

    char path[80];
    snprintf(path, sizeof(path), "/spiffs/%s", archivo);

    if (unlink(path) == 0) {
        ESP_LOGI(TAG, "Archivo borrado por web: %s", archivo);
        httpd_resp_sendstr(req, "Archivo eliminado correctamente.");
    } else {
        ESP_LOGE(TAG, "Error al borrar: %s", archivo);
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Error al eliminar archivo.");
    }
    load_playlist_from_spiffs();// Cargamos nuevamente playlist
    vTaskDelay(pdMS_TO_TICKS(100));
    return ESP_OK;
}

static esp_err_t descargar_handler(httpd_req_t *req) {
    char archivo[64];
    if (httpd_req_get_url_query_len(req) < 1) {
        return httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Falta parámetro");
    }

    char query[128];
    httpd_req_get_url_query_str(req, query, sizeof(query));
    if (httpd_query_key_value(query, "archivo", archivo, sizeof(archivo)) != ESP_OK) {
        return httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Falta nombre de archivo");
    }

    char path[96];
    snprintf(path, sizeof(path), "/spiffs/%s", archivo);
    FILE *f = fopen(path, "rb");
    if (!f) return httpd_resp_send_err(req, HTTPD_404_NOT_FOUND, "Archivo no encontrado");

    httpd_resp_set_type(req, "audio/wav");
    char hdr[128];
    snprintf(hdr, sizeof(hdr), "attachment; filename=\"%s\"", archivo);
    httpd_resp_set_hdr(req, "Content-Disposition", hdr);

    char buffer[1024];
    size_t len;
    while ((len = fread(buffer, 1, sizeof(buffer), f)) > 0) {
        httpd_resp_send_chunk(req, buffer, len);
    }
    fclose(f);
    httpd_resp_send_chunk(req, NULL, 0);
    return ESP_OK;
}

static esp_err_t reproducir_audio_handler(httpd_req_t *req) {
    char archivo[64] = {0};

    // Extrae el parámetro de la URL
    char *query = strchr(req->uri, '?');
    if (query && strstr(query, "archivo=")) {
        char *nombre = strstr(query, "archivo=") + strlen("archivo=");
        strncpy(archivo, nombre, sizeof(archivo) - 1);
    } else {
        httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Falta parámetro 'archivo'");
        return ESP_FAIL;
    }

    // Decodifica caracteres especiales si vinieron por URL (por ejemplo, %20)
    httpd_query_key_value(query + 1, "archivo", archivo, sizeof(archivo));

    // Detener cualquier reproducción anterior antes de cambiar el track
    audio_player_send_cmd(CMD_STOP);
    vTaskDelay(pdMS_TO_TICKS(150));  // Breve espera para asegurarse que se detuvo

    if (audio_player_reproducir_archivo(archivo)) {
        httpd_resp_sendstr(req, "OK");
        return ESP_OK;
    } else {
        httpd_resp_send_err(req, HTTPD_404_NOT_FOUND, "Archivo no encontrado en playlist");
        return ESP_FAIL;
    }
}

static esp_err_t listar_audios_json_handler(httpd_req_t *req) {
    DIR *dir = opendir("/spiffs");
    if (!dir) {
        return httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "No se pudo abrir SPIFFS");
    }

    httpd_resp_set_type(req, "application/json");
    httpd_resp_sendstr_chunk(req, "[");

    struct dirent *entry;
    bool primero = true;
    while ((entry = readdir(dir)) != NULL) {
        if (strstr(entry->d_name, ".wav")) {
            if (!primero) httpd_resp_sendstr_chunk(req, ",");
            httpd_resp_sendstr_chunk(req, "\"");
            httpd_resp_sendstr_chunk(req, entry->d_name);
            httpd_resp_sendstr_chunk(req, "\"");
            primero = false;
        }
    }

    closedir(dir);
    httpd_resp_sendstr_chunk(req, "]");
    httpd_resp_sendstr_chunk(req, NULL);
    return ESP_OK;
}

// Inicializar WebServer
void web_service_inicializar(void) {
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.max_uri_handlers = 15;
    config.recv_wait_timeout = 10;
    config.lru_purge_enable = true;
    config.stack_size = 8192;

    ESP_LOGI(TAG, "Iniciando WebServer");
    if (httpd_start(&server, &config) == ESP_OK) {
        const httpd_uri_t root_uri = {
            .uri       = "/",
            .method    = HTTP_GET,
            .handler   = root_get_handler,
            .user_ctx  = NULL
        };

        const httpd_uri_t guardar_wifi_uri = {
            .uri       = "/guardar_wifi",
            .method    = HTTP_POST,
            .handler   = guardar_wifi_handler,
            .user_ctx  = NULL
        };

        const httpd_uri_t status_uri = {
            .uri       = "/status",
            .method    = HTTP_GET,
            .handler   = audio_status_handler,
            .user_ctx  = NULL
        };

        const httpd_uri_t cmd_uri = {
            .uri       = "/cmd",
            .method    = HTTP_GET,
            .handler   = audio_cmd_handler,
            .user_ctx  = NULL
        };

        const httpd_uri_t favicon_uri = {
            .uri       = "/favicon.ico",
            .method    = HTTP_GET,
            .handler   = favicon_handler,
            .user_ctx  = NULL
        };
        const httpd_uri_t reset_uri = {
            .uri       = "/reset",
            .method    = HTTP_GET,
            .handler   = reset_handler,
            .user_ctx  = NULL
        };
        const httpd_uri_t upload_uri = {
            .uri = "/upload",
            .method = HTTP_POST,
            .handler = wav_upload_handler,
            .user_ctx = NULL
        };
        const httpd_uri_t borrar_uri = {
            .uri = "/borrar",
            .method = HTTP_GET,
            .handler = borrar_handler,
            .user_ctx = NULL
        };
        const httpd_uri_t borrar_cred_uri = {
        .uri = "/borrar_credenciales",
        .method = HTTP_GET,
        .handler = borrar_credenciales_handler,
        .user_ctx = NULL
        };
        const httpd_uri_t reload_playlist_uri = {
            .uri = "/reload_playlist",
            .method = HTTP_GET,
            .handler = reload_playlist_handler,
            .user_ctx = NULL
        };
        const httpd_uri_t descargar_uri = {
            .uri = "/descargar",
            .method = HTTP_GET,
            .handler = descargar_handler,
            .user_ctx = NULL
        };
        const httpd_uri_t listar_audios_uri = {
            .uri = "/listar_audios",
            .method = HTTP_GET,
            .handler = listar_audios_json_handler,
            .user_ctx = NULL
        };
        httpd_uri_t uri_reproducir = {
            .uri      = "/reproducir",
            .method   = HTTP_GET,
            .handler  = reproducir_audio_handler,
            .user_ctx = NULL
        };


        httpd_register_uri_handler(server, &root_uri);
        httpd_register_uri_handler(server, &guardar_wifi_uri);
        httpd_register_uri_handler(server, &status_uri);
        httpd_register_uri_handler(server, &cmd_uri);
        httpd_register_uri_handler(server, &favicon_uri);
        httpd_register_uri_handler(server, &reset_uri);
        httpd_register_uri_handler(server, &upload_uri);
        httpd_register_uri_handler(server, &borrar_uri);
        httpd_register_uri_handler(server, &borrar_cred_uri);
        httpd_register_uri_handler(server, &reload_playlist_uri);
        httpd_register_uri_handler(server, &descargar_uri);
        httpd_register_uri_handler(server, &listar_audios_uri);
        httpd_register_uri_handler(server, &uri_reproducir);
    }
}