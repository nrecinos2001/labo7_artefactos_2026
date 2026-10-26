#include <SPI.h>
#include <LoRa.h>

// ======================================================
// IDENTIFICACION
// ======================================================

const int MI_ID = 1;
const int DESTINO_ID = 2;

String mensajeOriginal = "ola de mar";

// ======================================================
// PINES
// ======================================================

// definir los pines del módulo LoRa con #define
#define LORA_SCK   18
#define LORA_MISO  19
#define LORA_MOSI  23
#define LORA_NSS   5

// definir el pin del sensor Touch
#define PIN_TOUCH     27
#define UMBRAL_TOUCH  1000
//       y el umbral. Recuerde comprobar el
//       valor de su ESP32 antes de fijar el umbral.

// ======================================================
// ATBASH
// ======================================================

char convertirAtbash(char letra) {

  if (letra >= 'A' && letra <= 'Z') {
    return 'Z' - (letra - 'A');
  }

  if (letra >= 'a' && letra <= 'z') {
    return 'z' - (letra - 'a');
  }

  return letra;
}

String cifrarAtbash(String mensaje) {

  String mensajeCifrado = "";

  for (int i = 0; i < mensaje.length(); i++) {
    mensajeCifrado += convertirAtbash(mensaje.charAt(i));
  }

  return mensajeCifrado;
}

// ======================================================
// ENVIAR MENSAJE
// ======================================================

void enviarMensaje() {

  String mensajeCifrado = cifrarAtbash(mensajeOriginal);

  Serial.println();
  Serial.println("TOUCH DETECTADO");

  Serial.print("Mensaje original: ");
  Serial.println(mensajeOriginal);

  Serial.print("Mensaje cifrado: ");
  Serial.println(mensajeCifrado);

  // Formato del paquete: ORIGEN|DESTINO|MENSAJE
  LoRa.beginPacket();
  LoRa.print(MI_ID);
  LoRa.print("|");
  LoRa.print(DESTINO_ID);
  LoRa.print("|");
  LoRa.print(mensajeCifrado);
  LoRa.endPacket();

  Serial.print("Paquete enviado: ");
  Serial.print(MI_ID);
  Serial.print("|");
  Serial.print(DESTINO_ID);
  Serial.print("|");
  Serial.println(mensajeCifrado);

  Serial.println("----------------------------");
}

// ======================================================
// SETUP
// ======================================================

void setup() {

  Serial.begin(115200);
  delay(1000);

  Serial.println("TRANSMISOR LORA");

  Serial.print("Mi ID: ");
  Serial.println(MI_ID);

  Serial.print("Destino: ");
  Serial.println(DESTINO_ID);

  // Iniciar el bus SPI: SCK, MISO, MOSI, SS
  SPI.begin(LORA_SCK, LORA_MISO, LORA_MOSI, LORA_NSS);

  // Pines del modulo: NSS, RESET (sin conectar), DIO0 (sin conectar)
  LoRa.setPins(LORA_NSS, -1, -1);

  if (!LoRa.begin(915E6)) {
    Serial.println("ERROR: No se pudo iniciar LoRa");
    while (true);
  }

  Serial.println("LoRa iniciado correctamente");
  Serial.println("Esperando toque...");
}

// ======================================================
// LOOP
// ======================================================

bool tocando = false; 

void loop() {

int valorTouch = touchRead(PIN_TOUCH);

  // Descomentar para calibrar el umbral:
  // Serial.println(valorTouch);

  if (valorTouch < UMBRAL_TOUCH) {
    // Solo enviar en el flanco: toque nuevo
    if (!tocando) {
      tocando = true;
      Serial.println("tocando");
      enviarMensaje();
    }
  } else {
    // Dedo retirado: listo para el siguiente toque
    tocando = false;
  }

  delay(100);
}
