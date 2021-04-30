#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "MQTTClient.h"
#include "mqtt.h"

#define QOS         1
#define TIMEOUT     10000L

MQTTClient MQTT_connect(char * address, char * username, char * password)
{
    int rc;
    MQTTClient client;
    MQTTClient_create(&client, address, CLIENTID, MQTTCLIENT_PERSISTENCE_NONE, NULL);

    MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
    conn_opts.username = username;
    conn_opts.password = password;
    conn_opts.keepAliveInterval = 600;
    conn_opts.cleansession = 1;

    MQTTClient_setCallbacks(client, NULL, MQTT_connection_lost, MQTT_message_arrived, MQTT_delivery_complete);

    if ((rc = MQTTClient_connect(client, &conn_opts)) != MQTTCLIENT_SUCCESS)
    {
    	printf("Connection refused: ");
    	switch (rc) {
    		case 1: printf("unacceptable protocol version\n");
    			break;

    		case 2: printf("identifier rejected\n");
    			break;

    		case 3: printf("server unavailable\n");
    			break;

    		case 4: printf("bad user name or password\n");
    			break;

    		case 5: printf("not authorised\n");
    			break;

    		default: printf("unknown %d\n", rc);
    			break;
    	}
        exit(-1);
    }
    return(client);
}

void MQTT_connection_lost(void *context, char *cause)
{
    printf("Connection lost, cause: %s\n", cause);
}

int MQTT_message_arrived(void *context, char *topicName, int topicLen, MQTTClient_message *message)
{
    int i;
    char* payloadptr = message->payload;

    printf("Message recieved from topic %s [", topicName);

    for(i = 0; i < message->payloadlen; i++)
    {
        putchar(*payloadptr++);
    }
    printf("]\n");

    MQTTClient_freeMessage(&message);
    MQTTClient_free(topicName);
    return 1;
}

void MQTT_delivery_complete(void *context, MQTTClient_deliveryToken dt)
{
    printf("Delivery confirmed for message with token %d\n", dt);
}

int MQTT_pub(MQTTClient client, char * topic, char * msg)
{
    int rc;

    MQTTClient_message pubmsg = MQTTClient_message_initializer;
    pubmsg.payload = msg;
    pubmsg.payloadlen = strlen(msg);
    pubmsg.qos = QOS;
    pubmsg.retained = 0;

    MQTTClient_deliveryToken token;

    MQTTClient_publishMessage(client, topic, &pubmsg, &token);
    printf("Publishing [%s] to topic %s, delivery token %d\n",msg, topic, token);

    /* If synchronous, uncomment following lines */
    //rc = MQTTClient_waitForCompletion(client, token, TIMEOUT);
    //printf("Message with delivery token %d delivered\n", token);
}

int MQTT_sub(MQTTClient client, char * topic)
{
    MQTTClient_subscribe(client, topic, QOS);
}

int MQTT_disconnect(MQTTClient client)
{
    MQTTClient_disconnect(client, 10000);
    MQTTClient_destroy(&client);
}


