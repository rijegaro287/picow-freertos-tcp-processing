#include "pico/cyw43_arch.h"

#include "FreeRTOS.h"
#include "task.h"

#include "tcp_server.h"
#include "tcp_client.h"
#include "processing.h"

#define NUMBER_OF_HANDLES 5

#define QUEUE_LENGTH 10
#define BUFFER_SIZE 512


typedef struct _wifi_credentials {
	const char *ssid;
	const char *password;
} wifi_credentials_t;

typedef enum _handle_index {
	CONNECTION_HANDLE,
	SERVER_HANDLE,
	CLIENT_HANDLE,
	PROCESSING_HANDLE,
	NOISE_HANDLE
} handle_index_t;


static TaskHandle_t task_handles[NUMBER_OF_HANDLES];

static void wifi_connect_task(void *pvParameters) {
	wifi_credentials_t *credentials = (wifi_credentials_t *)pvParameters;

	if(cyw43_arch_init() != PICO_OK) {
    printf("Failed to initialize CYW43_ARCH\n");
		vTaskDelete(NULL);
		return;
  };

  cyw43_arch_enable_sta_mode();

	int32_t status = CYW43_LINK_FAIL;
	while(status != CYW43_LINK_JOIN) {
		printf("Attempting to connect to Network: %s\n", credentials->ssid);
		cyw43_arch_wifi_connect_timeout_ms(credentials->ssid, 
																			 credentials->password,
																			 CYW43_AUTH_WPA2_MIXED_PSK,
																			 10000);
		
		status = cyw43_wifi_link_status(&cyw43_state, CYW43_ITF_STA);
		if(status == CYW43_LINK_JOIN) {
			printf("Connected to Network: %s\n", credentials->ssid);
		}
		else {
			printf("Failed to connect to Network: %s\n", credentials->ssid);
			vTaskDelay(pdMS_TO_TICKS(500));
		}
	}

	for(uint32_t i = 0; i < NUMBER_OF_HANDLES; i++) {
		if(task_handles[i] != NULL) {
			xTaskNotifyGive(task_handles[i]);
		}
	}

	vTaskDelete(NULL);
}

void noise_task(void *pvParameters) {
	while(true) {
		printf("Noise\n");
		vTaskDelay(pdMS_TO_TICKS(2000));
	}
}

int main() {
	queue_handles_t processing_handles = {
		.input_queue = xQueueCreate(QUEUE_LENGTH, BUFFER_SIZE),
		.output_queue = xQueueCreate(QUEUE_LENGTH, BUFFER_SIZE)
	};

	wifi_credentials_t wifi_credentials = {
		.ssid = "Familia Gatgens",
		.password = "adita123"
	};

	tcp_server_config_t tcp_server_config = {
		.port = 4242,
		.input_queue = processing_handles.input_queue,
	};

	tcp_client_config_t tcp_client_config = {
		.server_ip = "192.168.100.66",
		.port = 4242,
		.output_queue = processing_handles.output_queue
	};

	for(uint32_t i = 0; i < 5; i++) {
		task_handles[i] = NULL;
	}

	stdio_init_all();

	xTaskCreate(wifi_connect_task,
						  "connection to wifi",
							configMINIMAL_STACK_SIZE,
							&wifi_credentials,
							10,
							&(task_handles[CONNECTION_HANDLE]));

	xTaskCreate(tcp_server_task, 
						  "tcp server",
							configMINIMAL_STACK_SIZE,
							&tcp_server_config,
							5,
							&(task_handles[SERVER_HANDLE]));

	xTaskCreate(tcp_client_task,
						  "tcp client",
							configMINIMAL_STACK_SIZE,
							&tcp_client_config,
							5,
							&(task_handles[CLIENT_HANDLE]));

	xTaskCreate(processing_task,
						  "processing",
							configMINIMAL_STACK_SIZE,
							&processing_handles,
							5,
							&(task_handles[PROCESSING_HANDLE]));

	xTaskCreate(noise_task,
						  "noise",
							configMINIMAL_STACK_SIZE,
							NULL,
							1,
							&(task_handles[NOISE_HANDLE]));

	vTaskStartScheduler();

	return 0;
}
