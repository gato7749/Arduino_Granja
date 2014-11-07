#include <Wire.h>
//#include <Fat16.h>
//#include <Fat16util.h> 

#include <LiquidCrystal_I2C.h>
#include <DS1307.h>
#include <Shifter.h>
#include <avr/pgmspace.h>

#include <SD.h>

//SdCard card;
//Fat16 file;

//-----------------MENSAGES------------------------
prog_char string_0[] PROGMEM = "*****BIENVENIDO*****";   // "String 0" etc. son strings a guardar.
prog_char string_1[] PROGMEM = "Configuracion";
prog_char string_2[] PROGMEM = "Inicial";
prog_char string_3[] PROGMEM = "Inserte PIN MAESTRO:";
prog_char string_4[] PROGMEM = "SUPER!";
prog_char string_5[] PROGMEM = "ERROR!";
prog_char string_6[] PROGMEM = "Configurar";
prog_char string_7[] PROGMEM = "Password de Usuario:";
prog_char string_8[] PROGMEM = "Numero de Telefono";
prog_char string_9[] PROGMEM = "Hora:";
prog_char string_10[] PROGMEM = ":";
prog_char string_11[] PROGMEM = "Fecha:";
prog_char string_12[] PROGMEM = "/";
prog_char string_13[] PROGMEM = "Parametros de";
prog_char string_14[] PROGMEM = "Crianza";
prog_char string_15[] PROGMEM = "1:";
prog_char string_16[] PROGMEM = "2:";
prog_char string_17[] PROGMEM = "Automaticos";
prog_char string_18[] PROGMEM = "Temperatura";
prog_char string_19[] PROGMEM = "Maxima:";
prog_char string_20[] PROGMEM = "C";
prog_char string_21[] PROGMEM = "Minima:";
prog_char string_22[] PROGMEM = "Alimentacion";
prog_char string_23[] PROGMEM = "Alimentar";
prog_char string_24[] PROGMEM = "cada";
prog_char string_25[] PROGMEM = "XX";
prog_char string_26[] PROGMEM = "horas";
prog_char string_27[] PROGMEM = "en";
prog_char string_28[] PROGMEM = "especificas";
prog_char string_29[] PROGMEM = "Periodo de";
prog_char string_30[] PROGMEM = "Horarios de";
prog_char string_31[] PROGMEM = "a las:";
prog_char string_32[] PROGMEM = "Si, ";
prog_char string_33[] PROGMEM = "No, ";
prog_char string_34[] PROGMEM = "Ok, ";
prog_char string_35[] PROGMEM = "ERROR!";
prog_char string_36[] PROGMEM = "Inserte SD Card";
prog_char string_37[] PROGMEM = "Revise el Modulo GSM";
prog_char string_38[] PROGMEM = "AT+CMGF=1";
prog_char string_39[] PROGMEM = "AT+CIPSPRT=0";
prog_char string_40[] PROGMEM = "AT+CPMS=\"SM\",\"SM\",\"SM\"";
prog_char string_41[] PROGMEM = "at+CNMI=0,3,0,0";
prog_char string_42[] PROGMEM = "Alarma, alimento";
prog_char string_43[] PROGMEM = "insuficiente";
prog_char string_44[] PROGMEM = "                    ";

//---------------74hc595----------------
#define SER_Pin 9 //SER_IN
#define RCLK_Pin 7 //L_CLOCK
#define SRCLK_Pin 8 //CLOCK

#define NUM_REGISTERS 2 //how many registers are in the chain

//initaize shifter using the Shifter library
Shifter shifter(SER_Pin, RCLK_Pin, SRCLK_Pin, NUM_REGISTERS); 
//--------------------------------------------------

//------------Temperatura------------
#define DEV_ID 0x90 >> 1 // Esta es la direccion I2C del ds1621 (sensor de temperatura)
byte tempC;
byte tempx;
//--------------------------------------------
//----------------SD Card-----------------------------
byte ventilad;
byte calefac;
byte logSMS;
byte loginf;
byte logconfig;
byte horalog;
byte fechalog;
byte alarmaAlimento;
byte alarmaDesechos;

const int chipSelect = 53;
File SDFileData;


//#define RTTY_ASCII      7       // ASCII set for RTTY (7/8bit)
//#define GPSRATE         9600    // GPS baud rate
//#define BUFFERSIZE      90      // How many bytes of input to buffer from the GPS?
//#define ENDLN           "\r\n"  // SD
//------------------------------------------------------
//-------------Configuracion LCD por I2C----------------------
LiquidCrystal_I2C lcd(0x38,20,4); //
//-------------------------------------------------------------

//------------------clok---------------
int rtc[7];
byte second, minute, hour, dayOfWeek, dayOfMonth, month, year;
byte test;
//-----------------------------------
//------------IMAGENES VENTILADOR CALEFACTOR----------------
byte happy[8] = {
  0,19,26,4,11,25,0,0};
byte saddy[9] = {
  30,1,14,16,14,1,30,0};
//_--------------------------------------------------------

byte tmax;
byte tmin;
byte per;
byte alim1h=0;
byte alim1m=0;
byte alim2h=0;
byte alim2m=0;
byte alim3h=0;
byte alim3m=0;
byte secretCode[4] = {
  1, 3, 7, 9};
byte secretCode2[4] = {
  0, 0, 0, 0};
byte pasuser[4] = {
  0, 0, 0, 0};
byte telef[8] =  {
  0, 0, 0, 0, 0, 0, 0};
byte horas[4] = {
  0, 0, 0, 0};
byte fechas[6] = {
  0, 0, 0, 0, 0, 0};
byte tempmax[2] = {
  0, 0};
byte tempmin[2] = {
  0, 0};
byte period[2] = {
  0, 0};
byte horario[4] = {
  0, 0, 0, 0};
byte keyboardPin = 0;    // Analog input pin that the keypad is attached to
int keyboardValue = 0;

byte key;

byte pos;
boolean actalim;
byte alimrec;
byte nocard;
int cnt;

//-----------------variables PDU-----------------


char telefono[10];
char mensaje[70];

int8_t answer;
int8_t respuesta;
char SMS[200];//+++++
//char asc_msg[50];
byte res;
char aux_string[30];
char SMSrec[30];
char numTelef[8];
char buff[30];
char SMS_text[200];
//char buff[30];//DEBUG CHAR
//char smsbuffer[30];
char n[11];
char phone_num[8]; // array for the phone number string
//----------------------------------------------------

char buffer[30];    // debe ser tan grande como el string más grande.
boolean gsmBegin=false;

//-----------------DECLARACION DE MENSAGES PARA LCD----------------
PROGMEM const char *string_table[] =   
{   
  string_0,
  string_1,
  string_2,
  string_3,
  string_4,
  string_5,
  string_6,
  string_7,
  string_8,
  string_9,
  string_10,
  string_11,
  string_12,
  string_13,
  string_14,
  string_15,
  string_16,
  string_17,
  string_18,
  string_19,
  string_20,
  string_21,
  string_22,
  string_23,
  string_24,
  string_25,
  string_26,
  string_27,
  string_28,
  string_29,
  string_30,
  string_31,
  string_32,
  string_33,
  string_34,
  string_35,
  string_36,
  string_37,
  string_38,
  string_39,
  string_40,
  string_41,
  string_42,
  string_43,
  string_44
};
void escribe(byte linea, byte posicion, byte palabra)
{

  lcd.setCursor(posicion,linea);

  strcpy_P(buffer, (char*)pgm_read_word(&(string_table[palabra]))); // Casts necesarios e inferencia
  //Serial.println( buffer );
  lcd.print(buffer);
}

//--------------------------clok-------------------------------

void setDateDs1307() { 
  // Uso de (byte) tipo de conversión ASCII y matemáticas para alcanzar el resultado.  


  RTC.stop();
  RTC.set(DS1307_SEC,0);
  RTC.set(DS1307_MIN,((horas[2] * 10) + horas[3]));
  RTC.set(DS1307_HR,((horas[0] * 10) + horas[1]));
  RTC.set(DS1307_DOW,2);
  RTC.set(DS1307_DATE,((fechas[0] * 10) + fechas[1]));
  RTC.set(DS1307_MTH,((fechas[2] * 10) + fechas[3]));
  RTC.set(DS1307_YR,((fechas[4] * 10) + fechas[5]));
  RTC.start();
}

//Extrae la fecha y el tiempo del ds1307 e imprime el resultado
void getDateDs1307(){ 

  RTC.get(rtc,true);

  second     = rtc[0];
  minute     = rtc[1];
  hour       = rtc[2];
  dayOfWeek  = rtc[3];
  dayOfMonth = rtc[4];
  month      = rtc[5];
  year       = rtc[6];


  lcd.setCursor(0,0);
  if (hour < 10) lcd.print("0"); 
  lcd.print(hour, DEC);
  //lcd.print(":");
  lcd.setCursor(3,0);
  if (minute < 10) lcd.print("0"); 
  lcd.print(minute, DEC);
}

void inic1621(){
  //----------Iniciar ds1621----------------
  Wire.begin();
  Wire.beginTransmission(DEV_ID);           // coneccion con el DS1621 (#0)
  Wire.write(0xAC);                          // acceso a la configuracion
  Wire.write(0x02);                          // configurar en lectura continua
  Wire.beginTransmission(DEV_ID);           // reiniciar
  Wire.write(0xEE);                          // iniciar lectura
  Wire.endTransmission();
  //------------------------------------------------- 
  Wire.begin();
  //-----------------------------------------------
}

int get1621(){
  //tempC = 0;
  delay(1000);                              //tiempo para responder
  Wire.beginTransmission(DEV_ID);
  Wire.write(0xAA);                          // Leer temperatura
  Wire.endTransmission();
  Wire.requestFrom(DEV_ID, 1);              // requiere un byte desde el DS1621
  tempx = Wire.read();                   // leer la temperatura en grados
  if (tempx>100) tempx=tempC;
  tempC = tempx;

}

void setup()
{
  Serial.begin(2400);

  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);  
  
  pinMode(4, INPUT);
  pinMode(5, INPUT);
  pinMode(6, INPUT);

  pinMode(53, OUTPUT);
  
  //--------------Temperatura--------------------
  inic1621();

  //----------Clear REALYS----------------------------
  shifter.clear(); //set all pins on the shift register chain to LOW
  shifter.write(); //send changes to the chain and display them

  //-----------CREAR CARACTERES ESPECIALES---------------
  lcd.createChar(0, happy);
  lcd.createChar(1, saddy);
  //----------------------------------------------------  

  shifter.setPin(14, HIGH);//LCD back ligth ON
  shifter.write();
  lcd.init();                      // initialize the lcd

  escribe(0,0,0);      //BIENVENIDO
  escribe(1,4,1);      //Configuracion
  escribe(2,7,2);      //Inicial

  initSD();
  shifter.setPin(15, LOW); //Modulo SD Card Off
  shifter.write();

  power_on();

  delay(3000);
 /* 
  sendATcommand("AT+CMGF=1", "OK", 1000);    // sets the SMS mode to text
  sendATcommand("AT+CIPSPRT=0", "OK", 1000); //Prompt OFF
  sendATcommand("AT+CPMS=\"SM\",\"SM\",\"SM\"", "OK", 1000);    // selects the memory
  sendATcommand("AT+CNMI=0,3,0,0", "OK", 1000); 
 */ 
 
  strcpy_P(buffer, (char*)pgm_read_word(&(string_table[38])));
  sendATcommand(buffer, "OK", 1000);    // sets the SMS mode to text
    
  strcpy_P(buffer, (char*)pgm_read_word(&(string_table[39])));
  //sendATcommand("AT+CIPSPRT=0", "OK", 1000); //Prompt OFF
  sendATcommand(buffer, "OK", 1000); //Prompt OFF
  
  strcpy_P(buffer, (char*)pgm_read_word(&(string_table[40])));    
  //sendATcommand("AT+CPMS=\"SM\",\"SM\",\"SM\"", "OK", 1000);    // selects the memory
  sendATcommand(buffer, "OK", 1000); //Prompt OFF    
  
  strcpy_P(buffer, (char*)pgm_read_word(&(string_table[41])));    
  //sendATcommand("at+CNMI=0,3,0,0", "OK", 1000);
  sendATcommand(buffer, "OK", 1000); //Prompt OFF    

  delay(5000);

  //-----------------------------------------------------------------------  
  configinic();

  displayinic();

  crearFile();
  
  fechalog=RTC.get(DS1307_DATE,false);

  actalim=0;
  
}

void loop()

{ 

  getDateDs1307();
  get1621();

  delay(300);                              // tiempo para responder
  lcd.setCursor(3,1);
  //lcd.print(EEPROM.read(49));
  lcd.print(tempC);
  delay(500);                    


  respuesta = sendATcommand("AT+CMGR=1", "+CMGR:", 2000);    // reads the first SMS
  if (respuesta == 1)
  {
    comparaSMSs();
    answer=0;
    delay(2000);
    respuesta=0;

  }



  //-------------------CONTROL DE TEMPERATURA-----------------
  if (tempC > tmax){
    //if (EEPROM.read(49) > tmax){  
    lcd.setCursor(12,1);   
    lcd.write(byte(8));
    ventilad = 1;
    shifter.setPin(0, HIGH);// Ventilador
  }
  else {
    lcd.setCursor(12,1); 
    lcd.print(' ');
    ventilad = 0;
    shifter.setPin(0, LOW);// Ventilador   
  }

  if (tempC < tmin){
    //if (EEPROM.read(49) < tmin){
    lcd.setCursor(13,1);
    lcd.write(byte(9));
    calefac = 1;
    shifter.setPin(1, HIGH);// Calefactor
  }
  else {
    lcd.setCursor(13,1); 
    lcd.print(' ');
    calefac=0;
    shifter.setPin(1, LOW);// Calefactor
  }
  //-------------------------------------------------------------------
  //-----------------CONTROL DE ALIMENTO------------------------------

  if (actalim==1){
    if (minute!=alimrec){
      actalim=0;
    }
  } 

  if (actalim==0){     
    if (alim1h!=0 && alim1m!=0){     
      if (hour==alim1h && minute==alim1m){
        Alimentar(3); 
        actalim=1;  
        alimrec=minute;
      }
    }

    if (alim2h!=0 && alim2m!=0){                
      if (hour==alim2h && minute==alim2m){
        Alimentar(3); 
        actalim=1;  
        alimrec=minute;
      }
    }

    if (alim3h!=0 && alim3m!=0){                
      if (hour==alim3h && minute==alim3m){
        Alimentar(3); 
        actalim=1;  
        alimrec=minute;
        if (per!=0){
          alimperiodo(per);
        }
      }
    }  
  }

  //---------------------------------------------------------------   

  //--------------------------------------------------------------------

  shifter.write(); //send changes to the chain and display them
  
  
  if ((RTC.get(DS1307_MIN,false))==1 && horalog==0){
    horalog=1;
    revisaAlimento();
    revisaDesechos();
    logDatos();
  }
  else if((RTC.get(DS1307_MIN,false))!=1) horalog=0;
  
  if (fechalog!=RTC.get(DS1307_DATE,false)){
    fechalog=RTC.get(DS1307_DATE,false);
    crearFile();
  }

}

//----------------CONFIGURACION INICIAL----------------------------------

void configinic(){
  lcd.clear();
  escribe(0,0,3);  //Inserte PIN MAESTRO

  for (pos = 0; pos < 4; pos++ ){

    keyboardValue = analogRead(keyboardPin); // read the keyboard value (0 - 1023)
    while (keyboardValue < 25){
      keyboardValue = analogRead(keyboardPin); 
      delay(50);

    }
    readkeyboard(); 
    secretCode2[pos]= key;
    // EEPROM.write((pos+13), key);
    lcd.setCursor(pos,1);
    lcd.print("*");
    //lcd.setCursor(pos,2);
    //lcd.print(key);

  }
  if (secretCode2[0] == secretCode[0] && secretCode2[1] == secretCode[1] && secretCode2[2] == secretCode[2] && secretCode2[3] == secretCode[3]){    
    //if (EEPROM.read(13) == EEPROM.read(0) && EEPROM.read(14) == EEPROM.read(1) && EEPROM.read(15) == EEPROM.read(2) && EEPROM.read(16) == EEPROM.read(3)){
    escribe(1,0,4);           //SUPER
    delay(1000);
    pswrduser(); //Configurar pasword de usuario.
    numtlf();    //Configurar numero de telefono predeterminado para mandar informes SMS
    hora();      //Configurar la hora
    fecha();     //Configurar  la fecha
    
    do{
    delay(1000);
    setDateDs1307();  //Iniciar reloj en tiempo real
    delay(500);
    RTC.get(rtc,true);
    hour       = rtc[2];
    delay(500);   
    Serial.print(hour,DEC);
    Serial.println(((horas[0] * 10) + horas[1]));
    }while(hour!=((horas[0] * 10) + horas[1]));

    parametros();  //Configurar parametros de crianza
  }

  else {
    lcd.setCursor(0,1);
    escribe(1,0,5);           //ERROR
    delay(1000);  
    configinic();
  }  

}
//-------------------------------------------------------------------------

//-----------------CONFIG PASWORD USUARIO-----------------------------------
void pswrduser()
{
  lcd.clear();
  escribe(0,5,6);           //Configure
  escribe(1,0,7);           //Password de Usuario
  pos=0;
  key=0;

  while (key != 11){

    lcd.setCursor(pos,1);

    keyboardValue = analogRead(keyboardPin); // Leer el valor de la tecla presionada
    while (keyboardValue < 25){              //Si no hay tecla precionada
      keyboardValue = analogRead(keyboardPin);  //espera a que se precione una tecla
      delay(50);
    }
    readkeyboard();      

    if (key == 10){
      teclaborrar(0, 1, 2, 3, 0, 0, 0, 0, 2);
    }

    else if (key!= 11){
      byte poss[]={
        0, 1, 2, 3, 3                  };
      pasuser[pos] = key;
      lcd.setCursor(poss[pos],2);  
      lcd.print(pasuser[pos]);
    }
    pos++;
    if (pos>4){ 
      pos=4; 
    }
  }

  lcd.clear();
  for (int i = 0; i <= 3; i++) {
    lcd.print(pasuser[i]);
  }
  /*
lcd.print(pasuser[0]);
   lcd.print(pasuser[1]);
   lcd.print(pasuser[2]);
   lcd.print(pasuser[3]);
   */
  delay(1000);

}

//-----------------------------------------------------------------------------

//----------------------------CONFIG NUMERO DE TLF------------------------------

void numtlf(){

  lcd.clear();
  escribe(0,5,6);           //Configure
  escribe(1,0,8);           //Numero de Telefono
  pos=0;
  key=0;

  while (key != 11){

    lcd.setCursor(pos,1);

    keyboardValue = analogRead(keyboardPin); // Leer el valor de la tecla presionada
    while (keyboardValue < 25){              //Si no hay tecla precionada
      keyboardValue = analogRead(keyboardPin); //espera a que se precione una tecla
      delay(50);
    }
    readkeyboard();      

    if (key == 10){
      teclaborrar(0, 1, 2, 3, 4, 5, 6, 7, 2);
    }

    else if (key!= 11){
      byte poss[]={
        0, 1, 2, 3, 4, 5, 6, 7, 7  };
      telef[pos] = key;
      lcd.setCursor(poss[pos],2);    
      lcd.print(telef[pos]);    
    }
    pos++;
    if (pos>=8){ 
      pos=8; 
    }
  }

  lcd.clear();
  for (int i = 0; i <= 7; i++) {
    lcd.print(telef[i]);
  }
  //lcd.print(telef[0]);
  //lcd.print(telef[1]);
  //lcd.print(telef[2]);
  //lcd.print(telef[3]);
  //lcd.print(telef[4]);
  //lcd.print(telef[5]);
  //lcd.print(telef[6]);
  //lcd.print(telef[7]);
  delay(3000);

}

//--------------------------------------------------------------------------------

//-------------------------CONFIGURAR LA HORA-------------------------------

void hora(){

  lcd.clear();
  escribe(0,5,6);           //Configure
  escribe(1,0,9);           //Hora:
  escribe(2,2,10);           //:    

  pos=0;
  key=0;

  while (key != 11){

    lcd.setCursor(pos,1);

    keyboardValue = analogRead(keyboardPin); // Leer el valor de la tecla presionada
    while (keyboardValue < 25){              //Si no hay tecla precionada
      keyboardValue = analogRead(keyboardPin); //espera a que se precione una tecla
      delay(50);
    }        
    readkeyboard();      

    if (key == 10){
      teclaborrar(0, 1, 3, 4, 0, 0, 0, 0, 2);
    }

    else if(key!= 11){
      byte poss[]={
        0, 1, 3, 4, 4                  };
      horas[pos] = key;
      lcd.setCursor(poss[pos],2);
      //-----------------LIMITADORES DE HORA-----------------------
      if (pos==0 && key>2){
        horas[pos]=2;
      }
      if (horas[0]==2 && pos==1 && key>3){
        horas[pos]=3;
      }
      if (pos==2 && key>5){
        horas[pos]=5;
      }
      //---------------------------------------------------------         
      lcd.print(horas[pos]);
    }
    pos++;
    if(pos>4){ 
      pos=4; 
    }

  }
  /*
lcd.clear();
   lcd.print(horas[0]);
   lcd.print(horas[1]);
   lcd.print(":");
   lcd.print(horas[2]);
   lcd.print(horas[3]);
   delay(3000);  
   */

}
//------------------------------------------------------------------------------

//-------------------------CONFIGURAR LA FECHA-------------------------------

void fecha(){
  lcd.clear();
  escribe(0,5,6);           //Configure
  escribe(1,0,11);           //Fecha:
  escribe(2,2,12);           //  /
  escribe(2,5,12);           //  /

  pos=0;
  key=0;

  while (key != 11){
    lcd.setCursor(pos,2);

    keyboardValue = analogRead(keyboardPin); // Leer el valor de la tecla presionada
    while (keyboardValue < 25){              //Si no hay tecla precionada
      keyboardValue = analogRead(keyboardPin); //espera a que se precione una tecla
      delay(50);
    }        
    readkeyboard();      

    if (key == 10){
      teclaborrar(0, 1, 3, 4, 6, 7, 0, 0, 2);
    }
    else if (key!= 11){
      byte poss[]={
        0, 1, 3, 4, 6, 7, 7                  };
      fechas[pos] = key;
      lcd.setCursor(poss[pos],2);
      //-------------------LIMITADORES DE FECHA-------------------------            
      if (pos==0 && key>3){
        fechas[pos]=0;
      }
      if (fechas[0]==3 && pos==1 && key>1){
        fechas[pos]=0;
      }
      if (pos==2 && key>1){
        fechas[pos]=0;
      }                  
      if (fechas[2]==1 && pos==3 && key>2){
        fechas[pos]=0;
      }         
      //----------------------------------------------------------------      
      lcd.print(fechas[pos]);
    }
    pos++;
    if(pos>6){ 
      pos=6; 
    }
  }
}
//------------------------------------------------------------------------------
//-------------------------CONFIGURAR PARAMETROS DE CRIANZA-------------------------------

void parametros(){
  lcd.clear();
  escribe(0,3,13);           //Parametros de 
  escribe(1,6,14);           //Crianza
  escribe(2,0,15);           //1:
  escribe(2,2,6);           //Configurar    
  escribe(3,0,16);           //2:
  escribe(3,2,17);           //Automaticos     
  pos=0;
  key=0;
  lcd.setCursor(pos,1);

  keyboardValue = analogRead(keyboardPin); // Leer el valor de la tecla presionada
  while (keyboardValue < 25){                 //Si no hay tecla precionada
    keyboardValue = analogRead(keyboardPin); //espera a que se precione una tecla
    delay(50);
  }        
  readkeyboard();

  if (key == 1){ // Paramtetros personalizados
    temperaturamax();  //++++++
    temperaturamin();  //++++++
    alimentacion();    //++++++
  }
  else if (key == 2){ // Paramtetros automaticos
    lcd.clear();
    escribe(0,3,13);           //Parametros de 
    escribe(1,6,14);           //Crianza
    escribe(2,5,17);           //Automaticos
    delay(3000);      
    tmax = 23;
    tmin = 18;
    alim1h= 8;
    alim1m= 0;
    alim2h= 16;
    alim2m= 0;
    alim3h= 0;
    alim3m= 0;     
  }

}

//-----------------------------------------------------------------------------------

//------------------------CONFIGURAR TEMPERATURA MAXIMA--------------------------------

void temperaturamax(){

  lcd.clear();
  escribe(0,5,6);            //Configurar 
  escribe(1,0,18);           //Temperatura
  escribe(1,12,19);           //Maxima:
  lcd.setCursor(2,2);
  lcd.write(B11011111);      //°
  escribe(2,3,20);           //C 
  pos=0;
  key=0;

  while (key != 11){

    lcd.setCursor(pos,1);

    keyboardValue = analogRead(keyboardPin); // Leer el valor de la tecla presionada
    while (keyboardValue < 25){              //Si no hay tecla precionada
      keyboardValue = analogRead(keyboardPin); //espera a que se precione una tecla
      delay(50);
    }        
    readkeyboard();      

    if (key == 10){
      teclaborrar(0, 1, 0, 0, 0, 0, 0, 0, 2);
    }

    else if (key!= 11){
      byte poss[]={
        0, 1, 1                  };
      tempmax[pos] = key;
      lcd.setCursor(poss[pos],2);
      limittemp();        
      lcd.write(tempmax[pos]+48);
    }
    pos++;
    if (pos>2){
      pos=2;
    }
  }
  tmax = (byte) ((tempmax[0] * 10) + tempmax[1]);
}

//-------------------------------------------------------------------------------------
//------------------------CONFIGURAR TEMPERATURA MINIMA--------------------------------

void temperaturamin(){

  lcd.clear();
  escribe(0,5,6);            //Configurar     
  escribe(1,0,18);           //Temperatura
  escribe(1,12,21);           //Minima:
  lcd.setCursor(2,2);
  lcd.write(B11011111);      //°
  escribe(2,3,20);           //C 
  pos=0;
  key=0;

  while (key != 11){

    lcd.setCursor(pos,1);

    keyboardValue = analogRead(keyboardPin); // Leer el valor de la tecla presionada
    while (keyboardValue < 25){              //Si no hay tecla precionada
      keyboardValue = analogRead(keyboardPin); //espera a que se precione una tecla
      delay(50);
    }
    readkeyboard();

    if (key == 10){
      teclaborrar(0, 1, 0, 0, 0, 0, 0, 0, 2);
    }

    else if (key!= 11){
      byte poss[]={
        0, 1, 1                  };
      tempmin[pos] = key;
      lcd.setCursor(poss[pos],2);
      limittemp();    
      lcd.write(tempmin[pos]+48);
    }
    pos++;
    if (pos>2){
      pos=2;
    }
  }
  tmin = (byte) ((tempmin[0] * 10) + tempmin[1]);
}

//-------------------------------------------------------------------------------------
//------------------------CONFIGURACION ALIMENTACION-----------------------------------

void alimentacion(){
  lcd.clear();
  escribe(0,5,6);            //Configurar
  escribe(1,3,22);           //Alimentacion
  delay(3000);
  lcd.clear();    
  escribe(0,0,15);           //1:
  escribe(0,2,23);           //Alimentar
  escribe(0,12,24);          //cada 
  escribe(1,0,25);           //XX     
  escribe(1,3,26);           //horas
  escribe(2,0,16);           //2:
  escribe(2,2,23);           //Alimentar
  escribe(2,12,27);          //en
  escribe(3,0,26);           //horas   
  escribe(3,6,28);           //especificas        
  pos=0;
  key=0;
  lcd.setCursor(pos,1);

  keyboardValue = analogRead(keyboardPin); // Leer el valor de la tecla presionada
  while (keyboardValue < 25){                //Si no hay tecla precionada
    keyboardValue = analogRead(keyboardPin); //espera a que se precione una tecla
    delay(50);
  }        
  readkeyboard();      

  if (key == 1){
    periodo();
  }
  else if (key == 2){
    per=0;
    horarios();
  }

}
//--------------------------------------------------------------------------------------
//------------------------CONFIGURAR PERIODO--------------------------------

void periodo(){

  lcd.clear();
  escribe(0,5,29);           //Periodo de
  escribe(1,4,22);           //Alimentacion
  escribe(2,0,23);           //Alimentar
  escribe(2,10,24);          //cada
  escribe(3,8,26);           //horas         
  pos=0;
  key=0;

  while (key != 11){

    lcd.setCursor((pos+15),1);//===============================

    keyboardValue = analogRead(keyboardPin);  // Leer el valor de la tecla presionada
    while (keyboardValue < 25){              //Si no hay tecla precionada
      keyboardValue = analogRead(keyboardPin); //espera a que se precione una tecla
      delay(50);
    }        
    readkeyboard();      

    if (key == 10){
      teclaborrar(5, 6, 0, 0, 0, 0, 0, 0, 3);
    }

    else if (key!= 11){
      byte poss[]={
        5, 6, 6                  };
      period[pos] = key;//============================================================
      lcd.setCursor(poss[pos],3);
      //-------------------LIMITADORES DE PERIODO DE ALIMENTACION------------
      if (pos==0 && key>2){
        period[pos]=2;
      }
      //---------------------------------------------------------         
      lcd.write(period[pos]+48);//se suma 48 por que es tipo byte y se debe convertir a char
    }
    pos++;
    if (pos>2){
      pos=2;
    }
  }
  per = (byte) ((period[0] * 10) + period[1]);
  alimperiodo(per);

  //lcd.clear();
  //lcd.print(per,DEC);
  //delay(3000);

}

//-------------------------------------------------------------------------------------
//------------------------CONFIGURAR HORARIOS--------------------------------

void horarios(){
  int contalim;
  contalim=0;
  //  lcd.clear();
  //  lcd.home();
  //  lcd.print("Horarios de Alim");

  while (contalim != 3){  

    lcd.clear();
    escribe(0,4,30);            //Horarios de
    escribe(1,4,22);           //Alimentacion    
    escribe(2,2,23);           //Alimentar 
    escribe(2,12,31);          //a las
    escribe(3,0,9);            //Hora:
    lcd.setCursor(4,3);
    lcd.print(contalim+1);    
    escribe(3,5,10);           //:     
    escribe(3,9,10);           //:     
    pos=0;
    key=0;

    while (key != 11){

      lcd.setCursor((pos+7),1);

      keyboardValue = analogRead(keyboardPin); // Leer el valor de la tecla presionada
      while (keyboardValue < 25){              //Si no hay tecla precionada
        keyboardValue = analogRead(keyboardPin); //espera a que se precione una tecla
        delay(50);
      }        
      readkeyboard();

      if (key == 10){
        teclaborrar(7, 8, 10, 11, 0, 0, 0, 0, 3);
      }

      else if (key!= 11){
        byte poss[]={
          7, 8, 10, 11, 11                        };
        horario[pos] = key;
        lcd.setCursor(poss[pos],3);
        limithora();
        lcd.print(horario[pos]);
      }
      pos++;
      if(pos>4){ 
        pos=4; 
      }

    }
    if (contalim==0){
      alim1h = (byte) ((horario[0] * 10) + horario[1]);
      alim1m = (byte) ((horario[2] * 10) + horario[3]);
    }
    else if (contalim==1){
      alim2h = (byte) ((horario[0] * 10) + horario[1]);
      alim2m = (byte) ((horario[2] * 10) + horario[3]);
    }
    else if (contalim==2){
      alim3h = (byte) ((horario[0] * 10) + horario[1]);
      alim3m = (byte) ((horario[2] * 10) + horario[3]);
    }

    contalim++;

  }
  /*  
   lcd.clear();
   lcd.print(alim1h,DEC);
   lcd.print(alim1m,DEC);
   lcd.print("/");
   lcd.print(alim2h,DEC);
   lcd.print(alim2m,DEC);
   lcd.print("/");
   lcd.print(alim3h,DEC);
   lcd.print(alim3m,DEC);
   
   */
}

//-------------------------------------------------------------------------------------

//---------------------------TECLA BORRAR---------------------------
void teclaborrar(byte b0, byte b1, byte b2, byte b3, byte b4, byte b5, byte b6, byte b7, byte b8)
{ 
  byte bor[]={
    b0 ,b1, b2, b3, b4, b5, b6, b7      };
  /*
  EEPROM.write(61, b0);
   EEPROM.write(62, b1);
   EEPROM.write(63, b2);
   EEPROM.write(64, b3);
   EEPROM.write(65, b4);
   EEPROM.write(66, b5);
   EEPROM.write(67, b6);
   EEPROM.write(68, b7);
   */

  pos--;
  // lcd.setCursor(EEPROM.read(pos+60),b8);
  lcd.setCursor(bor[pos],b8);
  lcd.print(' ');
  delay(150);
  pos--;

  if (pos<0){
    pos=-1;
  }
}
//---------------------------------------------------------------    

//---------------LIMITADORES DE HORA-----------------------------
void limithora(){
  if (pos==0 && key>2){
    horario[pos]=2;
  }
  if (horario[0]==2 && pos==1 && key>3){
    horario[pos]=3;
  }
  if (pos==2 && key>5){
    horario[pos]=5;
  }
}    
//---------------------------------------------------------------    

//-------------------LIMITADORES DE TEMPERATURA-------------------
void limittemp(){
  if (pos==0 && key>2){
    tempmax[pos]=0;
  }      
  if (tempmin[0]==2 && pos==1 && key>7){
    tempmin[pos]=0;
  }//la temperatura no sera mayor a 27 grados.
  if (tempmin[0]==0 && pos==1 && key<7){
    tempmin[pos]=7;
  }//la temperatura no sera menor a 7 grados.
}         
//----------------------------------------------------------------

//-----------------------CALCULO DE PERIODO DE LAIEMTACION--------
void alimperiodo(byte per){

  //------------LEE LA HORA ACTUAL---------------------------------------
  RTC.get(rtc,true);
  second     = rtc[0];
  minute     = rtc[1];
  hour       = rtc[2];
  dayOfWeek  = rtc[3];
  dayOfMonth = rtc[4];
  month      = rtc[5];
  year       = rtc[6];
  //---------------------------------------------------------------------------

  alim1h=hour;
  alim1m=minute; 

  for(byte i=0; i<3; i++){

    if (i==0){
      alim1h=per+alim1h;
      if (alim1h>23){
        alim1h= alim1h-24;
      }
    }
    else if(i==1){
      alim2h=per+alim1h;
      if (alim2h>23){
        alim2h= alim2h-24;
      }
      alim2m= alim1m; 
    }
    else if(i==2){
      alim3h=per+alim2h;
      if (alim3h>23){
        alim3h= alim3h-24;
      }
      alim3m= alim1m; 
    }
  }

  //lcd.clear();
  //lcd.print(alim1h,DEC);
  //lcd.print(alim1m,DEC);
  //lcd.print("/");
  //lcd.print(alim2h,DEC);
  //lcd.print(alim2m,DEC);
  //lcd.print("/");
  //lcd.print(alim3h,DEC);
  //lcd.print(alim3m,DEC); 

  //delay(7000);

}
//---------------------------------------------------------------

//-------------------------ALIMENTAR----------------------------
void Alimentar(byte ti){
  shifter.setPin(3, HIGH);
  shifter.write(); 
  delay(ti*100);
  shifter.setPin(3, LOW); 
  shifter.write(); 
}
//---------------------------------------------------------------
//---------------------DESECHOS--------------------------------
void recojeDesechos(byte td){
  shifter.setPin(2, HIGH);
  shifter.write(); 
  delay(td*100);
  shifter.setPin(2, LOW); 
  shifter.write(); 
}
//------------------------------------------------------------
//---------------------REVISAR ALIMENTO------------------------
void revisaAlimento(){
  shifter.setPin(9, HIGH);
  shifter.write(); 
  
  delay(500);
  if (digitalRead(5)==1){
    alarmaAlimento=1;
    escribe(2,2,42);	//Alarma, alimento
    escribe(3,4,43);	//insuficiente
    sprintf(numTelef, "%01u%01u%01u%01u%01u%01u%01u%01u", telef[0], telef[1], telef[2], telef[3], telef[4], telef[5], telef[6], telef[7]);    
    SendSMS(numTelef,1);

  }
  else{ 
  	alarmaAlimento=0;
    escribe(2,0,44);	//<Eliminar linea de mensaje>
    escribe(3,0,44);	//<Eliminar linea de mensaje>
  }
  shifter.setPin(9, LOW); 
  shifter.write(); 
  
}
//--------------------------------------------------------------

//---------------------REVISAR DESECHOS------------------------
void revisaDesechos(){
  shifter.setPin(8, HIGH);
  shifter.write(); 

  delay(1000);
  if (digitalRead(6)!=1){
    alarmaDesechos=1;
    recojeDesechos(50);
  }
  else alarmaDesechos=0;
  
  shifter.setPin(8, LOW); 
  shifter.write(); 
}


//---------------INICIAR DISPLAY-----------------------------
void displayinic(){
  lcd.clear();
  lcd.setCursor(0,1);
  lcd.print("T1:");
  lcd.setCursor(5,1);
  lcd.write(B11011111);
  escribe(1,6,20);
  escribe(0,2,10);
  //lcd.setCursor(6,1);
  //lcd.print("C");  
  //lcd.setCursor(2,0);
  //lcd.print(":");
}  
//----------------------------------------------------------

//------------------------------KEYPAD-------------------------------
void readkeyboard(){
  keyboardValue = analogRead(keyboardPin); // LEER EL VALOR ENTRE 0 y 1023
  if (keyboardValue <25){
    key = 0;
  }
  if ((keyboardValue >25) && (keyboardValue < 67)){
    key = 1;
  }
  if ((keyboardValue >67) && (keyboardValue < 108)){
    key = 2;
  }
  if ((keyboardValue >108) && (keyboardValue < 162)){
    key = 3;
  }
  if ((keyboardValue >162) && (keyboardValue < 253)){
    key = 4;
  }
  if ((keyboardValue >253) && (keyboardValue < 361)){
    key = 5;
  }
  if ((keyboardValue >361) && (keyboardValue < 479)){
    key = 6;
  }
  if ((keyboardValue >479) && (keyboardValue < 619)){
    key = 7;
  }
  if ((keyboardValue >619) && (keyboardValue < 765)){
    key = 8;
  }
  if ((keyboardValue >765) && (keyboardValue < 819)){
    key = 9;
  }
  if ((keyboardValue >819) && (keyboardValue < 889)){
    key = 10;
  }
  if ((keyboardValue >889) && (keyboardValue < 938)){
    key = 0;
  }
  if (keyboardValue >938){
    key = 11;
  }
  //NOTE: the values used above are all halfway between the value obtained with each keypress in previous test sketch 

  while (keyboardValue > 25) {
    delay (100);
    keyboardValue = analogRead(keyboardPin); // LEER EL VALOR ENTRE 0 y 1023
  }
  delay (50);
}
//-------------------------------------------------------------------
//++++++++++++++++++++++++++++++++++++++++++++++
void comparaSMSs(){
  GetSMS();
  //Serial.println(n);
  Serial.println(SMS_text);
  //sprintf (SMSrec, "%01u%01u%01u%01uxxxxxxxxxxxxxxxxxxxx", pasuser[0],pasuser[1],pasuser[2],pasuser[3]);
  if(SMS_text[0]==(pasuser[0]+48) && SMS_text[1]==(pasuser[1]+48) && SMS_text[2]==(pasuser[2]+48) && SMS_text[3]==(pasuser[3]+48)){
    //Serial.println(SMSrec);
    //if(!strncmp(SMS_text,SMSrec,4)){
    if (SMS_text[4]=='a'){
      logconfig=1;
      tmax = ((SMS_text[5]-48)*10) + (SMS_text[6]-48);
      if (SMS_text[7]=='i'){
        tmin = ((SMS_text[8]-48)*10) + (SMS_text[9]-48);
        if (SMS_text[10]=='h'){
          alim1h = ((SMS_text[11]-48)*10) + (SMS_text[12]-48);
          alim1m = ((SMS_text[13]-48)*10) + (SMS_text[14]-48);
          alim2h = ((SMS_text[16]-48)*10) + (SMS_text[17]-48);
          alim2m = ((SMS_text[18]-48)*10) + (SMS_text[19]-48);
          alim3h = ((SMS_text[21]-48)*10) + (SMS_text[22]-48);
          alim3m = ((SMS_text[23]-48)*10) + (SMS_text[24]-48);  
          //sprintf(asc_msg, "Temp: %02dC,Alim1-%02d:%02d,Alim2-%02d:%02d,Alim3-%02d:%02d", tempC,alim1h,alim1m,alim2h,alim2m,alim3h,alim3m);          
          //sprintf(asc_msg, "Temp: %02dC,Alim1-%02d:%02d,Alim2-%02d:%02d,Alim3-%02d:%02d", EEPROM.read(49), alim1h,alim1m,alim2h,alim2m,alim3h,alim3m);          
        }   
        else if (SMS_text[10]=='p'){
          per = ((SMS_text[11]-48)*10) + (SMS_text[12]-48);
          //sprintf(asc_msg, "Temp: %02dC,Alim1-%02d:%02d,Alim2-%02d:%02d,Alim3-%02d:%02d", tempC,alim1h,alim1m,alim2h,alim2m,alim3h,alim3m);
          alimperiodo(per);
          //sprintf(asc_msg, "Temp: %02dC,Alim1-%02d:%02d,Alim2-%02d:%02d,Alim3-%02d:%02d", EEPROM.read(49),alim1h,alim1m,alim2h,alim2m,alim3h,alim3m);
        }              
      }
      sprintf(numTelef, "%01u%01u%01u%01u%01u%01u%01u%01u", telef[0], telef[1], telef[2], telef[3], telef[4], telef[5], telef[6], telef[7]);
      //sprintf(asc_msg, "Temp: %02dC,Alim1-%02d:%02d,Alim2-%02d:%02d,Alim3-%02d:%02d", tempC,alim1h,alim1m,alim2h,alim2m,alim3h,alim3m);
      //sprintf(asc_msg, "Temp: %02dC,Alim1-%02d:%02d,Alim2-%02d:%02d,Alim3-%02d:%02d", EEPROM.read(49),alim1h,alim1m,alim2h,alim2m,alim3h,alim3m);
      Serial.println("Config");
      if(alarmaAlimento==1){SendSMS(numTelef,1); delay(1000);}
      SendSMS(numTelef,2);
    }
    else if(SMS_text[4]=='i'){
      loginf=1;
      if (SMS_text[5]=='n'){   
        sprintf(numTelef, "%01u%01u%01u%01u%01u%01u%01u%01u", telef[0], telef[1], telef[2], telef[3], telef[4], telef[5], telef[6], telef[7]);
        //sprintf(asc_msg, "Temp: %02dC,Alim1-%02d:%02d,Alim2-%02d:%02d,Alim3-%02d:%02d", tempC,alim1h,alim1m,alim2h,alim2m,alim3h,alim3m);
        //sprintf(asc_msg, "Temp: %02dC,Alim1-%02d:%02d,Alim2-%02d:%02d,Alim3-%02d:%02d", EEPROM.read(49),alim1h,alim1m,alim2h,alim2m,alim3h,alim3m);
        Serial.println("Envia Informe");
        if(alarmaAlimento==1){SendSMS(numTelef,1); delay(1000);}
        SendSMS(numTelef,2);
      }   
      else if (SMS_text[5]!='n'){
        sprintf(numTelef, "%c%c%c%c%c%c%c%c", SMS_text[5], SMS_text[6], SMS_text[7], SMS_text[8], SMS_text[9], SMS_text[10], SMS_text[11], SMS_text[12]);
        //sprintf(asc_msg, "Temp: %02dC,Alim1-%02d:%02d,Alim2-%02d:%02d,Alim3-%02d:%02d", tempC,alim1h,alim1m,alim2h,alim2m,alim3h,alim3m);
        //sprintf(asc_msg, "Temp: %02dC,Alim1-%02d:%02d,Alim2-%02d:%02d,Alim3-%02d:%02d", EEPROM.read(49),alim1h,alim1m,alim2h,alim2m,alim3h,alim3m);
        Serial.println("Envia Informe a otro tlf");
        if(alarmaAlimento==1){SendSMS(numTelef,1); delay(1000);}
        SendSMS(numTelef,2);
      }              
    }
  }
  logSMS=1; 
  logDatos(); 
}
//=================Enciende GSM============================
void enciendeGSM()
{
  digitalWrite(3, HIGH);   // set the LED on
  delay(3000);              // wait for a second
  digitalWrite(3, LOW);   // set the LED on
}

//=================Reiniciar GSM==============================
void reiniciaGSM()
{
  digitalWrite(2, HIGH);   // set the LED on
  delay(3000);              // wait for a second
  digitalWrite(2, LOW);   // set the LED on
}

//====================ESCRIBE SD CARD=========================


void power_on(){

  uint8_t answer=0;
  int cnt;
  cnt=0;
  // checks if the module is started
  delay(2000);
  answer = sendATcommand("AT", "OK", 2000);
  if (answer == 0)
  {
    // power on pulse
    enciendeGSM();
    delay(20000);

    // waits for an answer from the module
    while(!(answer = sendATcommand("AT", "OK", 2000)) && (cnt<5)){
      cnt++;
    }    // Send AT every two seconds and wait for the answer
    cnt=0;
    while((sendATcommand("AT+CREG?", "+CREG: 0,1", 1000) || sendATcommand("AT+CREG?", "+CREG: 0,5", 1000)) == 0 && cnt<5){
      cnt++;
    }
    if(cnt>4){
      enciendeGSM();
      error(37); //ERROR Modulo GSM
    } 
    while(!(sendATcommand("AT+CMGDA=\"DEL ALL\"", "OK", 2000))); 
  }
}


char GetSMS(){
  int8_t x=0;
  
  char *p_char; 
  char *p_char1;
  
  byte max_SMS_len;
  byte len;
   max_SMS_len=200;
  

 // int pch;
  
        answer = 0;
        while(Serial.available() == 0);
        // this loop reads the data of the SMS
        do{
            // if there are data in the UART input buffer, reads it and checks for the asnwer
            if(Serial.available() > 0){    
                SMS[x] = Serial.read();
                x++;
                // check if the desired answer (OK) is in the response of the module
                if (strstr(SMS, "OK") != NULL)    
                {
                    answer = 1;
                }
            }
        }while(answer == 0);    // Waits for the asnwer with time out
        
        SMS[x] = '\0';
        
        //Serial.print(SMS);
        
        sprintf(buff,"%s",SMS);
        //strchr(pchar,
        
        p_char = strchr((char *)(SMS),',');
        p_char1 = p_char+2; // we are on the first phone number character
        p_char = strchr((char *)(p_char1),'"');
        if (p_char != NULL) {
          *p_char = 0; // end of string
          strcpy(telefono, (char *)(p_char1));
        }
        
        
        p_char = strchr(p_char+1, 0x0a);  // find <LF>
      if (p_char != NULL) {
        // next character after <LF> is the first SMS character
        p_char++; // now we are on the first SMS character 

        // find <CR> as the end of SMS string
        p_char1 = strchr((char *)(p_char), 0x0d);  
        if (p_char1 != NULL) {
          // finish the SMS text string 
          // because string must be finished for right behaviour 
          // of next strcpy() function
          *p_char1 = 0; 
        }
        // in case there is not finish sequence <CR><LF> because the SMS is
        // too long (more then 130 characters) sms text is finished by the 0x00
        // directly in the gsm.WaitResp() routine

        // find out length of the SMS (excluding 0x00 termination character)
        len = strlen(p_char);

        if (len < max_SMS_len) {
          // buffer SMS_text has enough place for copying all SMS text
          // so copy whole SMS text
          // from the beginning of the text(=p_char position) 
          // to the end of the string(= p_char1 position)
          strcpy(SMS_text, (char *)(p_char));
        }
        else {
          // buffer SMS_text doesn't have enough place for copying all SMS text
          // so cut SMS text to the (max_SMS_len-1)
          // (max_SMS_len-1) because we need 1 position for the 0x00 as finish 
          // string character
          memcpy(SMS_text, (char *)(p_char), (max_SMS_len-1));
          SMS_text[max_SMS_len] = 0; // finish string
        }
      }


        
    while(!(sendATcommand("AT+CMGDA=\"DEL ALL\"", "OK", 2000)));    
        
    while( Serial.available() > 0) Serial.read();    // Clean the input buffer
    Serial.flush();
    for(int i=0; i<200; i++) SMS[i] = '\0';
/*
  int8_t x=0;
  char *p_char; 
  char *p_char1;
  byte len;

  answer = 0;
  while(Serial.available() == 0);
  // this loop reads the data of the SMS
  do{
    // if there are data in the UART input buffer, reads it and checks for the asnwer
    if(Serial.available() > 0){    
      SMS[x] = Serial.read();
      x++;
      // check if the desired answer (OK) is in the response of the module
      if (strstr(SMS, "OK") != NULL)    
      {
        answer = 1;
      }
    }
  }
  while(answer == 0);    // Waits for the asnwer with time out

  SMS[x] = '\0';

  //Serial.print(SMS);

  sprintf(buff,"%s",SMS);
  //strchr(pchar,

  p_char = strchr((char *)(SMS),',');
  p_char1 = p_char+2; // we are on the first phone number character
  p_char = strchr((char *)(p_char1),'"');
  if (p_char != NULL) {
    *p_char = 0; // end of string
    strcpy(phone_number, (char *)(p_char1));
  }


  p_char = strchr(p_char+1, 0x0a);  // find <LF>
  if (p_char != NULL) {
    // next character after <LF> is the first SMS character
    p_char++; // now we are on the first SMS character 

    // find <CR> as the end of SMS string
    p_char1 = strchr((char *)(p_char), 0x0d);  
    if (p_char1 != NULL) {
      // finish the SMS text string 
      // because string must be finished for right behaviour 
      // of next strcpy() function
      *p_char1 = 0; 
    }
    // in case there is not finish sequence <CR><LF> because the SMS is
    // too long (more then 130 characters) sms text is finished by the 0x00
    // directly in the gsm.WaitResp() routine

    // find out length of the SMS (excluding 0x00 termination character)
    len = strlen(p_char);

    if (len < max_SMS_len) {
      // buffer SMS_text has enough place for copying all SMS text
      // so copy whole SMS text
      // from the beginning of the text(=p_char position) 
      // to the end of the string(= p_char1 position)
      strcpy(SMS_text, (char *)(p_char));
    }
    else {
      // buffer SMS_text doesn't have enough place for copying all SMS text
      // so cut SMS text to the (max_SMS_len-1)
      // (max_SMS_len-1) because we need 1 position for the 0x00 as finish 
      // string character
      memcpy(SMS_text, (char *)(p_char), (max_SMS_len-1));
      SMS_text[max_SMS_len] = 0; // finish string
    }
  }
  
  //Serial.println(smsbuffer);

  while(!(sendATcommand("AT+CMGDA=\"DEL ALL\"", "OK", 2000)));    


  while( Serial.available() > 0) Serial.read();    // Clean the input buffer
  Serial.flush();
  for(int i=0; i<200; i++) SMS[i] = '\0'; 
  //delay(2000); 

  //Serial.println(smsbuffer);  
  */

}

void SendSMS(char* NTelef, int mens){
  int8_t answer;
  //char* mensaje;

  if (mens==1) sprintf(mensaje, "Alarma, alimento insuficiente");
  else if(mens==2) sprintf(mensaje, "Temp: %02d C,Alim1-%02d:%02d,Alim2-%02d:%02d,Alim3-%02d:%02d", tempC,alim1h,alim1m,alim2h,alim2m,alim3h,alim3m);
  
  Serial.println(mensaje);

  
  sendATcommand("AT+CMGF=1", "OK", 1000);    // sets the SMS mode to text
  //Serial.println("Sending SMS");

  //for (int i=0;i<8;i++){
  // numTelef[i]=NTelef[i]+48;
  //}
  sprintf(aux_string,"AT+CMGS=\"%s\"", NTelef);
  answer = sendATcommand(aux_string, ">", 2000);    // send the SMS number
  if (answer == 1)
  {
    Serial.println(mensaje);
    Serial.write(0x1A);
    answer = sendATcommand("", "OK", 20000);
    if (answer == 1)
    {
      Serial.print("Sent ");    
    }
    else
    {
      Serial.print("error ");
    }
  }
  else
  {
    Serial.print("error ");
    //Serial.println(answer, DEC);
  }
}

int8_t sendATcommand(char* ATcommand, char* expected_answer, unsigned int timeout){

  uint8_t y=0,  answer=0;
  char response[100];
  unsigned long previous;

  memset(response, '\0', 100);    // Initialice the string

  delay(100);

  while( Serial.available() > 0) Serial.read();    // Clean the input buffer

  Serial.println(ATcommand);    // Send the AT command 


    y = 0;
  previous = millis();

  // this loop waits for the answer
  do{
    // if there are data in the UART input buffer, reads it and checks for the asnwer
    if(Serial.available() != 0){    
      response[y] = Serial.read();
      //Serial.print(response[y]);
      y++;
      // check if the desired answer is in the response of the module
      if (strstr(response, expected_answer) != NULL)    
      {
        answer = 1;
      }
    }
    // Waits for the asnwer with time out
  }
  while((answer == 0) && ((millis() - previous) < timeout));    
  Serial.println("2");
  return answer;
}

void error(int code){
  
  lcd.clear();
  escribe(0,5,35);      //ERROR!
  escribe(1,0,code);      //Inserte SD Card
  delay(10000);

  shifter.setPin(14, LOW);//LCD back ligth ON
  shifter.write();
  while(1);
}

void initSD(){

  if (!SD.begin(chipSelect))
  {
    /* If there was an error output this to the serial port and go no further */
    Serial.println("ERROR: SD card failed to initiliase");
  }else
  {
    Serial.println("SD Card OK");
  }

/*
  delay(4000);

  if (!card.init()) error(36);
  
  while( Serial.available() > 0) Serial.read();    // Clean the input buffer
  Serial.flush();
  
  if (!Fat16::init(&card)) error(36);
  */
}

void crearFile(){

  char name[] = "00_00_0000.CSV";

  sprintf(name,"%02d_%02d_%02d.CSV", RTC.get(DS1307_DATE,false), RTC.get(DS1307_MTH,false), (RTC.get(DS1307_YR,false)-2000));


  if(SD.exists(name))
  {
    /* If so then delete it */
    Serial.println("test.txt already exists...");
    //SD.remove("test.txt");
  }
  else{
    Serial.println("Creating LOG");
    SDFileData = SD.open(name, FILE_WRITE);
  }

  if (SDFileData)
  {
    //SDFileData.println("It worked !");
    SDFileData.println(", , , , , , , , , , , , , "); // write string from RAM
   // file.write("\r\n");
    SDFileData.println("HORA, TEMP, TEMPMAX, TEMPMIN, HORALIM1, HORALIM2, HORALIM3, CALEFACTOR, VEMTILADOR, NIVALIMENTO, SMS, CONFIGURACION, INFORMESME, DESTINO");
    //file.write("\r\n");



 
    /* Close the file */
    SDFileData.close();   
   
    Serial.println("done.");
  }
  else
  {
      Serial.println("Error writing to file");
  }
}

void writeNumber(uint32_t n) {
  uint8_t buf[4];//[10]
  uint8_t i = 0;
  do {
    i++;
    buf[sizeof(buf) - i] = n%10 + '0';
    n /= 10;
  } 
  while (n);
  SDFileData.write(&buf[sizeof(buf) - i], i); // write the part of buf with the number
  //SDFileData.println(&buf[sizeof(buf) - i], i); // write the part of buf with the number
}

void logDatos(){
  
  PgmPrintln("LOG");//+++++++++++++++++++++++++++++++++++++++
  
  //initSD();

  char name[] = "00_00_0000.CSV";
  
  sprintf(name,"%02d_%02d_%02d.CSV", RTC.get(DS1307_DATE,false), RTC.get(DS1307_MTH,false), (RTC.get(DS1307_YR,false)-2000));

  SDFileData = SD.open(name, FILE_WRITE);

  
 // if (file.open(name, O_WRITE));

 // if (!file.isOpen()){PgmPrintln("4"); error(36);}
  //else{
  /*
  if (SDFileData)
  {

    writeNumber(RTC.get(DS1307_HR,false));
    file.write_P(PSTR(":"));
    if ((RTC.get(DS1307_MIN,false)) < 10)
    file.write_P(PSTR("0"));
    writeNumber(RTC.get(DS1307_MIN,false)); 
    file.write_P(PSTR(","));
    writeNumber(tempx); 
    file.write_P(PSTR(","));
    writeNumber(tmax); 
    file.write_P(PSTR(","));
    writeNumber(tmin); 
    file.write_P(PSTR(",")); 
     writeNumber(alim1h); 
    file.write_P(PSTR(":"));
    if (alim1m < 10) file.write_P(PSTR("0")); 
    writeNumber(alim1m); 
    file.write_P(PSTR(","));    
     writeNumber(alim2h); 
    file.write_P(PSTR(":")); 
    if (alim1m < 10) file.write_P(PSTR("0"));
    writeNumber(alim2m); 
    file.write_P(PSTR(","));
     writeNumber(alim3h); 
    file.write_P(PSTR(":"));
    if (alim1m < 10) file.write_P(PSTR("0")); 
    writeNumber(alim3m); 
    file.write_P(PSTR(","));
    if (calefac==0) file.write_P(PSTR("No,"));
    else if (calefac==1) file.write_P(PSTR("Si,"));
    if (ventilad==0) file.write_P(PSTR("No,"));
    else if (ventilad==0) file.write_P(PSTR("Si,"));
    file.write_P(PSTR("Nivel,"));
    if (logSMS==1){file.write(smsbuffer);}
    else file.write_P(PSTR("No"));
    file.write_P(PSTR(","));
    if (logconfig==1){file.write_P(PSTR("Si")); logconfig=0;}
    else file.write_P(PSTR("No"));
    file.write_P(PSTR(","));
    if (loginf==1){file.write_P(PSTR("Si")); loginf=0;}
    else file.write_P(PSTR("No"));
    file.write_P(PSTR(","));    
    if (logSMS==1){file.write(numTelef); logSMS=0;}
    else file.write_P(PSTR("No"));   
    file.write("\r\n");
  }
*/
if (SDFileData)
  {
    //SDFileData.println("It worked");


    writeNumber(RTC.get(DS1307_HR,false));
    SDFileData.write(":");
    if ((RTC.get(DS1307_MIN,false)) < 10)
    SDFileData.write("0");
    writeNumber(RTC.get(DS1307_MIN,false)); 
    SDFileData.write(",");
    writeNumber(tempx); 
    SDFileData.write(",");
    writeNumber(tmax); 
    SDFileData.write(",");
    writeNumber(tmin); 
    SDFileData.write(","); 
     writeNumber(alim1h); 
    SDFileData.write(":");
    if (alim1m < 10) SDFileData.write("0"); 
    writeNumber(alim1m); 
    SDFileData.write(",");    
     writeNumber(alim2h); 
    SDFileData.write(":"); 
    if (alim1m < 10) SDFileData.write("0");
    writeNumber(alim2m); 
    SDFileData.write(",");
     writeNumber(alim3h); 
    SDFileData.write(":");
    if (alim1m < 10) SDFileData.write("0"); 
    writeNumber(alim3m); 
    SDFileData.write(",");
    if (calefac==0) SDFileData.write("No,");
    else if (calefac==1) SDFileData.write("Si,");
    if (ventilad==0) SDFileData.write("No,");
    else if (ventilad==1) SDFileData.write("Si,");
    SDFileData.write("Nivel,");
    if (logSMS==1){SDFileData.write(SMS_text);}
    else SDFileData.write("No");
    SDFileData.write(",");
    if (logconfig==1){SDFileData.write("Si"); logconfig=0;}
    else SDFileData.write("No");
    SDFileData.write(",");
    if (loginf==1){SDFileData.write("Si"); loginf=0;}
    else SDFileData.write("No");
    SDFileData.write(",");    
    if (logSMS==1){SDFileData.write(numTelef); logSMS=0;}
    else SDFileData.write("No");   
    SDFileData.write("\r\n");
 
    /* Close the file */
    SDFileData.close();   
   
    Serial.println("done.");
  }else

  {
      Serial.println("Error writing to file");
  }




  // close file and force write of all data to the SD card
  //file.close(); 
  //if (!file.sync());

  delay(2000);  
}


