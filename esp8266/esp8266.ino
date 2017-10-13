//#include <SoftwareSerial.h>
//SoftwareSerial esp(3, 2); // RX, TX

String ssid ="HOUSE";
String password="a1234567";
String server = "api.thingspeak.com";
String uri = "/update";
String apikey = "0JFBHJP721DRYBHX";
String data = apikey + "&field1=73";

void setup() {
  Serial3.begin(115200);
  Serial.begin(9600);
  reset();
  connectWifi();
}

//reset the esp8266 module
void reset() {
  Serial3.println("AT+RST");
  delay(1000);
  if(Serial3.find("OK") ) Serial.println("Module Reset");
}

//connect to your wifi network
void connectWifi() {
  String cmd = "AT+CWJAP=\"" +ssid+"\",\"" + password + "\"";
  Serial3.println(cmd);
  delay(4000);
  if(Serial3.find("OK")) {
    Serial.println("Connected!");
  } else {
    connectWifi();
    Serial.println("Cannot connect to wifi"); 
  }
}

void loop () {

  httppost();
  delay(5000);

}

void httppost () {
  
  Serial3.println("AT+CIPSTART=\"TCP\",\"" + server + "\",80");//start a TCP connection.
  
  if( Serial3.find("OK")) {
    Serial.println("TCP connection ready");
  } 
  
  delay(1000);
  
  String postRequest =
  
  "POST " + uri + " HTTP/1.1\r\n" +
  "Host: " + server + "\r\n" +
  "Connection: close\r\n" + 
  "X-THINGSPEAKAPIKEY: 0JFBHJP721DRYBHX\r\n" + 
  "Content-Type: application/x-www-form-urlencoded\r\n" +
  "Content-Length: " + data.length() + "\r\n" +
  "\r\n" + data;
  
  String sendCmd = "AT+CIPSEND=";//determine the number of caracters to be sent.
  
  Serial3.print(sendCmd);
  Serial3.println(postRequest.length() );
  delay(1000);
  
  if(Serial3.find(">")) { 
    Serial.println("Sending.."); 
    Serial3.print(postRequest);
    
    if( Serial3.find("SEND OK")) { 
      
      Serial.println("Packet sent");
      
      while (Serial3.available()) {
        String tmpResp = Serial3.readString();
        Serial.println(tmpResp);
      }
      
      // close the connection
      Serial3.println("AT+CIPCLOSE");
    }
  }
}
