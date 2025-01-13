#include "processing.h"

void processing_task(void *pvParameters) {
	ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

  xQueueHandle *queue = (xQueueHandle*)pvParameters;
  uint32_t buffer_size = uxQueueGetQueueItemSize(*queue);
  while (true) {
    uint8_t recv_buffer[buffer_size];
    while(xQueueReceive(*queue, recv_buffer, 0) == pdTRUE) {
      uint16_t frame_buffer[buffer_size / 2];
      for(uint32_t i = 0; i < buffer_size; i++) {
        frame_buffer[i] = (recv_buffer[2*i] << 8) | recv_buffer[2*i + 1];;
      }

      for(uint32_t i = 0; i < (buffer_size / 2); i++) {
        printf("%x", frame_buffer[i]);
      }
      printf("\n");
    }
    vTaskDelay(pdMS_TO_TICKS(10));
  }
}