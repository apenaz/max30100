#include <Arduino.h>
#include <Wire.h>
#include "MAX30100_PulseOximeter.h"
#include <ESP8266WiFi.h>
#include <SaIoTDeviceLib.h>

/**
 * configurações para uso do Saiot
 * 
 */
#define timeToSend 5
const char *deviceName = "oximetro";
const char *deviceSerial = "001";
//const char* email_do_sigaa = "email_do_sigaa@colocar.aqui";
const char *email_do_sigaa = "tinasaude@email.com";
String senhaDoUsuario = "senha123456";
unsigned long tDecorrido;
WiFiClient espClient;
void callback(char *topic, byte *payload, unsigned int length);

bool deadBand();
void enviarDados();
// criação do dispositivo
SaIoTDeviceLib dispositivo(deviceName, deviceSerial, email_do_sigaa);
// criação dos sensores
SaIoTSensor bpm("bpm", "bpm", "bpm", "number");
SaIoTSensor spo2("SpO2", "SpO2", "%", "number");

#define REPORTING_PERIOD_MS 1000

// PulseOximeter is the higher level interface to the sensor
// it offers:
//  * beat detection reporting
//  * heart rate calculation
//  * SpO2 (oxidation level) calculation
PulseOximeter pox;

uint32_t tsLastReport = 0;

// Callback (registered below) fired when a pulse is detected
void onBeatDetected()
{
  Serial.println("Beat!");
}

void setup()
{
  Serial.begin(115200);
  dispositivo.addSensor(bpm);
  dispositivo.addSensor(spo2);
  dispositivo.preSetCom(espClient, callback, 300);
  dispositivo.start(senhaDoUsuario);

  Serial.print("Initializing pulse oximeter..");

  // Initialize the PulseOximeter instance
  // Failures are generally due to an improper I2C wiring, missing power supply
  // or wrong target chip
  if (!pox.begin())
  {
    Serial.println("FAILED");
    for (;;)
      ;
  }
  else
  {
    Serial.println("SUCCESS");
  }

  // The default current for the IR LED is 50mA and it could be changed
  //   by uncommenting the following line. Check MAX30100_Registers.h for all the
  //   available options.
  // pox.setIRLedCurrent(MAX30100_LED_CURR_7_6MA);

  // Register a callback for the beat detection
  pox.setOnBeatDetectedCallback(onBeatDetected);
  tDecorrido = millis();
}

void loop()
{
  delay(100);
  dispositivo.handleLoop();
  // Make sure to call update as fast as possible
  pox.update();

  // Asynchronously dump heart rate and oxidation levels to the serial
  // For both, a value of 0 means "invalid"
  if (millis() - tsLastReport > REPORTING_PERIOD_MS)
  {
    Serial.print("Heart rate:");
    Serial.print(pox.getHeartRate());
    Serial.print("bpm / SpO2:");
    Serial.print(pox.getSpO2());
    Serial.println("%");
  }
  if (((millis() - tDecorrido) / 1000) >= timeToSend)
  {
    enviarDados();
    tDecorrido = millis();
  }
  dispositivo.handleLoop();
}

void callback(char *topic, byte *payload, unsigned int length)
{
  String payloadS;

  for (unsigned int i = 0; i < length; i++)
  {
    payloadS += (char)payload[i];
  }
  if (strcmp(topic, dispositivo.getSerial().c_str()) == 0)
  {
    //Serial.println("SerialLog: " + payloadS);
  }

  if (strcmp(topic, (dispositivo.getSerial() + bpm.getKey()).c_str()) == 0)
  {
  }

  if (strcmp(topic, (dispositivo.getSerial() + spo2.getKey()).c_str()) == 0)
  {
  }
}

void enviarDados()
{
  String data_hora = SaIoTCom::getDateNow();
  bpm.sendData(pox.getHeartRate());
  spo2.sendData(pox.getSpO2());
}
