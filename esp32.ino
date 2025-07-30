#include <WiFi.h>
#include <HTTPClient.h>
#include <Arduino_JSON.h>

#define KEY_0_IN 18
#define KEY_1_IN 19
#define KEY_2_IN 21
#define KEY_3_IN 17

const int keys[4] = { KEY_0_IN, KEY_1_IN, KEY_2_IN, KEY_3_IN };

// WiFi network name and password:
const char * networkName = "<NETWORK NAME>";
const char * networkPswd = "<NETWORK PASSWORD>";

const char* deviceId = "<DEVICE ID>";
const char BASE_URL[] = "http://192.168.20.101:8080";
const char EXECUTE_SHORTCUT_URI[] = "/shortcuts/execute";

/* * * * SET UP * * * */
void setupKey(int keyPin) {
  pinMode(keyPin, INPUT_PULLUP);
  digitalWrite(keyPin, HIGH);
}

void setupKeys() {
  // Serial.begin(9600);

  setupKey(KEY_0_IN);
  setupKey(KEY_1_IN);
  setupKey(KEY_2_IN);
  setupKey(KEY_3_IN);
}
/* * * * * * * * * * */


/* * * * PERIPHERALS * * * */
bool keyPressed(int keyPin) {
  const bool keyState = digitalRead(keyPin) == 0 ? true : false;
  return keyState;
}
/* * * * * * * * * * */


int getDeviceKeyForPin(int keyPin) {
  switch (keyPin) {
    case KEY_0_IN:
      return 0;
    case KEY_1_IN:
      return 1;
    case KEY_2_IN:
      return 2;
    case KEY_3_IN:
      return 3;
  }
}

void setup()
{
  delay(500);

  Serial.begin(115200);

  setupKeys();

  // Connect to the WiFi network (see function below loop)
  connectToWiFi(networkName, networkPswd);
}

void handleKeyPress(int keyPin) {
  Serial.print(keyPin);
  Serial.println(" - key pressed");
  executeShortcut(keyPin);
}

void executeShortcut(int keyPin) {
  httpPOSTrequest(String(BASE_URL) + String(EXECUTE_SHORTCUT_URI), keyPin);
}

void loop()
{
  for (int key : keys) {
    if (keyPressed(key)) {
      handleKeyPress(key);
    }
  }

  delay(100);
}

void connectToWiFi(const char * ssid, const char * pwd)
{
  printLine();
  Serial.print("Connecting to WiFi network: ");
  Serial.println(String(ssid));

  WiFi.begin(ssid, pwd);

  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("WiFi connected!");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void printLine()
{
  Serial.println();
  for (int i=0; i<30; i++)
    Serial.print("-");
  Serial.println();
}

String httpPOSTrequest(const String url, const int deviceKeyId) {

  const char* DEVICE_ID = "deviceId";
  const char* DEVICE_KEY_ID = "deviceKeyId";

    WiFiClient client;
    HTTPClient http;

    
      // Your Domain name with URL path or IP address with path
      http.begin(client, url);
      
      // Specify content-type header
      http.addHeader("Content-Type", "application/json");

      // Data to send with HTTP POST
      String httpRequestData = String("{") +
        String("\"") + DEVICE_ID + String("\": \"") + String(deviceId) + String("\"") + String(",") +
        String("\"") + DEVICE_KEY_ID + String("\": \"") + String(getDeviceKeyForPin(deviceKeyId)) + String("\"") + 
      String("}");        

      Serial.println(httpRequestData);  

      // Send HTTP POST request
      int httpResponseCode = http.POST(httpRequestData);
      
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);
        
      // Free resources
      http.end();
}

String httpGETRequest(const char* url) {
  WiFiClient client;
  HTTPClient http;
    
  // Your Domain name with URL path or IP address with path
  http.begin(client, url);
  
  // Send HTTP POST request
  int httpResponseCode = http.GET();
  
  String payload = "{}"; 
  
  if (httpResponseCode>0) {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    payload = http.getString();
  }
  else {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
  }
  // Free resources
  http.end();

  return payload;
}

