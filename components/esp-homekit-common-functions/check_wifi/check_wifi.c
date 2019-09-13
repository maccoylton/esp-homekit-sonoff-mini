#include <stdio.h>
#include <espressif/esp_wifi.h>
#include <espressif/esp_sta.h>
#include <espressif/esp_common.h>
#include <esp8266.h>
#include <FreeRTOS.h>
#include <task.h>
#include <led_codes.h>

#include "check_wifi.h"

bool wifi_connected;

void checkWifiTask(void *pvParameters)
{
    uint8_t status ;
	
    wifi_connected = false;

    while (1)
    {
	status = sdk_wifi_station_get_connect_status();
 		switch (status)
        	{
        	case STATION_WRONG_PASSWORD:
            		printf("WiFi: wrong password\n\r");
            		break;
        	case STATION_NO_AP_FOUND:
            		printf("WiFi: AP not found\n\r");
            		break;
        	case STATION_CONNECT_FAIL:
            		printf("WiFi: connection failed\r\n");
            		break;
        	case STATION_GOT_IP:
			wifi_connected = true;
            		break;
        	default:
            		printf("%s: status = %d\n\r", __func__, status);
            		break;
        	}
		vTaskDelay(CHECK_INTERVAL / portTICK_PERIOD_MS);
    }
}
