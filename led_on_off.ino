#include <WiFi.h>

// Constante para definir rede a ser conectada
const char* ssid = "RouterTest";
const char* password = "routertest";

// Define um IP estático
IPAddress local_IP(192, 168, 0, 150);
IPAddress gateway(192, 168, 0, 1);
IPAddress subnet(255, 255, 255, 0);
WiFiServer server(80);

#define LED32  32    // LED port32

String estado = "";
int wait30 = 30000; // timer usado em caso de desconexões

void setup() {
  Serial.begin(9600);
  pinMode(LED32, OUTPUT);

  // Configura o IP estático
  if (!WiFi.config(local_IP, gateway, subnet)) {
    Serial.println("Erro em configurar a rede.");
  }

  // Conecta a rede WI-FI
  Serial.println();
  Serial.print("Conectando-se com rede: ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Conectado a rede WiFi.");

  // Inicia o servidor web
  server.begin();
  Serial.println("Servidor Web iniciado.");

  // Mostra o IP do microcontrolador na serial
  Serial.print("IP para se conectar ao servidor web: ");
  Serial.print("http://");
  Serial.println(WiFi.localIP());
}

void loop() {
  // Se desconectado da rede, tenta reconectar a cada 30 segundos
  if ((WiFi.status() != WL_CONNECTED) && (millis() > wait30)) {
    Serial.println("Tentando reconectar a rede Wi-Fi...");
    WiFi.disconnect();
    WiFi.begin(ssid, password);
    wait30 = millis() + 30000;
  }
  // Confere se o cliente se conectou
  WiFiClient client = server.available();
  if (!client) {
    return;
  }

  Serial.print("Novo Cliente: ");
  Serial.println(client.remoteIP());

  // Espera que o cliente envie alguns dados
  while (!client.available()) {
    delay(1);
  }

  // Le as informações enviadas pelo cliente.
  String req = client.readStringUntil('\r');
  Serial.println(req);

  // Make the client's request.
  if (req.indexOf("ledon") != -1) {
    digitalWrite(LED32, HIGH);
    estado = "LED Ligado";
  }
  if (req.indexOf("ledoff") != -1) {
    digitalWrite(LED32, LOW);
    estado = "LED desligado";
  }

  //////////////////////////////////////////////
  // Página WEB. ////////////////////////////
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println(""); 
  client.println(estado); 

  client.flush();
  client.stop();
}
