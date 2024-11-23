#include <SoftwareSerial.h>
#include <ESP8266WiFi.h>
#include <ESP_Mail_Client.h>

// Declare the global used SMTPSession object for SMTP transport
SMTPSession smtp;

// Declare the global used Session_Config for user defined session credentials
Session_Config config;

// Declare the SMTP_Message class variable to handle to message being transport
SMTP_Message message;

// Software serial to communicate with STM32
SoftwareSerial stmSerial(5, 4);

// Delay between POST
unsigned long lastTime = 0;
unsigned long timerDelay = 10000;

// Delay between mails
unsigned char sendMail = 0;

unsigned long lastMail = 0;
unsigned long mailDelay = 60000;

// Function declaration
void smtpCallback(SMTP_Status status);

void setup() {
  Serial.begin(115200);
  Serial.println("Connected to computer serial console");
  
  // Serial connection to STM32
  stmSerial.begin(9600);

  // Connect to wifi
  WiFi.begin("Valentin LLV", "valentinllv");
  Serial.print("Connecting to Wi-Fi");

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(300);
  }
  
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  // Set SMTP session config
  config.server.host_name = "smtp.sorbonne-universite.fr"; // for outlook.com
  config.server.port = 587; // for TLS with STARTTLS or 25 (Plain/TLS with STARTTLS) or 465 (SSL)
  config.login.email = "21315107@etu.sorbonne-universite.fr"; // set to empty for no SMTP Authentication
  config.login.password = "Valentin2003!"; // set to empty for no SMTP Authentication

  // Config STMP server
  config.time.ntp_server = "pool.ntp.org,time.nist.gov";
  config.time.gmt_offset = 3;
  config.time.day_light_offset = 0;

  // Set the message headers
  message.sender.name = "Valentin Le Lievre";
  message.sender.email = "valentin.le_lievre@etu.sorbonne-universite.fr";
  message.subject = "Test sending Email";
  message.addRecipient("Valentin Le Lievre", "lelievre.valentin17@gmail.com");

  // Set debug option
  smtp.debug(1);

  // Set the callback function to get the sending results
  smtp.callback(smtpCallback);

  // // Connect to TTN server
  // while(counter <= 8) {
  //   Serial.println(data[counter].c_str());
  //   loraModuleSerial.println(data[counter].c_str());

  //   delay(500);
  //   while(loraModuleSerial.available()) { Serial.write(loraModuleSerial.read()); }

  //   counter += 1;
  // }

  // char connected = 0;
  // while(!connected) {
  //   if(loraModuleSerial.available()) {
  //     delay(1000);
  //     while(loraModuleSerial.available()) Serial.write(loraModuleSerial.read());
  //     connected = 1;
  //   }
  // }

  Serial.println("Ready");
}

void smtpSendMail() {
  Serial.println("Sending mail");

  // Connect to the server
  smtp.connect(&config);

  // Start sending Email and close the session
  if(!MailClient.sendMail(&smtp, &message)) Serial.println("Error sending Email, " + smtp.errorReason());
}

void smtpCallback(SMTP_Status status) {
  Serial.println(status.info());
}

void loop() {
  if(sendMail && millis() - lastMail > mailDelay) {
    smtpSendMail();
    lastMail = millis();

    sendMail = 0;
  } else sendMail = 0;

  while(stmSerial.available()) {
    String message = stmSerial.readString();
    Serial.println(message);

    if(message == "sendMail") {
      Serial.println("Recieve send mail request");
      sendMail = 1;
    }
  }
}