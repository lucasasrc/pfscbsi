#include <WiFi.h>
#include <HTTPClient.h>

// ==== CONFIGURAÇÕES ====
const char* ssid = "Wokwi-GUEST";
const char* password = "";
const char* urlAlarme = "https://onlinewinxclub.com/centro-web/admin/lucas/api/disparar_alarme.php";
const char* urlAcesso = "https://onlinewinxclub.com/centro-web/admin/lucas/api/registrar_acesso.php";

const int pinBotao = 13;
const int pinPIR = 15;
const int pinLED = 4;
const String senhaCorreta = "1234";

// ==== ESTADOS ====
bool aguardandoSenha = false;
int contadorEventos = 0;

void setup() {
  Serial.begin(115200);
  pinMode(pinBotao, INPUT_PULLUP);
  pinMode(pinPIR, INPUT);
  pinMode(pinLED, OUTPUT);

  Serial.println("[LOG] Inicializando Wi‑Fi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\n[LOG] Wi‑Fi conectado: " + WiFi.localIP().toString());
}

void loop() {
  // === DETECÇÃO DE MOVIMENTO ===
  if (digitalRead(pinPIR) == HIGH) {
    Serial.println("[LOG] Movimento detectado.");
    enviarAlarme();
    delay(500);
  }

  // === VERIFICAÇÃO DE BOTÃO ===
  if (digitalRead(pinBotao) == LOW && !aguardandoSenha) {
    Serial.println("[LOG] Botão pressionado. Digite a senha:");
    aguardandoSenha = true;
    delay(500);
  }

  // === PROCESSAMENTO DE SENHA ===
  if (aguardandoSenha && Serial.available()) {
    String entrada = Serial.readStringUntil('\n');
    entrada.trim();
    contadorEventos++;

    if (entrada == senhaCorreta) {
      Serial.println("[LOG] Senha correta. Acesso autorizado.");
      digitalWrite(pinLED, HIGH);
      unsigned long inicio = millis();
      registrarAcesso(true);
      unsigned long fim = millis();
      Serial.print("[METRICA] Latência HTTP (ms): ");
      Serial.println(fim - inicio);
      delay(2000);
      digitalWrite(pinLED, LOW);
    } else {
      Serial.println("[LOG] Senha incorreta. Acesso negado.");
      registrarAcesso(false);
    }

    aguardandoSenha = false;
  }

  delay(100);
}

// === ENVIA LOG DE DISPARO DE ALARME ===
void enviarAlarme() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(urlAlarme);
    int resposta = http.POST(""); // Sem corpo
    Serial.print("[LOG] Alarme enviado. Código HTTP: ");
    Serial.println(resposta);
    http.end();
  } else {
    Serial.println("[ERRO] Wi‑Fi desconectado. Falha ao enviar alarme.");
  }
}

// === REGISTRA ACESSO PERMITIDO OU NEGADO ===
void registrarAcesso(bool sucesso) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(urlAcesso);
    http.addHeader("Content-Type", "application/json");
    String payload = "{\"sucesso\":" + String(sucesso ? "1" : "0") + "}";
    int resposta = http.POST(payload);
    Serial.print("[LOG] Registro de acesso enviado. Código HTTP: ");
    Serial.println(resposta);
    http.end();
  } else {
    Serial.println("[ERRO] Wi‑Fi desconectado. Falha ao registrar acesso.");
  }
}
