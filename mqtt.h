
#define CLIENTID    "Paho"

MQTTClient MQTT_connect(char * address, char * username, char * password);
void MQTT_connection_lost(void *context, char *cause);
int MQTT_message_arrived(void *context, char *topicName, int topicLen, MQTTClient_message *message);
void MQTT_delivery_complete(void *context, MQTTClient_deliveryToken dt);
int MQTT_pub(MQTTClient client, char * topic, char * msg);
int MQTT_sub(MQTTClient client, char * topic);
int MQTT_disconnect(MQTTClient client);
