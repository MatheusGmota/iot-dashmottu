#include <WiFi.h>
#include <HTTPClient.h>
#include <FirebaseESP32.h>
#include <ArduinoJson.h>

// --- Configurações de Rede ---
const char* ssid = "Wokwi-GUEST";
const char* password = "";

// --- Configurações do Firebase ---
#define FIREBASE_HOST "https://dashmottu-default-rtdb.firebaseio.com/" 
#define FIREBASE_AUTH "7sx1x5MMxMQA96oBmtk2PI0CjU5eEbVQsLGOJSXs"

// Objetos do Firebase
FirebaseData firebaseData;
FirebaseConfig firebaseConfig;
FirebaseAuth firebaseAuth;

// --- Endpoint da API ---
const char* API_URL = "https://dashmottu-api.onrender.com/api/moto?codTag=";

// --- Pinos dos Botões ---
const int BUTTON_ANC1_PIN = 2;
const int BUTTON_ANC2_PIN = 4;
const int BUTTON_ANC3_PIN = 5;

// --- ID da Moto ---
const String COD_TAG = "TAG002"; 

// --- Estrutura de Anchor ---
struct Anchor {
  float x;
  float y;
  float distance; // distância medida até a moto
};

// --- Anchors fixos ---
Anchor anchor1 = {0.0, 0.0, 0.0};
Anchor anchor2 = {10.0, 0.0, 0.0};
Anchor anchor3 = {5.0, 8.0, 0.0};

// --- Funções ---
void sendToFirebase(float posX, float posY);
void sendToAPI(float posX, float posY);
void trilateration();

// --- Setup ---
void setup() {
  Serial .begin(115200);
  Serial.print("Conectando ao WiFi ");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi Conectado! IP: ");
  Serial.println(WiFi.localIP());

  // Configuração dos botões
  pinMode(BUTTON_ANC1_PIN, INPUT_PULLUP);
  pinMode(BUTTON_ANC2_PIN, INPUT_PULLUP);
  pinMode(BUTTON_ANC3_PIN, INPUT_PULLUP);

  // Firebase
  // firebaseConfig.host = FIREBASE_HOST;
  // firebaseConfig.signer.tokens.legacy_token = FIREBASE_AUTH;
  // Firebase.begin(&firebaseConfig, &firebaseAuth);
  // Firebase.reconnectWiFi(true);

  Serial.println("Gateway iniciado.");
}

// --- Loop ---
void loop() {
  if (digitalRead(BUTTON_ANC1_PIN) == LOW) {
    anchor1.distance = random(500, 1500) / 100.0; // valor aleatório entre 5.0 e 15.0
    Serial.printf("Anchor1 detectou distância %.2f\n", anchor1.distance);
    trilateration();
    delay(500);
  }
  if (digitalRead(BUTTON_ANC2_PIN) == LOW) {
    anchor2.distance = random(500, 1500) / 100.0;
    Serial.printf("Anchor2 detectou distância %.2f\n", anchor2.distance);
    trilateration();
    delay(500);
  }
  if (digitalRead(BUTTON_ANC3_PIN) == LOW) {
    anchor3.distance = random(500, 1500) / 100.0;
    Serial.printf("Anchor3 detectou distância %.2f\n", anchor3.distance);
    trilateration();
    delay(500);
  }
}

// --- Trilateração ---
// Fórmula básica com 3 círculos (simplificação)
void trilateration() {
  float x1 = anchor1.x, y1 = anchor1.y, r1 = anchor1.distance;
  float x2 = anchor2.x, y2 = anchor2.y, r2 = anchor2.distance;
  float x3 = anchor3.x, y3 = anchor3.y, r3 = anchor3.distance;

  float A = 2 * (x2 - x1);
  float B = 2 * (y2 - y1);
  float C = r1*r1 - r2*r2 - x1*x1 + x2*x2 - y1*y1 + y2*y2;
  float D = 2 * (x3 - x2);
  float E = 2 * (y3 - y2);
  float F = r2*r2 - r3*r3 - x2*x2 + x3*x3 - y2*y2 + y3*y3;

  float denom = (A*E - B*D);
  if (abs(denom) < 1e-6) {
    Serial.println("Erro: denominador zero na trilateração");
    return;
  }

  float posX = (C*E - B*F) / denom;
  float posY = (A*F - C*D) / denom;

  Serial.printf("Posição calculada: (%.2f, %.2f)\n", posX, posY);

  // Enviar resultados
  // sendToFirebase(posX, posY);
  sendToAPI(posX, posY);
}

// --- Envia dados para Firebase ---
void sendToFirebase(float posX, float posY) {
  if (WiFi.status() == WL_CONNECTED) {
    FirebaseJson json;
    json.set("posicaoX", String(posX, 2));
    json.set("posicaoY", String(posY, 2));
    json.set("timestamp", String(millis()));

    String path = "/motos/posicao/";
    path.concat(COD_TAG);

    if (Firebase.updateNode(firebaseData, path, json)) {
      Serial.println("SUCESSO Firebase");
    } else {
      Serial.print("ERRO Firebase: ");
      Serial.println(firebaseData.errorReason());
    }
  }
}

// --- Envia dados para a API ---
void sendToAPI(float posX, float posY) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    String url = String(API_URL);
    url += COD_TAG;
    http.begin(url);
    http.addHeader("Content-Type", "application/json");

    StaticJsonDocument<200> doc;
    doc["posicaoX"] = posX;
    doc["posicaoY"] = posY;

    String payload;
    serializeJson(doc, payload);

    int httpCode = http.PUT(payload);

    if (httpCode > 0) {
      Serial.printf("API Status: %d\n", httpCode);
      String response = http.getString();
      Serial.println(response);
    } else {
      Serial.printf("Erro API: %s\n", http.errorToString(httpCode).c_str());
    }

    http.end();
  }
}