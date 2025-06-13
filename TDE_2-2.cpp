#include <WiFi.h>
#include <HTTPClient.h>

const char* ssid = "Wokwi-GUEST";
const char* password = "";
const char* urlAlarme = "https://onlinewinxclub.com/centro-web/admin/lucas/api/disparar_alarme.php";
const char* urlAcesso = "https://onlinewinxclub.com/centro-web/admin/lucas/api/registrar_acesso.php";

const int pinBotao = 13;
const int pinPIR = 15;
const int pinLED = 4;
const String senhaCorreta = "1234";

bool aguardandoSenha = false;
int contadorEventos = 0;
bool movimentoDetectado = false;

// buffer para os dados de uso de CPU
char bufferStats[512];

// ==== SETUP ====
void setup() {
  Serial.begin(115200);
  pinMode(pinBotao, INPUT_PULLUP);
  pinMode(pinPIR, INPUT);
  pinMode(pinLED, OUTPUT);

  Serial.println("[LOG] Conectando Wi-Fi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("\n[LOG] Wi-Fi conectado: " + WiFi.localIP().toString());

  // Criação de tarefas paralelas
  xTaskCreate(tarefaAlarme, "Alarme", 4096, NULL, 3, NULL);
  xTaskCreate(tarefaWiFi, "WiFiMonitor", 2048, NULL, 2, NULL);
  xTaskCreate(tarefaMonitorCPU, "CPU Monitor", 4096, NULL, 1, NULL);

}

// ==== LOOP PRINCIPAL ====
void loop() {
  if (digitalRead(pinBotao) == LOW && !aguardandoSenha) {
    Serial.println("[LOG] Botão pressionado. Digite a senha:");
    aguardandoSenha = true;
    delay(500);
  }

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

  // Flag de movimento setada pela task
  if (digitalRead(pinPIR) == HIGH) {
    movimentoDetectado = true;
  }

  delay(100);
}

// ==== TAREFA PARA MONITORAR O PIR ====
void tarefaAlarme(void* param) {
  while (true) {
    if (movimentoDetectado) {
      enviarAlarme();
      movimentoDetectado = false;
    }
    vTaskDelay(100 / portTICK_PERIOD_MS); // verifica a cada 100ms
  }
}

// ==== TAREFA PARA VERIFICAR WI-FI ====
void tarefaWiFi(void* param) {
  while (true) {
    if (WiFi.status() != WL_CONNECTED) {
      Serial.println("[LOG] Wi-Fi perdido. Tentando reconectar...");
      WiFi.disconnect();
      WiFi.begin(ssid, password);
    }
    vTaskDelay(10000 / portTICK_PERIOD_MS); // verifica a cada 10s
  }
}

// ==== FUNÇÃO: Disparar alarme ====
void enviarAlarme() {
  Serial.println("[LOG] Disparando alarme!!.");
  if (WiFi.status() == WL_CONNECTED) {
    unsigned long inicio = millis(); // início da medição de latência

    HTTPClient http;
    http.begin(urlAlarme);
    int resposta = http.POST("");
    http.end();

    unsigned long fim = millis(); // fim da medição

    Serial.print("[LOG] Alarme enviado. Código HTTP: ");
    Serial.println(resposta);
    Serial.print("[METRICA] Latência do alarme (ms): ");
    Serial.println(fim - inicio);
  } else {
    Serial.println("[ERRO] Wi-Fi desconectado. Falha ao enviar alarme.");
  }
}

// ==== FUNÇÃO: Registrar acesso ====
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
    Serial.println("[ERRO] Wi-Fi desconectado. Falha ao registrar acesso.");
  }
}

// ==== FUNÇÃO: Registrar o uso da CPU ====
void tarefaMonitorCPU(void* param) {
  while (true) {
    vTaskGetRunTimeStats(bufferStats);
    Serial.println("======= CPU USO POR TASK =======");
    Serial.println("Task          Tempo     % Uso");
    Serial.println("--------------------------------");
    Serial.print(bufferStats);
    Serial.println("================================");
    vTaskDelay(10000 / portTICK_PERIOD_MS); // a cada 10s
  }
}
