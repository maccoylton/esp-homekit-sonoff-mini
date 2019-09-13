#include <stdio.h>
#include <espressif/esp_wifi.h>
#include <espressif/esp_sta.h>
#include <espressif/esp_common.h>
#include <esp/uart.h>
#include <esp8266.h>
#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>


#include <string.h>

#include <lwip/err.h>
#include <lwip/sockets.h>
#include <lwip/sys.h>
#include <lwip/netdb.h>
#include <lwip/dns.h>
#include <http_post.h>


#define WEB_SERVER "192.168.1.2"
#define WEB_PORT "80"
#define WEB_URL "/esplogger/postsql.php"
char request[450];
char details[150];
float temperature;
float humidity;
float moisture;

void http_post_task(void * pvParameters)
{
    int successes = 0, failures = 0;
    printf("HTTP get task starting...\r\n");
    
    while(1) {
        //        sdk_wifi_set_sleep_type(WIFI_SLEEP_NONE);
        
        printf ("Suspending http_post_task\n");
        vTaskSuspend( NULL );
        
        printf ("Resumed http_post_task\n");
        
        const struct addrinfo hints = {
            .ai_family = AF_INET,
            .ai_socktype = SOCK_STREAM,
        };
        struct addrinfo *res;
        
        printf("Running DNS lookup for %s...\r\n", WEB_SERVER);
        int err = getaddrinfo(WEB_SERVER, WEB_PORT, &hints, &res);
        
        if(err != 0 || res == NULL) {
            printf("DNS lookup failed err=%d res=%p\r\n", err, res);
            if(res)
                freeaddrinfo(res);
            vTaskDelay(1000 / portTICK_PERIOD_MS);
            failures++;
            continue;
        }
        /* Note: inet_ntoa is non-reentrant, look at ipaddr_ntoa_r for "real" code */
        struct in_addr *addr = &((struct sockaddr_in *)res->ai_addr)->sin_addr;
        printf("DNS lookup succeeded. IP=%s\r\n", inet_ntoa(*addr));
        
        int s = lwip_socket(res->ai_family, res->ai_socktype, 0);
        if(s < 0) {
            printf("... Failed to allocate socket\r\n");
            freeaddrinfo(res);
            vTaskDelay(1000 / portTICK_PERIOD_MS);
            failures++;
            continue;
        }
        
        printf("... allocated socket: %d \r\n", s);
        
        if(lwip_connect(s, res->ai_addr, res->ai_addrlen) != 0) {
            lwip_close(s);
            freeaddrinfo(res);
            printf("... socket connect failed.\r\n");
            vTaskDelay(4000 / portTICK_PERIOD_MS);
            failures++;
            continue;
        }
        
        printf("... connected\r\n");
        freeaddrinfo(res);
        request[0] = "\0";
        //        snprintf(details, 80, "{\"temp\": %.3f, \"hum\": %.3f, \"mois\": %.3f}\r\n", temperature, humidity, moisture);
        
        snprintf(details, 150, post_string);
        snprintf(request, 450, "POST %s  HTTP/1.0\r\n"
                 "Host: %s\r\n"
                 //			"User-Agent: esp-open-rtos/0.1 esp8266\r\n"
                 "Content-type: application/x-www-form-urlencoded\r\n"
                 "Content-Length: %d\r\n\r\n"
                 "%s", WEB_URL, WEB_SERVER, strlen(details), details);
        printf(request);
        if (lwip_write(s, request, strlen(request)) < 0) {
            printf("... socket send failed\r\n");
            lwip_close(s);
            vTaskDelay(4000 / portTICK_PERIOD_MS);
            failures++;
            continue;
        }
        printf("... socket send success\r\n");
        
        static char recv_buf[200];
        int r;
        do {
            printf("receiving...");
            bzero(recv_buf, 200);
            r = lwip_read(s, recv_buf, 199);
            if(r > 0) {
                printf("%s", recv_buf);
            }
        } while(r > 0);
        
        printf("... done reading from socket. Last read return=%d errno=%d\r\n", r, errno);
        if(r != 0)
            failures++;
        else
            successes++;
        lwip_close(s);
        printf("successes = %d failures = %d\r\n", successes, failures);
        //        sdk_wifi_set_sleep_type(WIFI_SLEEP_LIGHT);
        vTaskDelay(10000 / portTICK_PERIOD_MS);
        printf("\r\nStarting again!\r\n");
    }
}

