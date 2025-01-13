#include <SPI.h>
#include <SD.h>
#include <Wire.h>
#include <SoftwareSerial.h>
#include <TinyGPS.h>

#define LED_AVISO 2

int controle = 0;  // Definição da variável global
const char* caminho = "/GPS/Lat_Long.txt";  // Define o caminho do arquivo

SoftwareSerial serial1(27, 22); // RX, TX
TinyGPS gps1;

void verificarSD();
void criarPasta();
void criarArquivo();

void lerLatitudeandLogitudeandData();

void setup() {
  pinMode(LED_AVISO,OUTPUT);
  serial1.begin(9600);
  Serial.begin(9600);
  Serial.println("O GPS aguarda pelo sinal dos satelites...");
  verificarSD();
  criarPasta();
  criarArquivo();

}

void loop() {

  bool recebido = false;
  static unsigned long delayPrint;

  while (serial1.available()) {
     char cIn = serial1.read();
     recebido = (gps1.encode(cIn) || recebido);  //Verifica até receber o primeiro sinal dos satelites
  }
  if ( (recebido) && ((millis() - delayPrint) > 1000) ) {  //Mostra apenas após receber o primeiro sinal. Após o primeiro sinal, mostra a cada segundo.
     delayPrint = millis();
     Serial.println("----------------------------------------");
     float latitude, longitude; //As variaveis podem ser float, para não precisar fazer nenhum cálculo
     
     unsigned long idadeInfo;
     gps1.f_get_position(&latitude, &longitude, &idadeInfo);   //O método f_get_position é mais indicado para retornar as coordenadas em variáveis float, para não precisar fazer nenhum cálculo    

     if (latitude != TinyGPS::GPS_INVALID_F_ANGLE) {
        Serial.print("Latitude: ");
        Serial.println(latitude, 6);  //Mostra a latitude com a precisão de 6 dígitos decimais
     }

     if (longitude != TinyGPS::GPS_INVALID_F_ANGLE) {
        Serial.print("Longitude: ");
        Serial.println(longitude, 6);  //Mostra a longitude com a precisão de 6 dígitos decimais
     }

     //Dia e Hora
     int ano;
     byte mes, dia, hora, minuto, segundo, centesimo;
     gps1.crack_datetime(&ano, &mes, &dia, &hora, &minuto, &segundo, &centesimo, &idadeInfo);

    if (hora < 3){
      hora = 21 + hora;
      dia = dia - 1;
    }
     Serial.print("Horario (GMT-3): ");
     Serial.print(hora-3);
     Serial.print(":");
     Serial.print(minuto);
     Serial.print(":");
     Serial.print(segundo);
     Serial.print(":");
     Serial.println(centesimo);

     //velocidade
     float velocidade;
     velocidade = gps1.f_speed_kmph();   //km/h

     Serial.print("Velocidade (km/h): ");
     Serial.println(velocidade, 2);  //Conversão de Nós para Km/h
     addValor(velocidade);
     addValor(hora-3);
     addValor(minuto);
     addValor(segundo);
     addValor(latitude);
     addValor(longitude);
  }
}

void erro(){
  while(true){
    digitalWrite(LED_AVISO,HIGH);
    delay(1000);
    digitalWrite(LED_AVISO,LOW);
    delay(1000);
  }
}

void verificarSD(){
  if(!SD.begin()){
    Serial.println("Falha ao conectar com modulo SD");
    erro();
  }
  else{
    Serial.println("Cartao conectado");
  }
}

void criarPasta(){
  if(!SD.exists("/GPS")){
    Serial.println("Criando pasta dos dados");
    if(SD.mkdir("/GPS")){
      Serial.println("Pasta criada com sucesso");
    }
    else{
      Serial.println("Erro ao criar a pasta");
      erro();
    }
  }
  else{
    Serial.println("Pasta já existe");
  }
}

void criarArquivo(){
  if(!SD.exists(caminho)){
    File arquivo = SD.open(caminho,FILE_WRITE);
    if(arquivo){
      Serial.println("Arquivo Criado com Sucesso");
      arquivo.println("Vel,Hora,Min,Seg,Lat,Long");
      arquivo.close();
    }
    else{
      Serial.println("Erro ao criar arquivo");
      erro();
    }
  }
  else{
    Serial.println("Arquivo ja existe");
  }  
}

void addValor(float valor){
  if(SD.exists(caminho)){
    File arquivo = SD.open(caminho,FILE_APPEND);
    if(controle >= 4){
      arquivo.print(valor,6);
    }
    else{
      arquivo.print(valor);
    }
    arquivo.print(",");
    controle++;
    if(controle == 6){
      controle = 0;
      arquivo.println();
    }
    arquivo.close();
  }
  else{
    Serial.println("Erro ao armazenar os dados");
    erro();
  }
}