#include "processing.h"

void processing_task(void *pvParameters) {
  processing_queue_t *queue = (processing_queue_t*)pvParameters;
  while (true) {
    uint8_t recv_buffer[queue->buffer_size];
    while(xQueueReceive(queue->handle, recv_buffer, 0) == pdTRUE) {
      uint16_t frame_buffer[queue->buffer_size / 2];
      for(uint32_t i = 0; i < queue->buffer_size; i++) {
        frame_buffer[i] = (recv_buffer[2*i] << 8) | recv_buffer[2*i + 1];;
      }

      for(uint32_t i = 0; i < (queue->buffer_size / 2); i++) {
        printf("%x", frame_buffer[i]);
      }
      printf("\n");
    }
    vTaskDelay(pdMS_TO_TICKS(10));
  }
}