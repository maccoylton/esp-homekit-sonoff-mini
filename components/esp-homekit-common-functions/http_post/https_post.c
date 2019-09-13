#include <wolfssl/ssl.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <http_post.h>
#include <task.h>
#include <lwip/sockets.h>

#define WEB_SERVER "192.168.1.2"
#define WEB_PORT 80
#define WEB_URL "/esplogger/postsql.php"
#define POST_REQUEST "POST %s  HTTP/1.0\r\nHost: %s\r\nContent-type: application/x-www-form-urlencoded\r\nContent-Length: %d\r\n\r\n%s"

char request[450];
char details[150];


void https_post_wolfssl_task(void * pvParameters)
{
    
    
    while (1) {
        printf ("Suspending http_post_task\n");
        vTaskSuspend( NULL );
        printf ("Resumed http_post_task\n");
        
        int sockfd;
        
        WOLFSSL_CTX* ctx;
        
        WOLFSSL* ssl;
        
        WOLFSSL_METHOD* method;
        struct  sockaddr_in servAddr;
        
        /* create and set up socket */
        
        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        memset(&servAddr, 0, sizeof(servAddr));
        servAddr.sin_family = AF_INET;
        servAddr.sin_port = htons(WEB_PORT);
        
        /* connect to socket */
        connect(sockfd, (struct sockaddr *) &servAddr, sizeof(servAddr));
        
        /* initialize wolfssl library */
        
        wolfSSL_Init();
        method = wolfTLSv1_2_client_method(); /* use TLS v1.2 */
        
        /* make new ssl context */
        if ( (ctx = wolfSSL_CTX_new(method)) == NULL) {
            printf("wolfSSL_CTX_new error");
        }
        
        /* make new wolfSSL struct */
        if ( (ssl = wolfSSL_new(ctx)) == NULL) {
            printf("wolfSSL_new error");
        }
        
        /* Add cert to ctx */
        if (wolfSSL_CTX_load_verify_locations(ctx, "certs/ca-cert.pem", 0) !=SSL_SUCCESS) {
            printf("Error loading certs/ca-cert.pem");
        }
        
        request[0] = "\0";
        snprintf(details, 150, post_string);
        snprintf(request, 450, "POST %s  HTTP/1.0\r\n"
                 "Host: %s\r\n"
                 "Content-type: application/x-www-form-urlencoded\r\n"
                 "Content-Length: %d\r\n\r\n"
                 "%s", WEB_URL, WEB_SERVER, strlen(details), details);
        printf(request);
        
        
        /* Connect wolfssl to the socket, server, then send message */
        wolfSSL_set_fd(ssl, sockfd);
        wolfSSL_connect(ssl);
        wolfSSL_write(ssl, request, strlen(request));
        
        /* frees all data before client termination */
        wolfSSL_free(ssl);
        wolfSSL_CTX_free(ctx);
        wolfSSL_Cleanup();
    }
}
