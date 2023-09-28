//Declaramos el pin que encendera
int pin_veintiuno = 21;
//Iniciamos los pines del ESP32
void setup() {
  //Declaramos que el pin del led es de tipo salida, osea que la señal va salir
  pinMode(pin_veintiuno, OUTPUT);
}
//Iniciamos la funcion bucle que se repetira indefinidamente
void loop() {
  //Encendemos el led
  digitalWrite(pin_veintiuno, HIGH);
  //Esperamos un segundo
  delay(1000);
  //Apagamos el led
  digitalWrite(pin_veintiuno, LOW);
  //Esperamos un segundo
  delay(1000);
}