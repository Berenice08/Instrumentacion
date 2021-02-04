#define maxperiod_siz 80 // nmaximo de periodos
#define measures 10      // nperiodos almacenados
#define samp_siz 4       // muestras para promedio
#define rise_threshold 3 // serie de medidas para determinar el aumento de un pico

#include <Ethernet.h>

#include <SPI.h>
#include <Wire.h>      // libreria para bus I2C
#include <Adafruit_GFX.h>   // libreria para pantallas graficas
#include <Adafruit_SSD1306.h>   // libreria para controlador SSD1306
 
#define ANCHO 128     // reemplaza ocurrencia de ANCHO por 128
#define ALTO 64       // reemplaza ocurrencia de ALTO por 64

#define OLED_RESET 4      // necesario por la libreria pero no usado
Adafruit_SSD1306 oled(ANCHO, ALTO, &Wire, OLED_RESET);  // crea objeto

int T = 20;              // slot milliseconds to read a value from the sensor
int sensorPin = A1; 
int REDLed = 3;
int IRLed = 4;

int sensor=A0;
int lecturaSensor = 0;

byte data [128];
byte indexs =0;
byte anterior;


// Configuracion del Ethernet Shield
byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFF, 0xEE}; // Direccion MAC
byte ip[] = { 192,168,100,40 }; // Direccion IP del Arduino
byte server[] = { 192,168,100,11 }; // Direccion IP del servidor (compu) 


long  rango= 5;
long ultimamedicion;
int ciclo = 0;
int cambiodeciclo = 0;
int picodetension;
int valledetension=1023;
long contadorciclo;


boolean estadoBPM = true;
boolean estadoLed=true;
int intervaloEncendido =100;
int intervaloApagado = 100;


int IntervaloBPM = 5000;   //cada 10 segundos srian 10000, para mutiplicarlo luego por 12 es 5000
int IntervaloBPM2= 1;


unsigned long tiempoAnteriorEncendido = 0;
unsigned long tiempoAnteriorApagado =0;
unsigned long tiempoAnteriorBPM = 0;
unsigned long tiempoAnteriorBPM2=0;


int pulsos=0;
int pulsos2=0;
int senal =0;

EthernetClient client;

void setup() {
  Serial.begin(9600);
   Serial.flush();
   pinMode(sensorPin,INPUT);
   pinMode(REDLed,OUTPUT);
   pinMode(IRLed,OUTPUT);

   Wire.begin();         // inicializa bus I2C
   oled.begin(SSD1306_SWITCHCAPVCC, 0x3C); // inicializa pantalla con direccion 0x3C
   Ethernet.begin(mac, ip);

   // initialize the LCD
   

   // turn off leds
   digitalWrite(REDLed,LOW);
   digitalWrite(IRLed,LOW);

   pinMode (8, OUTPUT);
   for (int i=0;i<128; i++)
    data[i]=0;

}

void loop() {

  
    
  
  bool finger_status = true;
  
  float readsIR[samp_siz], sumIR,lastIR, reader, start;
  float readsRED[samp_siz], sumRED,lastRED;


  int period, samples;
  period=0; samples=0;
  int samplesCounter = 0;
  float readsIRMM[maxperiod_siz],readsREDMM[maxperiod_siz];
  int ptrMM =0;
  for (int i = 0; i < maxperiod_siz; i++) { readsIRMM[i] = 0;readsREDMM[i]=0;}
  float IRmax=0;
  float IRmin=0;
  float REDmax=0;
  float REDmin=0;
  double R=0;

  int SpO2=0;
  int bpm=0;
  

  float measuresR[measures];
  int measuresPeriods[measures];
  int m = 0;
  for (int i = 0; i < measures; i++) { measuresPeriods[i]=0; measuresR[i]=0; }
   
  int ptr;

  float beforeIR;

  bool rising;
  int rise_count;
  int n;
  long int last_beat;
  for (int i = 0; i < samp_siz; i++) { readsIR[i] = 0; readsRED[i]=0; }
  sumIR = 0; sumRED=0; 
  ptr = 0; 
    
 
  
  while(1)
  {

    float avR = 0;
    int avBPM=0;
    //
    // enciende LED IR
    digitalWrite(REDLed,LOW);
    digitalWrite(IRLed,HIGH);
    
    // promedio de la señal durante 20 ms
    n = 0;
    start = millis();
    reader = 0.;
    do
    {
      reader += analogRead (sensorPin);
      n++;
    }
    while (millis() < start + T);  
    reader /= n;  // promedio (añadir nueva medida al array y sustarer de la antigua, para asi tener la ultima medida
    sumIR += reader;
    readsIR[ptr] = reader;
    lastIR = sumIR / samp_siz;

    
    digitalWrite(REDLed,HIGH);
    digitalWrite(IRLed,LOW);

    n = 0;
    start = millis();
    reader = 0.;
    do
    {
      reader += analogRead (sensorPin);
      n++;
    }
    while (millis() < start + T);  
    reader /= n;  
    sumRED -= readsRED[ptr];
    sumRED += reader;
    readsRED[ptr] = reader;
    lastRED = sumRED / samp_siz;


    //                                  
    // Cálculo de R, se guardan  las muestras de periodo tanto de IR como para el LED RED  

    readsIRMM[ptrMM]=lastIR;
    readsREDMM[ptrMM]=lastRED;
    ptrMM++;
    ptrMM %= maxperiod_siz;
    samplesCounter++;
    

  
    R =  ( (REDmax-REDmin) / REDmin) / ( (IRmax-IRmin) / IRmin ) ;
        
     
    //Serial.println("R");  
    //Serial.print("=");
    //Serial.print(R);
    //Serial.print('\n');

     
    SpO2 = -57.143*R+119;  //modifica con calibración

    
    if (lastIR > beforeIR)
       {
 
         rise_count++;  // count the number of samples that are rising
         if (!rising && rise_count > rise_threshold)
         {
            rising = true;

            measuresR[m] = R;
            measuresPeriods[m] = millis() - last_beat;
            last_beat = millis();
            int period = 0;
            for(int i =0; i<measures; i++) period += measuresPeriods[i];

            period = period / measures;   //muestras promedio para hallar maximos y minimos
            samples = period / (2*T);
              
             int avPeriod = 0;

            int c = 0;  //medidas "buenas" en los últimos 10 picos
            for(int i =1; i<measures; i++) {
              if ( (measuresPeriods[i] <  measuresPeriods[i-1] * 1.1)  &&  
                    (measuresPeriods[i] >  measuresPeriods[i-1] / 1.1)  ) {

                  c++;
                  avPeriod += measuresPeriods[i];
                  avR += measuresR[i];

              }
            }
              
            m++;
            m %= measures;
                        
            avBPM = 60000 / ( avPeriod / c) ;  //promedio de al menos 5 picos "buenos"
            avR = avR / c ;

    
    bpm= avBPM;

    if (SpO2<94){                //Con este ciclo marca dedo y no dedo, MODIFICA    

      
      Serial.print("No dedo");
      Serial.print('\n');
      delay(1000); //tiempo
   
      if (client.connect(server, 80)>0) {  // Conexion con el servidor
          client.print("GET /ethernet/iot2.php?"); // Enviamos los datos por GET
          client.print("SPO2=");
          client.print(0);
          client.print("&bpm=");
          client.print(0);
          client.print("&FR=");
          client.print(0);
          
          //Serial.print(SpO2);
          //client.print("GET /ethernet/iot2.php?bpm=");
          //client.print(SpO2);
          //Serial.print(SpO2);
          client.println(" HTTP/1.0");
          client.println("User-Agent: Arduino 1.0");
          client.println();

          Serial.println("Conectado");
        } else {
          Serial.println("Fallo en la conexion");
        }


      oled.clearDisplay();      // limpia pantalla
      oled.setTextColor(WHITE);   // establece color al unico disponible (pantalla monocromo)
      oled.setCursor(0, 0);     // ubica cursor en inicio de coordenadas 0,0
      oled.setTextSize(3);      // establece tamano de texto en 1
      oled.print("No hay dedo");  // escribe en pantalla el texto
      
      oled.display();     // muestra en pantalla todo lo establecido anteriormente
    
    }
    else {

        Serial.println("Connecting...");
        
        if (client.connect(server, 80)>0) {  // Conexion con el servidor
          client.print("GET /ethernet/iot2.php?"); // Enviamos los datos por GET
          client.print("SPO2=");
          client.print(SpO2);
          client.print("&bpm=");
          client.print(bpm);
          client.print("&FR=");
          client.print(pulsos2*12);
          
          //Serial.print(SpO2);
          //client.print("GET /ethernet/iot2.php?bpm=");
          //client.print(SpO2);
          //Serial.print(SpO2);
          client.println(" HTTP/1.0");
          client.println("User-Agent: Arduino 1.0");
          client.println();

          Serial.println("Conectado");
        } else {
          Serial.println("Fallo en la conexion");
        }
        if (!client.connected()) {
          Serial.println("Disconnected!");
        }
        client.stop();
        client.flush();
        delay(100); // Espero un minuto antes de tomar otra muestra TIEMPO
      /*Serial.print("SPo2= "); 
      Serial.print(SpO2);
      Serial.print('\n');
      Serial.print("bpm= ");
      Serial.print(bpm);
      Serial.print('\n');*/
      delay(1000); //TIEMPO


      oled.clearDisplay();      // limpia pantalla
      oled.setTextColor(WHITE);   // establece color al unico disponible (pantalla monocromo)
      oled.setCursor(0, 0);     // ubica cursor en inicio de coordenadas 0,0
      oled.setTextSize(1);      // establece tamano de texto en 1
      oled.print("%SpO2:");  // escribe en pantalla el texto
      oled.setCursor (1, 10);    // ubica cursor en coordenas 10,30
      oled.setTextSize(2);      // establece tamano de texto en 2
      oled.print(SpO2);    // escribe valor de millis() dividido por 1000
         
      
      oled.setCursor(4, 40);     // ubica cursor en inicio de coordenadas 0,0
      oled.setTextSize(1);      // establece tamano de texto en 1
      oled.print("BPM:");  // escribe en pantalla el texto
      oled.setCursor (5, 50);    // ubica cursor en coordenas 10,30
      oled.setTextSize(2);      // establece tamano de texto en 2
      oled.print(bpm);    // escribe valor de millis() dividido por 1000

         Serial.println("SPO2:");
    Serial.println(SpO2);
    
      
      //oled.display();     // muestra en pantalla todo lo establecido anteriormente
    //}descomenta 
      



   //oled.clearDisplay();
  indexs= indexs % 128;
  lecturaSensor= analogRead (sensor);
  //Serial.println(lecturaSensor);
  data[indexs]=64*lecturaSensor / 1024;
  indexs++;

  byte i = indexs;


for (int x=0; x<128; x++)
  {
    i=i % 128;

    //display.drawLine (x, data [i], x, anterior, 1);

    anterior = data [i];

    i++;

    if (x == 0)
    {
      //oled.clearDisplay();
    }
  }


  oled.setCursor (80,20);
  oled.setTextSize(1);
  oled.print ("F.R.");
//display.setCursor(10,20);
 oled.setCursor(83,30);
 oled.setTextSize(2);
 oled.print(pulsos2*12);  //era por 6
 oled.display();


 //Serial.println(pulsos2*12);
 //Calculo BPM

    senal=lecturaSensor;

   if (senal >= (ultimamedicion+12))  //moddii
   {
   ultimamedicion=senal;
   ciclo=1;

     if (senal>picodetension)
     {
      picodetension=senal;
     }
     }

   if (senal <= (ultimamedicion-12)) //modi
   {
    ultimamedicion =senal;
    ciclo=0;

    if (senal<valledetension)
     {
      valledetension=senal;
     }
   }

   ///////////// 1 minuto

   if (millis()-tiempoAnteriorBPM>=IntervaloBPM){
      lecturaSensor=analogRead(sensor);
    estadoBPM=false;
    pulsos2=pulsos;
      tiempoAnteriorBPM= millis();
    pulsos=0;

   }

   if (millis()-tiempoAnteriorBPM2>=IntervaloBPM2){
      lecturaSensor=analogRead(sensor);
    estadoBPM=true;

    tiempoAnteriorBPM2= millis();
   }

   if ((millis()-tiempoAnteriorEncendido>=intervaloEncendido) &&estadoLed==true && ciclo == 0)
   //if ((millis()-tiempoAnteriorEncendido>=intervaloEncendido) &&estadoLed==false && ciclo == 0)
   {
    estadoLed=false;
    //estadoLed=true;
    picodetension=senal;
    valledetension=senal;
    digitalWrite (8,HIGH);
    
    tiempoAnteriorApagado=millis();

   }

   if ((millis()-tiempoAnteriorEncendido>=  intervaloApagado) &&estadoLed==false && ciclo == 1)
   //if ((millis()-tiempoAnteriorEncendido>=intervaloEncendido) &&estadoLed==true && ciclo == 0)
    {
      pulsos++;
      picodetension=senal;
      valledetension=senal;
      estadoLed=true;
      //estadoLed=false;
    
      digitalWrite (8,LOW);

      tiempoAnteriorEncendido= millis();

    }

    oled.display();

    
    //delay(1000); TIMEPO


    
        }////////////


 }
 delay(20000);
  }
  }
}
 //delay(10000);
   

   
