#pragma once

#include "stdio.h"
#include "pico/cyw43_arch.h"

#include "lwip/ip4_addr.h"
#include "lwip/netif.h"
#include "lwip/sockets.h"

#include "FreeRTOS.h"
#include "queue.h"


typedef struct _tcp_server_config {
  const uint16_t port;
  xQueueHandle input_queue;
} tcp_server_config_t;


extern void tcp_server_task(void *pvParameters);