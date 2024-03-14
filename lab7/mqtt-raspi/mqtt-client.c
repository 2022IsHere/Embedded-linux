#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "MQTTClient.h"
#include "gpio.h"
// At home
// 192.168.68.57 HOST PC
// 192.168.68.57 Raspi
// 192.168.68.58 VM

// At school
// 172.27.228.37 HOST PC
// 172.27.227.238 Raspi
// 172.27.228.38 VM

#include <gpiod.h>
#include <pthread.h>
#include <sys/time.h> 

// At home VM ADDRESS
//#define ADDRESS     "tcp://192.168.68.58:1883"
// At school VM ADRESS 
#define ADDRESS "tcp://172.27.228.38:1883"

#define CLIENTID    "ExampleClientPub"
#define TOPIC       "/home/sensors/temp/kitchen"
#define PAYLOAD     "Hi there! Hopefully you see me."
#define QOS         1
#define TIMEOUT     10000L

int main(int argc, char* argv[])
{
    struct gpiod_chip *chip;
    struct gpiod_line *lineButton;	        // System mode control button (pin 22)

    char *chipname = "gpiochip0";

    int ret;

    chip = gpiod_chip_open_by_name(chipname);
	if (!chip) {
		perror("Open chip failed\n");
		ret = -1;
		goto end;
	}

    pthread_t gpio_thread;
    struct timespec ts = { WAIT_FOR_SEC, WAIT_FOR_NS };
    lineButton = gpiod_chip_get_line(chip, BUTTON_PIN);
    gpiod_line_request_rising_edge_events(lineButton, CONSUMER);

    gpio_stuff_t gpio_stuff;

	gpio_stuff.line = lineButton;
	gpio_stuff.line_num = BUTTON_PIN;
	gpio_stuff.ts = ts;

    pthread_create( &gpio_thread, NULL, gpio_function, &gpio_stuff);

    while(true) {
        if (gpiod_line_get_value(lineButton) == 0) {

            printf("\nButton has been pressed! Message on it's way! Hold on to your boxers!\n\n");

            MQTTClient client;
            MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
            MQTTClient_message pubmsg = MQTTClient_message_initializer;
            MQTTClient_deliveryToken token;
            int rc;

            MQTTClient_create(&client, ADDRESS, CLIENTID,
                MQTTCLIENT_PERSISTENCE_NONE, NULL);
            conn_opts.keepAliveInterval = 20;
            conn_opts.cleansession = 1;

            if ((rc = MQTTClient_connect(client, &conn_opts)) != MQTTCLIENT_SUCCESS)
            {
                printf("Failed to connect, return code %d\n", rc);
                exit(-1);
            }
            pubmsg.payload = PAYLOAD;
            pubmsg.payloadlen = strlen(PAYLOAD);
            pubmsg.qos = QOS;
            pubmsg.retained = 0;
            MQTTClient_publishMessage(client, TOPIC, &pubmsg, &token);
            printf("\n\n\n\nWaiting for up to %d seconds for publication of %s\n"
                    "on topic %s for client with ClientID: %s\n",
                    (int)(TIMEOUT/1000), PAYLOAD, TOPIC, CLIENTID);
            rc = MQTTClient_waitForCompletion(client, token, TIMEOUT);
            printf("Message with delivery token %d delivered\n", token);
            MQTTClient_disconnect(client, 10000);
            MQTTClient_destroy(&client);
            return rc;
        }
        usleep(100e3);
    }

close_chip:
	gpiod_chip_close(chip);
end:
	return ret;
}