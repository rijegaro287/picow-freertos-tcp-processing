#include "tcp_client.h"


static int32_t tcp_client_create_socket(tcp_client_config_t *config) {
  struct sockaddr_in dest_addr = {
    .sin_len = sizeof(struct sockaddr_in),
    .sin_family = AF_INET,
    .sin_addr.s_addr = inet_addr(config->server_ip),
    .sin_port = htons(config->port)};

  int32_t client_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
  if (client_socket < 0) {
    printf("Failed to create client socket\n");
    return PICO_ERROR_GENERIC;
  }

  int32_t status = connect(client_socket, 
                           (struct sockaddr *)&dest_addr,
                           sizeof(struct sockaddr_in));

  if (status != PICO_OK) {
    printf("Failed to connect to external server\n");
    return PICO_ERROR_GENERIC;
  }

  return client_socket;
}

void tcp_client_task(void *pvParameters) {
  ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

  tcp_client_config_t *config = (tcp_client_config_t *)pvParameters;

  uint32_t buffer_size = uxQueueGetQueueItemSize(config->output_queue);
  uint8_t send_buffer[buffer_size];

  while (true) {
    if(xQueueReceive(config->output_queue, send_buffer, 0) == pdTRUE) {
      int32_t client_socket = tcp_client_create_socket(config);
      if(client_socket >= 0) {
        if(send(client_socket, send_buffer, buffer_size, 0) < 0) {
          printf("Failed to send data\n");
        }
        close(client_socket);
      }
    }
    vTaskDelay(pdMS_TO_TICKS(10));
  }
}
