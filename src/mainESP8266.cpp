

//https://github.com/acrobotic/Ai_Tips_ESP8266/blob/master/webserver_websockets/webserver_websockets.ino

/*------------------------------------------------------------------------------
  07/01/2018
  Author: Makerbro
  Platforms: ESP8266
  Language: C++/Arduino
  File: webserver_html.ino
  ------------------------------------------------------------------------------
  Description:
  Code for YouTube video demonstrating how to transfer data between a web server
  and a web client in real-time using websockets.
  https://youtu.be/ROeT-gyYZfw
  ------------------------------------------------------------------------------
 */

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WebSocketsServer.h>
#include <ESP8266mDNS.h>

ESP8266WebServer server(80);
WebSocketsServer webSocket = WebSocketsServer(81);

#define rele1  D0  //(D0)
#define rele2  D1 // D1
#define rele3  D2 // D3


const char WiFiAPPSK[] = "12345678";  //CONTRASEÑA
const char ssid[] = "ESP8266"; // NOMBRE DE LA RED
uint8_t pin_led = 2;

char webpage[] PROGMEM = R"=====(
<html>
<head>
  <script>
    var Socket;

    function init()
    {
      Socket = new WebSocket('ws://' + window.location.hostname + ':81/',['arduino']);
      Socket.onopen = function ()
      {
        Socket.send('Connect ' + new Date());
      }
      Socket.onmessage = function(event)
      {
        document.getElementById("rxConsole").value += event.data;
      }
    }

    function sendTextSSID()
    {
      Socket.send("#"+document.getElementById("txSSID").value);
      document.getElementById("txSSID").value = "";
    }
    function sendTextPswd()
    {
      Socket.send("!"+document.getElementById("txPswd").value);
      document.getElementById("txPswd").value = "";
    }
    function sendTextNum1()
    {
      Socket.send("*"+document.getElementById("txNum1").value);
      document.getElementById("txNum1").value = "";
    }
    function sendTextCol1()
    {
      Socket.send("+"+document.getElementById("txCol1").value);
      document.getElementById("txCol1").value = "";
    }
    function sendTextNum2()
    {
      Socket.send("/"+document.getElementById("txNum2").value);
      document.getElementById("txNum1").value = "";
    }
    function sendTextCol2()
    {
      Socket.send("-"+document.getElementById("txCol2").value);
      document.getElementById("txCol2").value = "";
    }



</script>
</head>
<body onload="javascript:init()">
  <div>
    <textarea id="rxConsole"></textarea>
  </div>
  <hr/>
  <div>
    <br/>SSID: <input type="text" id="txSSID" onkeydown="if(event.keyCode == 13) sendTextSSID();" />
  </div>
  <div>
    <br/>PASSWORD:<input type="text" id="txPswd" onkeydown="if(event.keyCode == 13) sendTextPswd();" />
  </div>
  <div>
    <br/>NUMERO 1: <input type="text" id="txNum1" onkeydown="if(event.keyCode == 13) sendTextNum1();" />
  </div>
  <div>
    <br/>COLOR 1: <input type="text" id="txCol1" onkeydown="if(event.keyCode == 13) sendTextCol1();" />
  </div>
  <div>
    <br/>NUMERO 2: <input type="text" id="txNum2" onkeydown="if(event.keyCode == 13) sendTextNum2();" />
  </div>
  <div>
    <br/>COLOR 2: <input type="text" id="txCol2" onkeydown="if(event.keyCode == 13) sendTextCol2();" />
  </div>

  <hr/>
</body>
</html>
)=====";

//------------------------------------------------------------ setupWiFi
void setupWiFi()
{
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid, NULL);
  Serial.print("\n IP Address: ");
  Serial.println(WiFi.softAPIP());
}
//-------------------------------------------------------------webSocketEvent
void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length)
{
  switch(type)
    {
      case WStype_TEXT:
        for(int i = 0; i < length; i++) Serial.print((char) payload[i]);

      break;

      case WStype_DISCONNECTED:
            Serial.printf("[%u] Disconnected!\n", num);
      break;


     case WStype_CONNECTED: {
            IPAddress ip = webSocket.remoteIP(num);
            Serial.printf("[%u] Connected from %d.%d.%d.%d url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);

            // send message to client
            webSocket.sendTXT(num, "Connected");
        }
      break;

    }
}

//--------------------------------------------------------------- setup
void setup()
{
  Serial.begin(115200);
  Serial.print("\nSetting up... ");

  //Engine channels
  pinMode(rele1, OUTPUT);
  pinMode(rele2,OUTPUT);
  pinMode(rele3,OUTPUT);
  digitalWrite(rele1, HIGH);
  digitalWrite(rele2,HIGH);
  digitalWrite(rele3,HIGH);


  delay(1000);//wait for a second

  setupWiFi();


  if(MDNS.begin("esp8266"))
  {
    Serial.println("\nMDNS responder started");
  }

  // handle index
  server.on("/", []()
  {
    // send index.html
    server.send_P(200, "text/html", webpage);

  });

  server.begin();
  // start webSocket server
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);

  // Add service to MDNS
  MDNS.addService("http", "tcp", 80);
  MDNS.addService("ws", "tcp", 81);
}

//----------------------------------------------------------- loop
void loop()
{
  webSocket.loop();
  server.handleClient();
  if(Serial.available() > 0)
  {
    char c[] = {(char)Serial.read()};
    //webSocket.broadcastTXT(c, sizeof(c));
    webSocket.broadcastTXT("Display Seguridad Industria 4.0");
  }
}
