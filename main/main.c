#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_tls.h"
#include "esp_crt_bundle.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include <stdint.h>
#include "esp_http_client.h"
#include "freertos/queue.h"

#include "lora.h"

// The lora packet queue
static QueueHandle_t s_lora_queue_handler;

void lora_receive_task(void *pvParameters) {
  uint8_t *recv_buffer = malloc(256);

  lora_packet_t packet = {
      .payload = recv_buffer,
      .payload_size = 256};

  while (true) {
    lora_receive();

    while (lora_received()) {
      packet.payload_size = lora_receive_packet(packet.payload, 256);
      lora_header_t header;
      header.node_id = *((uint32_t *)packet.payload);
      header.message_id = *(((uint32_t *)packet.payload) + 1);
      ESP_LOGW("main", "Received packet with size %d, node_id: %lu, message_id: %lu", packet.payload_size, header.node_id, header.message_id);
      if (lora_packet_is_duplicate(header)) {
        xQueueSend(s_lora_queue_handler, &packet, portMAX_DELAY);
      }
      lora_add_to_history(header);

      lora_receive();
    }
    vTaskDelay(1);
  }
}

void lora_send_task(void *pvParameters) {
  lora_packet_t packet;

  while (1) {
    BaseType_t status = xQueueReceive(s_lora_queue_handler, &packet, portMAX_DELAY);

    if (status == pdTRUE) {
      lora_send_packet(packet.payload, packet.payload_size);
    }
  }
}

void app_main(void) {
  ESP_ERROR_CHECK(lora_initialize_radio());
  s_lora_queue_handler = xQueueCreate(1, sizeof(lora_packet_t));

  // Create the task that waits for LoRa transmission and then adds the
  // received packets to queue.
  xTaskCreate(lora_receive_task, "lora_receive_task", 8192, NULL, 5, NULL);

  // Create the task which will then send the data we got effectively relaxing it.
  // Has higher priority since we do not want to interrupt that process.
  xTaskCreate(lora_send_task, "lora_send_task", 8192, NULL, 6, NULL);
}