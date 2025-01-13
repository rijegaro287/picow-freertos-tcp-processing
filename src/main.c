#include "FreeRTOS.h"
#include "task.h"

#include "tcp_server.h"
#include "processing.h"

#define NUMBER_OF_HANDLES 5

#define PROCESSING_QUEUE_SIZE 10
#define BUFFER_SIZE 512


typedef struct _wifi_credentials {
	const char *ssid;
	const char *password;
} wifi_credentials_t;

typedef struct _tcp_client_config {
	const char *server_ip;
	const uint16_t port;
} tcp_client_config_t;

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

void tcp_client_task(void *pvParameters) {
	ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

	tcp_client_config_t *config = (tcp_client_config_t *)pvParameters;
	while (true) {
		struct sockaddr_in dest_addr = {
			.sin_len = sizeof(struct sockaddr_in),
			.sin_family = AF_INET,
			.sin_addr.s_addr = inet_addr(config->server_ip),
			.sin_port = htons(config->port)
		};

		int32_t client_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
		if(client_socket < 0) {
			printf("Failed to create client socket\n");
			vTaskDelay(pdMS_TO_TICKS(500));
			continue;
		}

		int32_t status = -1;
		while(status != 0) {
			status = connect(client_socket, (struct sockaddr *)&dest_addr, sizeof(struct sockaddr_in));
			if(status != 0) {
				printf("Failed to connect to external server\n");
				vTaskDelay(pdMS_TO_TICKS(10));
			}
		}

		if(send(client_socket, "Hello", 5, 0) < 0) {
			printf("Failed to send data\n");
		}

		close(client_socket);
		vTaskDelay(pdMS_TO_TICKS(500));
	}

}

void noise_task(void *pvParameters) {
	while(true) {
		printf("Noise\n");
		vTaskDelay(pdMS_TO_TICKS(2000));
	}
}

int main() {
	wifi_credentials_t wifi_credentials = {
		.ssid = "Familia Gatgens",
		.password = "adita123"
	};

	tcp_server_config_t tcp_server_config = {
		.port = 4242,
		.processing_queue = xQueueCreate(PROCESSING_QUEUE_SIZE, BUFFER_SIZE)
	};

	tcp_client_config_t tcp_client_config = {
		.server_ip = "192.168.100.66",
		.port = 4242
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
							&(tcp_server_config.processing_queue),
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
