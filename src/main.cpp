#include <WiFi.h>
#include <FirebaseESP32.h> // Instale esta biblioteca via Library Manager

// --- Configurações de Rede (Wokwi) ---
const char* ssid = "Wokwi-GUEST";
const char* password = "";

// --- Configurações do Firebase ---
#define FIREBASE_HOST "insira seu ID do seu projeto no formato URL" 
#define FIREBASE_AUTH "insira sua secret key"

// Objeto FirebaseData para comunicação
FirebaseData firebaseData;

// Objeto Firebase
FirebaseConfig firebaseConfig;
FirebaseAuth firebaseAuth;

// --- Pinos dos Botões ---
const int BUTTON_ANC1_PIN = 2;
const int BUTTON_ANC2_PIN = 4;
const int BUTTON_ANC3_PIN = 5;

// --- ID da Moto Fictícia ---
const String MOCK_MOTO_COD_TAG = "MOTO001"; 

// --- Estrutura para Posições Pré-Definidas ---
struct MockPosition {
  float x;
  float y;
};

// Posições que serão enviadas quando cada botão for pressionado
const MockPosition POS_BTN1 = {5.0, 5.0};   // Posição para Botão 1
const MockPosition POS_BTN2 = {15.0, 5.0};  // Posição para Botão 2
const MockPosition POS_BTN3 = {10.0, 12.0}; // Posição para Botão 3


// --- Funções ---
void sendPositionToFirebase(String codTag, float posX, float posY);


void setup() {
  Serial.begin(115200);
  delay(100);

  Serial.print("Conectando ao WiFi ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi Conectado! IP: ");
  Serial.println(WiFi.localIP());

  // Configurando pinos dos botões 
  pinMode(BUTTON_ANC1_PIN, INPUT_PULLUP);
  pinMode(BUTTON_ANC2_PIN, INPUT_PULLUP);
  pinMode(BUTTON_ANC3_PIN, INPUT_PULLUP);

  // --- Configurando Firebase ---
  firebaseConfig.host = FIREBASE_HOST;
  firebaseConfig.signer.tokens.legacy_token = FIREBASE_AUTH;

  Firebase.begin(&firebaseConfig, &firebaseAuth); 
  Firebase.reconnectWiFi(true);

  Serial.println("Gateway Principal com posições mockadas e Firebase pronto.");
}

void loop() {
  // --- Monitorar os botões e enviar posições mockadas ---
  if (digitalRead(BUTTON_ANC1_PIN) == LOW) { // Botão 1 pressionado
    Serial.println("Botão 1 pressionado! Enviando POS_BTN1 para Firebase.");
    sendPositionToFirebase(MOCK_MOTO_COD_TAG, POS_BTN1.x, POS_BTN1.y);
    delay(500); // Pequeno atraso para evitar múltiplos envios por um único clique
  }
  if (digitalRead(BUTTON_ANC2_PIN) == LOW) { // Botão 2 pressionado
    Serial.println("Botão 2 pressionado! Enviando POS_BTN2 para Firebase.");
    sendPositionToFirebase(MOCK_MOTO_COD_TAG, POS_BTN2.x, POS_BTN2.y);
    delay(500);
  }
  if (digitalRead(BUTTON_ANC3_PIN) == LOW) { // Botão 3 pressionado
    Serial.println("Botão 3 pressionado! Enviando POS_BTN3 para Firebase.");
    sendPositionToFirebase(MOCK_MOTO_COD_TAG, POS_BTN3.x, POS_BTN3.y);
    delay(500);
  }
}

void sendPositionToFirebase(String codTag, float posX, float posY) {
  if (WiFi.status() == WL_CONNECTED) {
    // Criar um JSON com os dados da posição
    FirebaseJson json;
    json.set("posicaoX", String(posX, 2));
    json.set("posicaoY", String(posY, 2));
    json.set("timestamp", String(millis()));

    String path = "/motos/posicao/";
    path.concat(codTag);

    Serial.print("Enviando para Firebase Path: ");
    Serial.println(path);
    Serial.print("Dados: ");
    Serial.println(json.raw());

    // Enviar os dados JSON para o Firebase
    if (Firebase.updateNode(firebaseData, path, json)) {
      Serial.println("SUCESSO: Dados enviados para Firebase.");
    } else {
      Serial.print("ERRO: Falha ao enviar para Firebase: ");
      Serial.println(firebaseData.errorReason());
    }
  } else {
    Serial.println("WiFi Desconectado. Não foi possível enviar para Firebase.");
  }
}