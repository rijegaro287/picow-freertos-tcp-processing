#pragma once

#include "stdio.h"
#include "pico/stdlib.h"

#include "lwip/ip4_addr.h"
#include "lwip/netif.h"
#include <lwip/sockets.h>

#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"

typedef struct _tcp_client_config {
  const uint8_t *server_ip;
  const uint16_t port;
  xQueueHandle output_queue;
  xSemaphoreHandle output_semaphore;
} tcp_client_config_t;

extern void tcp_client_task(void *pvParameters);
