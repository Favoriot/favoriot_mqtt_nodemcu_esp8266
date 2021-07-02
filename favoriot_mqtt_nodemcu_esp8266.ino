#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#define accessToken "rw-apikey/accesstoken"  //replace with your access token

const char* ssid = "";  // **replace the SSID
const char* pass = "";  // **replace the password

const char* mqttServer = "mqtt.favoriot.com";
const int mqttPort = 1883;

const char* username = accessToken;
const char* password = accessToken;

unsigned long lastSend;

WiFiClient espClient;
PubSubClient mqttClient(espClient);

void reconnect() {
  while (!mqttClient.connected()) {
     Serial.print("re-try Connecting to Favoriot platform ...");
     if(mqttClient.connect("bc5a7607-9423-4b16-b420-21081b721c3f",username,password)){ // set any device id or use device id in favoriot platform
        Serial.println("Connected");
        String tempToken = accessToken;
        String topicSubscribe = tempToken + "/v2/streams/status";
        char tempSubscribe[topicSubscribe.length()+1];
        topicSubscribe.toCharArray(tempSubscribe,topicSubscribe.length()+1);
        mqttClient.subscribe(tempSubscribe);
      }
      else{
        Serial.print(mqttClient.state());
        Serial.println( " : retrying in 5 seconds]" );
      // Wait 5 seconds before retrying
        delay(5000);
      }
   }
}

void callback(char* topic, byte* payload, unsigned int length) {
    Serial.print("Response:[");
    for (int i = 0; i < length; i++){
       Serial.print((char)payload[i]); 
     }
    Serial.print("] ");
    Serial.println();
 }

void setup() {
   Serial.begin(115200);
   while(WiFi.status() != WL_CONNECTED){
       Serial.print("Attempting to connect to WPA SSID: ");
       Serial.println(ssid);
       WiFi.begin(ssid,pass); // connect to WPA/WPA2 network
       delay(5000);
   }
   Serial.println("Connected to the network");
   delay(5000);
   mqttClient.setServer(mqttServer,mqttPort);
   mqttClient.setCallback(callback);
}

void loop() {
   if (!mqttClient.connected()) {
      reconnect();
     } 
    if(millis() - lastSend > 30000 ) { // Update and send only after 30 seconds
     int randomNum = random(0,100); //generate random number
     dataStream(randomNum);  //send data to FAVORIOT platform
     lastSend = millis(); 
    }

    mqttClient.loop();
}


void dataStream(int randomNum){ 
  String json = "{";
    json += "\"device_developer_id\":"; 
    json += "\"deviceDefault@favoriot\""; // replace with your device developer id
    json +=",";
    json +="\"data\":";
    json += "{";
    json += "\"Number\":";
    json += "\""+String(randomNum)+"\"";
    json += "}}";

    Serial.print("Sending: ");
    Serial.println(json);
    char tempObj[json.length()+1];
    json.toCharArray(tempObj,json.length()+1);
    String tempToken = accessToken;
    String topicPublish = tempToken+"/v2/streams";
    char tempPublish[topicPublish.length()+1];
    topicPublish.toCharArray(tempPublish,topicPublish.length()+1);
    mqttClient.publish(tempPublish,tempObj);

}
