#pragma once

#include "stdio.h"

#include "FreeRTOS.h"
#include "queue.h"

#include "tcp_server.h"


typedef struct _queue_handles_t {
  xQueueHandle input_queue;
  xQueueHandle output_queue;
} queue_handles_t;

extern void processing_task(void *pvParameters);
