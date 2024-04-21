#ifndef _LORA_H_
#define _LORA_H_

#include "sx1278.h"
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define LORA_FREQ 433e6

// Size of lora duplication queue. Must not be larger than 256.
#define LORA_DUPLICATE_HISTORY_SIZE 100

typedef struct {
  uint8_t *payload;
  size_t payload_size;
} lora_packet_t;

typedef struct {
  uint32_t node_id;
  uint32_t message_id;
} lora_header_t;

uint8_t lora_packet_is_duplicate(lora_header_t header);
void lora_add_to_history(lora_header_t header);

esp_err_t lora_initialize_radio();

#endif