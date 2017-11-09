
#include <OneWire.h>
#include <Wire.h>
#include <TimerOne.h>
#include <DS3231.h>


DS3231 Reloj;


#define DIGITO1 Digitos[3]
#define DIGITO2 Digitos[2]
#define DIGITO3 Digitos[1]
#define DIGITO4 Digitos[0]

#define RAYA  0x40
#define GRADO B10100011
#define DP    B10000000

const int SETMODO = 2; // Button SET MENU'
const int SETMAS = 3; // Button +
const int SETMENOS = 4; // Button -
const int SETALARMA = 5; // SWITCH Alarm
const int led       = 13;

const byte DIGIT_COUNT = 4;

int str4094 = 6;
int clk4094 = 7;
int data4094 = 8;

uint8_t dato[] = {0,};
byte Digitos[4];

uint8_t hora = 17;
uint8_t minu = 8;
uint8_t sec = 0;



byte diaA1, horaA1, minA1, secA1, bitsA1;
byte diaA2, horaA2, minA2, bitsA2;
bool dyA1, dyA2, a12h, a1pm;
bool h12 = false;
bool PM = false;


boolean bSec = false;
uint8_t modo = 0;
boolean bSetAlarma = false;
boolean bAlarmaONOFF = false;
boolean bUpdate = false;
boolean bVisu = true;


float temperatura = 0.0;
boolean bLeeT = false;

unsigned char digits[11] = {B00111111, B00000110, B01011011, B01001111, B01100110, B01101101, B01111101,
                            B00100111, B01111111, B01101111, B00000000
                           };
unsigned char digits_inv[11] = {B00111111, B00000110, B10011011, B10001111, B10100110, B10101101,
                                B11111101, B10000111,  B11111111, B11101111, B10000000
                               };

//*******************************************************************************
uint8_t bin2bcd(uint8_t bin)
{
  return (bin / 10 * 16) + (bin % 10);
}


//**************************************************************************************

byte numToDigit(byte num)
{

  return (digits[num & 0x0F]);
}
//*^***********************************************************************************
byte numToDigitInv(byte num)
{

  return (digits_inv[num & 0x0F]);
}
//***************************************************************************************
void visuHora()
{

  byte i, iPrimerDigito;

  iPrimerDigito = (bin2bcd(hora) & 0xF0) >> 4;

  if (!iPrimerDigito)
    DIGITO1 = 0;
  else
    DIGITO1 = numToDigit(iPrimerDigito);

  if (!bSec)
  {
    DIGITO2 = numToDigit(bin2bcd(hora) & 0x0F) | DP;
    DIGITO3 = numToDigitInv((bin2bcd(minu) & 0xF0) >> 4) | B01000000;
  }
  else
  {
    DIGITO2 = numToDigit(bin2bcd(hora) & 0x0F);
    DIGITO3 = numToDigitInv((bin2bcd(minu) & 0xF0) >> 4);

  }

  DIGITO4 = numToDigit(bin2bcd(minu) & 0x0F);

  for (i = 0; i < 4; i++) {
    digitalWrite(str4094, HIGH); //Pull latch LOW to send data
    shiftOut(data4094, clk4094, MSBFIRST, Digitos[i]); //Send the data
    digitalWrite(str4094, LOW); // Pull latch HIGH to stop sending data
  }
}
//****************************************************************************
void VisuHoraProg(byte Modo) {

  int i;

  switch (Modo) {

    case 0: DIGITO1 = numToDigit((bin2bcd(hora) & 0xF0) >> 4);
            DIGITO2 = numToDigit(bin2bcd(hora) & 0x0F);
            DIGITO3 = 0;
            DIGITO4 = 0;
      break;
    case 1:
      DIGITO1 = numToDigit((bin2bcd(horaA1) & 0xF0) >> 4);
      DIGITO2 = numToDigit(bin2bcd(horaA1) & 0x0F);
      DIGITO3 = B10110111;  //letra A
      DIGITO4 = B00000110;  //1
      break;

    case 2:
      DIGITO1 = numToDigit((bin2bcd(horaA2) & 0xF0) >> 4);
      DIGITO2 = numToDigit(bin2bcd(horaA2) & 0x0F);
      DIGITO3 = B10110111;  //letra A
      DIGITO4 = B01011011;  //2
      break;

    default: break;

  }
  for (i = 0; i < 4; i++) {
    digitalWrite(str4094, HIGH); //Pull latch LOW to send data
    shiftOut(data4094, clk4094, MSBFIRST, Digitos[i]); //Send the data
    digitalWrite(str4094, LOW); // Pull latch HIGH to stop sending data
  }
}
//*****************************************************************************
void VisuMinProg(byte bModo) {

  int i;

  switch (bModo) {

    case 0: DIGITO1 = 0;
      DIGITO2 = 0;
      DIGITO3 = numToDigitInv((bin2bcd(minu) & 0xF0) >> 4);
      DIGITO4 = numToDigit(bin2bcd(minu) & 0x0F);
      break;

    case 1:

      DIGITO1 = B01110111;  //letra A
      DIGITO2 = B00000110;  //1
      DIGITO3 = numToDigitInv((bin2bcd(minA1) & 0xF0) >> 4);
      DIGITO4 = numToDigit(bin2bcd(minA1) & 0x0F);;
      break;

    case 2:

      DIGITO1 = B01110111;  //letra A
      DIGITO2 = B01011011;  //2
      DIGITO3 = numToDigitInv((bin2bcd(minA2) & 0xF0) >> 4);
      DIGITO4 = numToDigit(bin2bcd(minA2) & 0x0F);;
      break;

    default: break;
  }

  for (i = 0; i < 4; i++) {
    digitalWrite(str4094, HIGH); //Pull latch LOW to send data
    shiftOut(data4094, clk4094, MSBFIRST, Digitos[i]); //Send the data
    digitalWrite(str4094, LOW); // Pull latch HIGH to stop sending data
  }
}
//***************************************************************************
void visuTemperatura(float temperatura)
{
  int i;
  int temp = (int)temperatura;
  DIGITO1 = numToDigit((bin2bcd(temp) & 0xF0 ) >> 4);
  DIGITO2 = numToDigit(bin2bcd(temp) & 0x0F );
  DIGITO3 = GRADO;
  DIGITO4 = B00111001;

  for (i = 0; i < 4; i++) {
    digitalWrite(str4094, HIGH); //Pull latch LOW to send data
    shiftOut(data4094, clk4094, MSBFIRST, Digitos[i]); //Send the data
    digitalWrite(str4094, LOW); // Pull latch HIGH to stop sending data
  }

}


//***************************************************************************
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  Wire.begin();
  pinMode(str4094, OUTPUT);
  pinMode(clk4094, OUTPUT);
  pinMode(data4094, OUTPUT);
  pinMode(SETMODO, INPUT);
  pinMode(SETMAS, INPUT);
  pinMode(SETMENOS, INPUT);
  pinMode(SETALARMA, INPUT);
  pinMode(led, OUTPUT);


  Timer1.initialize(400000);//timing for 500ms
  Timer1.attachInterrupt(TimingISR);//declare the interrupt serve routine:TimingIS

  hora = Reloj.getHour(h12, PM);
  minu = Reloj.getMinute();
  temperatura = Reloj.getTemperature();

  Reloj.getA1Time (diaA1, horaA1, minA1, secA1, bitsA1, dyA1, a12h, a1pm);
  Reloj.getA2Time (diaA2, horaA2, minA2,  bitsA1, dyA2, a12h, a1pm);
  digitalWrite(led, LOW);
}
//***************************************************************************


void loop() {
  // put your main code here, to run repeatedly:

  if (Reloj.checkIfAlarm(1) )
    Serial.println("!!!!!!!!!!Alarma 1 !!!!!!!!!!!");

  if ( Reloj.checkIfAlarm(2))
    Serial.println("!!!!!!!!!!Alarma 2 !!!!!!!!!!!");
    
  if (bUpdate)
  {
    hora = Reloj.getHour(h12, PM);
    minu = Reloj.getMinute();


    bUpdate = false;
  }




  if (digitalRead(SETMODO) == LOW) {

    if (++modo > 6 )
      modo = 0;
    delay(100);
  }

  if (digitalRead(SETMAS) == LOW) {

    switch (modo)
    {
      case 0: bAlarmaONOFF = 0;
        break;
      case 1: if (++hora >= 24)
          hora = 0;
        Reloj.setHour(hora);
        break;
      case 2: if (++minu >= 59)
          minu = 0;
        Reloj.setMinute(minu);
        break;
      case 3: if (++horaA1 >= 24)
          horaA1 = 0;
        break;
      case 4: if (++minA1 >= 59)
          minA1 = 0;
        break;

      case 5: if (++horaA2 >= 24)
          horaA2 = 0;
        break;
      case 6: if (++minA2 >= 59)
          minA2 = 0;
        break;

      default: break;

    }

    delay(100);
  }

  if (digitalRead(SETMENOS) == LOW) {

    switch (modo)
    {
      case 0: bAlarmaONOFF = 0;
        break;
      case 1: if (--hora == 0xFF)
          hora = 24;
        Reloj.setHour(hora);
        break;
      case 2: if (--minu == 0xFF)
          minu = 59;
        Reloj.setMinute(minu);
        break;
      case 3: if (--horaA1 == 0xFF)
          horaA1 = 24;
        break;
      case 4: if (--minA1 == 0xFF)
          minA1 = 59;
        break;
      case 5: if (--horaA2 == 0xFF)
          horaA2 = 24;
        break;
      case 6: if (--minA2 == 0xFF)
          minA2 = 59;
        break;

      default: break;

    }

    delay(100);
  }

  if (digitalRead(SETALARMA) == LOW) {

    bSetAlarma = !bSetAlarma;

    if (bSetAlarma) {
      Serial.print("Alarma 1: Hora: ");
      Serial.print(horaA1);
      Serial.println(minA1);
      Serial.print("Alarma 2: Hora: ");
      Serial.print(horaA2);
      Serial.println(minA2);
      Reloj.setA1Time(0, horaA1, minA1, 0, 0b1000, false, false, false);
      Reloj.setA2Time(0, horaA2, minA2, 0b100, false, false, false);
      Reloj.turnOnAlarm(1);
      Reloj.turnOnAlarm(2);

      Serial.print("Alarma Enabled ");
      Serial.print(Reloj.checkAlarmEnabled(1));
      Serial.println(Reloj.checkAlarmEnabled(2));
      digitalWrite(led, HIGH);
    }
    else {
      Serial.println("Alarma off");
      Reloj.turnOffAlarm(1);
      Reloj.turnOffAlarm(2);
      Serial.print("Alarma Enabled ");
      Serial.print(Reloj.checkAlarmEnabled(1));
      Serial.println(Reloj.checkAlarmEnabled(2));
      digitalWrite(led, LOW);
    }

    delay(100);
  }

  switch (modo)
  {
    case 0: if (bVisu)
        visuHora();
      else
        visuTemperatura(temperatura);
      break;
    case 1: VisuHoraProg(0); break;
    case 2: VisuMinProg(0); break;
    case 3: VisuHoraProg(1); break;
    case 4: VisuMinProg(1); break;
    case 5: VisuHoraProg(2); break;
    case 6: VisuMinProg(2); break;
    default: break;

  }

  if (bLeeT) {


    temperatura = Reloj.getTemperature();
    bLeeT = false;
  }


  delay(200);

}

//*******************************************************************************************

void TimingISR()
{

  static byte cntTemp = 0;
  static byte cntTemperatura = 0;
  static byte cntLeeTemperatura = 0;

  if (++cntTemp > 2)  //1 segundo
  {


    bUpdate = !bUpdate;
    bSec = !bSec;
    cntTemp = 0;
  }

  if (++cntTemperatura > 20)  //5 segundos
  {
    bVisu = !bVisu;
    cntTemperatura = 0;
  }

  if (++cntLeeTemperatura > 100)  //5 min
  {
    bLeeT = !bLeeT;
    cntLeeTemperatura = 0;
  }

}

