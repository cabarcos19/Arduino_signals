  #include <MsTimer2.h>
  
  // Valores constantes
  const float Vref= 5.0; // Voltaje de referencia del ADC (por defecto)
  const unsigned int rango= 1024; // Rango ADC de Arduino Uno (10 bits)  
  const float pi= 3.1416;
  const unsigned int T= 1; // Intervalo entre puntos en ms
  const unsigned int Tg= 1; // Intervalo de generación en ms  
  const unsigned int Ts= 1; // Intervalo de muestreo en ms
  const int input_frecuencia=4;
  const int boton_1=2;
  const int boton_2=3;
  bool canal=true;
  // Variables globales
  volatile float n= 0; // Número de punto (0 a N-1)
  volatile unsigned int dg= 0; // Divisor de gen. de ms(0 a Tg-1)
  volatile unsigned int ds= 0; // Divisor de adq. de ms(0 a Ts-1)
  volatile unsigned int A_in; // Valor adquirido en A0  
  unsigned int N=50; // Número de puntos por ciclo
  int potenciometro1,potenciometro2,pt1,pt2,i;
  int canal_1;
  int canal_2;
  
  
  void setup()  
  {  
    Serial.begin(250000);
    canal_1=0;
    canal_2=0;  
    pt1=map(analogRead(A3),0,1023,0,127);
    pt2=map(analogRead(A4),0,1023,0,127);
    pinMode(input_frecuencia,INPUT_PULLUP);
    pinMode(boton_1, INPUT_PULLUP);
    pinMode(boton_2,INPUT_PULLUP);
    TCCR1B &= 0xF8; // Pone cero en bits preescaler PWM 9 y 10
    TCCR1B |= 0x01; // Pone PWM 9 y 10 a frec. de 32 kHz   
    MsTimer2::set(T,arrancar); // Int. T2 cada T ms
    MsTimer2::start();
  }  
      
  void loop() { // El bucle principal no hace nada  
  }
 
  void arrancar(){
    if(digitalRead(boton_1)==LOW){
        canal_1++;
        if(canal_1>3)
        canal_1=0;
        canal=true;
        while(digitalRead(boton_1)==LOW)
          delay(50);
    }
    if(digitalRead(boton_2)==LOW){
        canal_2++;
        if(canal_2>3)
        canal_2=0;
        canal=false;
        while(digitalRead(boton_2)==LOW)
          delay(50);
    }
    if(digitalRead(input_frecuencia)==LOW){
        N=N+50;
        if(N>1000)
          N=50;
        while(digitalRead(input_frecuencia)==LOW)
          delay(50);
    }
    if(i<300){
          potenciometro1=(potenciometro1+map(analogRead(A3),0,1023,0,127))/2;
          potenciometro2=(potenciometro2+map(analogRead(A2),0,1023,0,127))/2;
          i++;
    }
    else {
        pt1=potenciometro1;
        pt2=potenciometro2;
        potenciometro1=0;
        potenciometro2=0;
        i=0;
    }
    
   switch(canal_1){
      case 0:
        seno(pt1,9,A0);
        if(canal==true)
          Serial.println(A_in*Vref/rango);
        break;    
      case 1:
        triangular(pt1,9,A0);
        if(canal==true)
          Serial.println(A_in*Vref/rango);
        break;
      case 2:  
        cuadrada(pt1,9,A0);
        if(canal==true)
          Serial.println(A_in*Vref/rango);
        break;
      case 3:
        continua(pt1,9,A0);
        if(canal==true)
          Serial.println(A_in*Vref/rango);
        break;  
    }
    switch(canal_2){
      case 0:
        seno(pt2,10,A1);
        if(canal==false)
          Serial.println(A_in*Vref/rango);
        break;    
      case 1:
        triangular(pt2,10,A1);
        if(canal==false)
          Serial.println(A_in*Vref/rango);
        break;
      case 2:  
        cuadrada(pt2,10,A1);
        if(canal==false)
          Serial.println(A_in*Vref/rango);
        break;
      case 3:
        continua(pt2,10,A1);
        if(canal==false)
          Serial.println(A_in*Vref/rango);
        break;  
    }  
  }
  
  void seno(int pot,int lec,int ent) {
    if (++dg== Tg) {
      byte ch1;
      dg= 0; // Salidas PWM para generación cada 'Tg*T' ms
      ch1=128+pot*sin(n*2*pi/N);
      analogWrite(lec,ch1);
    }  
    if (++n== N) { // Incrementa número de punto
    n= 0;
    }  
    if (++ds== Ts) {
      ds= 0; // Aquisición de canal cada 'Ts*T' ms
      A_in= analogRead(ent); // Adquiere A0
    }
  }

  void triangular(int pot,int lec,int ent) {
    if (++dg== Tg) {
      byte ch1;
      dg= 0; // Salidas PWM para generación cada 'Tg*T' ms
      if(0<=n && n<(N/4))
        ch1=128 - pot*(0-(4*n/N));
      else if((N/4)<=n && n<(3*N/4))
        ch1=128 + pot*(2-(4*n/N));
      else if((3*N/4)<=n && n<N)
        ch1=128 - pot*(4-(4*n/N));   
      analogWrite(lec, ch1); // Salida de CH1 por pin9 PWM
    }
    if (++n== N) { // Incrementa número de punto
    n= 0;
    }  
    if (++ds== Ts) {
      ds= 0; // Aquisición de canal cada 'Ts*T' ms
      A_in= analogRead(ent); // Adquiere A2
    }
  }

void cuadrada(int pt,int ch,int adq){
    if (++dg== Tg) {
      byte ch1;
      dg= 0; // Salidas PWM para generación cada 'Tg*T' ms
       if (n<= N/2) {
           ch1=128-pt;
           analogWrite(ch, ch1);
      }
      else if (n> N/2) {
          ch1= 128+pt;
          analogWrite(ch, ch1);
      }
    }
    if (++n>= N) { // Incrementa número de punto
      n= 0;
    }
    if (++ds== Ts) {
      ds= 0; // Aquisición de canal cada 'Ts*T' ms
      A_in= analogRead(adq); // Adquiere A0
    }

  }  

void continua(int pot,int lec,int ent){
  if (++dg== Tg) {
      byte ch1;
      dg= 0; // Salidas PWM para generación cada 'Tg*T' ms
      if(n<=N){
        ch1=pot*2;   
        analogWrite(lec, ch1); // Salida de CH1 por pin9 PWM
      }
    }
    if (++n== N) { // Incrementa número de punto
    n= 0;
    }  
    if (++ds== Ts) {
      ds= 0; // Aquisición de canal cada 'Ts*T' ms
      A_in= analogRead(ent); 
    }  
  }
  
