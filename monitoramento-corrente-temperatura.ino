/* 
 *  Exemplo de Utilização de Sensor de Corrente, Temperatura e Menu LCD
 *  
 *  Medição de Parâmetros de Equipamentos Eletroeletrônicos
 *  
 *  Autor: Yhan Christian Souza Silva - Data: 23/12/2015
 *    
 */
 #include <SoftwareSerial.h>
 #include <LiquidCrystal.h>
 #include "Wire.h"
 #include <DHT.h>
 #include <EmonLib.h>
 EnergyMonitor emon1;
 LiquidCrystal display_lcd(7, 6, 5, 4, 3, 2);
 SoftwareSerial bluetooth(0, 1);

 #define DS1307_ADDRESS 0x68
 #define Button_Up 12
 #define Button_Down 11
 #define Button_Default 10
 #define Button_On 9
 #define Definir_Tensao 8
 #define Carga A0
 #define Pino_Sensor A3
 #define Modelo_Sensor DHT11
 #define Current_Sensor 1
 #define Buzzer A2 

 DHT dht(Pino_Sensor, Modelo_Sensor);
 void changeMenu();
 void febrace();
 void dispMenu();
 void menuDefault();
 void date_hour();
 void temperature();
 void carga();
 void current();
 double voltage();

 char menu = 0x01;
 char set_1 = 0x00, set_2 = 0x00, set_3 = 0x00;
 boolean flag_up, flag_down, flag_default, flag_ligar, flag_ddp;
 double tensao;//Definindo tensão da rede

//Array simbolo grau
byte grau[8] = { 0x0C,0x12,0x12,0x0C,0x00, 0x00, 0x00, 0x00};
byte zero = (0x00);

void setup() 
{
  Wire.begin();
  //Pino, calibracao - Verificar calibração correta!
  emon1.current(Current_Sensor, 29);
  display_lcd.begin(16, 2); 
  //Serial.begin(9600);
//  bluetooth.begin(9600);

    // Entrada para botões habilitando resistor pull up
    for (char x = 8; x < 13; x++)
    {
      pinMode(x, INPUT_PULLUP);
    }
    pinMode(Carga, OUTPUT);
    digitalWrite(Carga, HIGH);
    pinMode(Buzzer, OUTPUT);
    flag_up = 0x00;
    flag_down = 0x00;
    flag_default = 0x00;
    flag_ligar = 0x00;
    flag_ddp = 0x00;
      // A linha abaixo pode ser retirada apos setar a data e hora - configura data e hora
     // SelecionaDataeHora(); 
}
    void loop()
    {
      dispMenu();
      changeMenu();
      menuDefault();
    }

// Desenvolvimento das funções auxiliares

void changeMenu()
//Função para mudança de menus pressionando up/down
{
  if (!digitalRead(Button_Up)) 
  {
    flag_up = 0x01;
    digitalWrite(Buzzer, 1);
  }
  if (!digitalRead(Button_Down))
  {
    flag_down = 0x01;
    digitalWrite(Buzzer, 1);
  }

  if(digitalRead(Button_Up) && flag_up)
  {
    flag_up = 0x00;
    display_lcd.clear();
    menu++;
    digitalWrite(Buzzer, 0);
    
    if ( menu > 0x06)
    {
      menu = 0x01;
    }
  }
  if(digitalRead(Button_Down) && flag_down)
  {
    flag_down = 0x00;
    display_lcd.clear();
    menu--;
    digitalWrite(Buzzer, 0);
    
    if ( menu < 0x01)
    {
      menu = 0x06;
    }
  }
}
void dispMenu()
// Função para exibição do menu conforme pressionar botões up/down de acordo com o case
{
 switch(menu)
 {

   case 0x01:
   febrace();
   break;
   case 0x02:
   date_hour();
   break;
   case 0x03:
   carga();
   break;
   case 0x04:
   voltage();
   break;
   case 0x05: 
   current();
   break;
   case 0x06:
   temperature();
   break;
 }  
}

void febrace()
// Criação de menu apenas p/ exibir informações de boas vindas
// Para o usuário 
{
  display_lcd.setCursor(4,0);
  display_lcd.print("Febrace");
  display_lcd.setCursor(3,1);
  display_lcd.print("Bem-Vindo ");
  delay(25);
}

void SelecionaDataeHora()
{
  byte seconds = 00;
  byte minutes = 29;
  byte hours = 11;
  byte day = 25;
  byte month = 10;
  byte years = 15;
  Wire.beginTransmission(DS1307_ADDRESS);
  Wire.write(zero);

  // Inserindo os valores no CI declarados nas variaveis acima
  Wire.write(ConverttoBCD(seconds));
  Wire.write(ConverttoBCD(minutes));
  Wire.write(ConverttoBCD(hours));
  Wire.write(ConverttoBCD(day));
  Wire.write(ConverttoBCD(month));
  Wire.write(ConverttoBCD(years));
  Wire.write(zero);
  Wire.endTransmission();  
}

byte ConverttoBCD(byte y)

{
  return ((y / 10 * 16) + (y % 10));
}

byte ConverttoDec(byte y)
{
  return((y / 16 * 10) + (y % 16));
}


void date_hour()
{
  //Função para exibir data e hora atual - sensor RTC 1307
  Wire.beginTransmission(DS1307_ADDRESS);
  Wire.write(zero);
  Wire.endTransmission();
  Wire.requestFrom(DS1307_ADDRESS, 7);
  int seconds = ConverttoDec(Wire.read());
  int minutes = ConverttoDec(Wire.read());
  int hours = ConverttoDec(Wire.read() & 0b111111);
  int day = ConverttoDec(Wire.read());
  int month = ConverttoDec(Wire.read());
  int years = ConverttoDec(Wire.read());
  display_lcd.setCursor(0, 0);
  display_lcd.print("Hora: ");
 

  if ( hours < 10)
  {
    display_lcd.print("0");
  }
  display_lcd.print(hours);
  display_lcd.print(":");

  if (minutes < 10)
  {
    display_lcd.print("0");
  }
  display_lcd.print(minutes);
  display_lcd.print(":");

  if(seconds < 10)
  {
    display_lcd.print("0");
  }
  display_lcd.print(seconds);

  display_lcd.setCursor(0, 1);
  display_lcd.print("Data: ");
 
  if(day < 10)
  {
    display_lcd.print("0");
  }
  display_lcd.print(day);
  display_lcd.print(".");

  if(month < 10)
  {
    display_lcd.print("0");
  }
  display_lcd.print(month);
  display_lcd.print(".");
  display_lcd.print(years);
}

void temperature()
{
  // Leitura de temperatura - e exibição no display LCD
  display_lcd.createChar(0, grau);
  float temp = dht.readTemperature();
  display_lcd.setCursor(0,0);
  display_lcd.print("Temp : ");
  display_lcd.print(" ");
  display_lcd.setCursor(7,0);
  display_lcd.print(temp, 1);
  display_lcd.setCursor(12,0);
  display_lcd.write((byte)0);
  delay(25);
}

void carga()
// Função para controle de carga - COM LÓGICA INVERSA LOW - LIGA | HIGH - DESLIGA
{
  display_lcd.setCursor(5,0);
  display_lcd.print("Status"); 
  
  if(!digitalRead(Button_On))
  {
    flag_ligar = 0x01;
    digitalWrite(Buzzer, 1);
  }
  if(digitalRead(Button_On) && flag_ligar)
  {
    flag_ligar = 0x00;
    set_1++;
    digitalWrite(Buzzer, 0);
  }
  if (set_1 > 2)
  {
    set_1 = 0x01;
  }
  switch(set_1)
  {
    case 0x01:
    display_lcd.setCursor(0,1);
    display_lcd.print("Equipamento ON ");
    digitalWrite(Carga,LOW);
    break;
    case 0x02:
    display_lcd.setCursor(0,1);
    display_lcd.print("Equipamento OFF");
    digitalWrite(Carga,HIGH);
    break;  
  }
}

double voltage()
// Função para definir tensão de entrada
{
  display_lcd.setCursor(0,0);
  display_lcd.print("Tensao Carga");
  if(!digitalRead(Definir_Tensao))
  {
    flag_ddp = 0x01;
    digitalWrite(Buzzer, 1);
  }
  if(digitalRead(Definir_Tensao) && flag_ddp)
  {
    flag_ddp = 0x00;
    set_3++;
    digitalWrite(Buzzer, 0);
  }
  if(set_3 > 2)
  {
    set_3 = 0x01;
  }
  switch(set_3)
  {
    case 0x01:
    tensao = 110;
    display_lcd.setCursor(0,1);
    display_lcd.print("Tensao 110V");
    break;
    case 0x02:
    tensao =220;
    display_lcd.setCursor(0,1);
    display_lcd.print("Tensao 220V");
    break;
    display_lcd.clear();
    return tensao;
  }
}


void current()
{
  // Função para leitura da corrente elétrica e calculo de potência
  double Irms = emon1.calcIrms(1480); 
  display_lcd.setCursor(0,0);
  display_lcd.print("Corr. (A): " );
  display_lcd.print(Irms);
  display_lcd.setCursor(1,1);
  display_lcd.print("Pot (W): ");
  display_lcd.print(Irms * tensao, 0);
  delay(25);
}

void menuDefault()
{
    //Passagem pelos menus hora e data, temperatura e corrente
    if(!digitalRead(Button_Default))
    {
      flag_default = 0x01;
      delay(25);
      digitalWrite(Buzzer, 1);
    }
    if(digitalRead(Button_Default) && flag_default)
    {
     for (char y = 3; y > 0; y--)
     {
      if (y > 0)
      {
        digitalWrite(Buzzer, 0);
        display_lcd.clear();
        delay(25);
        flag_default = 0x00;
        set_2 = 0x01;
        date_hour();
        delay(1000);
        display_lcd.clear();
        delay(25);
        temperature();
        delay(1000);  
        display_lcd.clear();
        delay(25); 
        current();
        delay(1000);
      }
      display_lcd.clear();
    }
  }
}
