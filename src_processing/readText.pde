import processing.serial.*;  // Importer la bibliothèque Serial pour la communication série

Serial myPort;  // Objet Serial pour la communication avec le port série

boolean isText = false;

void setup() {
  surface.setVisible(false);
  
  // Imprimer la liste des ports série disponibles
  // Cela aide à identifier quel port est utilisé par le microcontrôleur
  printArray(Serial.list());
  
  // Ouvrir le port série pour la communication (remplacer par le bon port si nécessaire)
  String portName = Serial.list()[1];  // Utiliser le premier port série trouvé
  myPort = new Serial(this, portName, 75000);
}

void keyPressed() {}
void draw() {}

void serialEvent(Serial myPort) {
  int inByte = myPort.read();  // Lire l'octet reçu
  
  if(inByte == 36) isText = true;
  else if(inByte == 10) {
    isText = false;
    print('\n');
  } else if(isText == true) print(Character.toString ((char) inByte));
  else print(inByte);
}
