#include "processing.h"

void processing_task(void *pvParameters) {
	ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

  queue_handles_t *handles = (queue_handles_t*)pvParameters;

  uint32_t buffer_size = uxQueueGetQueueItemSize(handles->input_queue);
  uint8_t recv_buffer[buffer_size];

  while(true) {
    if(xQueueReceive(handles->input_queue, recv_buffer, 0) == pdTRUE) {
      uint16_t frame_buffer[buffer_size / 2];
      for(uint32_t i = 0; i < buffer_size; i++) {
        frame_buffer[i] = (recv_buffer[2*i] << 8) | recv_buffer[(2*i) + 1];;
      }

      /* Process data... */

      uint8_t result_buffer[buffer_size];
      for (uint32_t i = 0; i < (buffer_size / 2); i++) {
        result_buffer[2*i] = (frame_buffer[i] >> 8) & 0xFF;
        result_buffer[(2*i) + 1] = frame_buffer[i] & 0xFF;
      }

      xQueueSend(handles->output_queue, result_buffer, 0);
    }
    vTaskDelay(pdMS_TO_TICKS(10));
  }
}