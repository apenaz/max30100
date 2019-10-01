#include "SaIoTDeviceLib.h"

SaIoTDeviceLib::SaIoTDeviceLib(){};
SaIoTDeviceLib::SaIoTDeviceLib(String _name, String _serial, String _email)
{
  name = _name;
  serial = _serial;
  email = _email;
};
int SaIoTDeviceLib::tryConnection(unsigned long timeOutSeconds)
{
  WiFiManager wifi;
  if (timeOutSeconds)
  {
    wifi.setTimeout(timeOutSeconds);
  }
  if (!wifi.autoConnect(serial.c_str()))
  {
    Serial.println("failed to connect and hit timeout");
    delay(3000);
    //reset and try again, or maybe put it to deep sleep
    ESP.reset();
    delay(5000);
  }
};
void SaIoTDeviceLib::preSetCom(WiFiClient &espClient, fptr _function)
{
  objCom.setClient(espClient);
  objCom.setCallback(_function);
  objCom.setServerPort(HOST, PORT);
  tryConnection(0);
  //fptr = *SaIoTDeviceLib::callback(char* topic, byte* payload, unsigned int length);
  //objCom.setCallback(&SaIoTDeviceLib::callback(char* topic, byte* payload, unsigned int length));
};
void SaIoTDeviceLib::preSetCom(WiFiClient &espClient, fptr _function, unsigned long timeOutSeconds)
{
  objCom.setClient(espClient);
  objCom.setCallback(_function);
  objCom.setServerPort(HOST, PORT);
  tryConnection(timeOutSeconds);
};
void SaIoTDeviceLib::start(String s)
{
  startCom(HOST, PORT, hostHttp, POSTDISPOSITIVO, s);
};
void SaIoTDeviceLib::startCom(const char *hostSend, uint16_t portSend, const char *hostTok, const char *hostCd, String pUser)
{
  this->setToken(objCom.getToken(hostTok, email, pUser, serial));
  String keys[qtdControllers];
  for (int i = 0; i < qtdControllers; i++)
  {
    keys[i] += controllers[i]->getKey();
  }
  Serial.println(this->getJconf());
  objCom.registerDevice(serial, email, token, this->getJconf(), hostCd, keys, qtdControllers);
};
void SaIoTDeviceLib::setToken(String _token)
{
  token = _token;
};
void SaIoTDeviceLib::setEmail(String _email)
{
  email = _email;
};

bool SaIoTDeviceLib::reportController(String controllerKey, int value)
{
  String JSON;
  JSON += "{\"token\":\"" + token + "\",\"data\":{\"serial\":\"" + serial + "\",\"key\":\"" + controllerKey + "\",\"value\":\"" + String(value) + "\"}}";
  return objCom.putMeController(JSON);
}

bool SaIoTDeviceLib::reportController(String controllerKey, String value)
{
  String JSON;
  JSON += "{\"token\":\"" + token + "\",\"data\":{\"serial\":\"" + serial + "\",\"key\":\"" + controllerKey + "\",\"value\":\"" + value + "\"}}";
  return objCom.putMeController(JSON);
}

bool SaIoTDeviceLib::handleLoop()
{
  if (!objCom.connected())
  {
    Serial.println("Desconectado do BROKER ERR");
    String keys[qtdControllers];
    for (int i = 0; i < qtdControllers; i++)
    {
      keys[i] += controllers[i]->getKey();
    }
    objCom.registerDevice(serial, email, token, this->getJconf(), POSTDISPOSITIVO, keys, qtdControllers);
  }
  for (int i = 0; i < qtdSensors; i++)
  {
    if (sensors[i]->getReport())
    {
      String payload = ""; //VER COMO FICARIA PRA TRATAR A DATA
      //payload += "{\"token\":\""+token+"\",\"data\":{\"serial\":\""+serial+"\",\"key\":\""+sensors[i]->getKey()+"\",\"value\":";
      if (sensors[i]->getByField("type") == "number")
      {
        payload += "{\"token\":\"" + token + "\",\"data\":{\"serial\":\"" + serial + "\",\"key\":\"" + sensors[i]->getKey() + "\",\"value\":" + sensors[i]->getValue() + ",\"dateTime\":\"" + sensors[i]->getLastDate() + "\"}}";
      }
      else
      {
        payload += "{\"token\":\"" + token + "\",\"data\":{\"serial\":\"" + serial + "\",\"key\":\"" + sensors[i]->getKey() + "\",\"value\":\"" + sensors[i]->getValueStr() + "\",\"dateTime\":\"" + sensors[i]->getLastDate() + "\"}}";
      }

      if (objCom.publishData(payload))
      {
        Serial.println("Dado enviado!");
      }
      else
      {
        Serial.print("Erro ao enviar o dado: ");
        Serial.println(payload);
      }
    }
  }
  return objCom.handleCom();
}
String SaIoTDeviceLib::getName(void)
{
  return name;
}
String SaIoTDeviceLib::getSerial(void)
{
  return serial;
}
String SaIoTDeviceLib::getToken(void)
{
  return token;
}
String SaIoTDeviceLib::getEmail(void)
{
  return email;
}
String SaIoTDeviceLib::getJconf(void)
{
  String JSON;
  JSON += "{\"token\":\"" + token + "\",\"data\":{\"name\":\"" + name + "\",\"serial\":\"" + serial + "\"";
  if (qtdControllers > 0)
  {
    JSON += ",\"controllers\":[";
    for (int i = 0; i < qtdControllers; i++)
    {
      JSON += controllers[i]->getJsonConfig();
      if (i == qtdControllers - 1)
      {
        JSON += "]";
      }
      else
      {
        JSON += ",";
      }
    }
  }
  if (qtdSensors > 0)
  {
    JSON += ",\"sensors\":[";
    for (int i = 0; i < qtdSensors; i++)
    {
      JSON += sensors[i]->getJsonConfig();
      if (i == qtdSensors - 1)
      {
        JSON += "]";
      }
      else
      {
        JSON += ",";
      }
    }
  }
  JSON += "}}";
  return JSON;
}

int SaIoTDeviceLib::getNSensors(void)
{
  return qtdSensors;
}

int SaIoTDeviceLib::getNControllers(void)
{
  return qtdControllers;
}

void SaIoTDeviceLib::addSensor(SaIoTSensor &newSensor)
{
  sensors[qtdSensors++] = &newSensor;
}

void SaIoTDeviceLib::addController(SaIoTController &newController)
{
  controllers[qtdControllers++] = &newController;
}
