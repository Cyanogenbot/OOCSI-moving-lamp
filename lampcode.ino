#include <ArduinoHttpClient.h>
#include <Adafruit_NeoPixel.h>
#include <SPI.h>
#include <OOCSI.h>
#include <OOCSIDevice.h>
#include <SpeedyStepper.h>

#define LED_PINR     15
#define LED_PINL     25
#define LED_COUNTL  78
#define LED_COUNTR  78

// define ledstrips
Adafruit_NeoPixel stripL(LED_COUNTL, LED_PINL, NEO_GRBW + NEO_KHZ800);
Adafruit_NeoPixel stripR(LED_COUNTR, LED_PINR, NEO_GRBW + NEO_KHZ800);

int lastColour[] = {255, 255, 255, 255};
int brightnessNormal;
int brightnessNormalOld;
int brightnessWhite;
int brightnessWhiteOld;

//define stepper
SpeedyStepper stepper;
const int EN_PIN = 7;
const int MOTOR_STEP_PIN = 11;
const int MOTOR_DIRECTION_PIN = 12;

//define oocsi
OOCSI oocsi = OOCSI();


// put your setup code here, to run once:
void setup() {

  // Disable steppers
  pinMode(EN_PIN, OUTPUT);
  digitalWrite(EN_PIN, HIGH); 

  // Start strips and serial
  Serial.begin(9600);
  stripL.begin();
  stripR.begin();


  // Setup stepper
  stepper.connectToPins(MOTOR_STEP_PIN, MOTOR_DIRECTION_PIN);

  // Set up oocsi server
  oocsi.connect("NAME", "oocsi_server_ip", "SSID", "WIFIpassword", processOOCSI);

  // Create and send heyoocsi device
  OOCSIDevice prototype = oocsi.heyOOCSI("device_name");

  // Add signature to HA
  prototype.property("creator", "Diede!");

  // Add light
  prototype.addLight(
    "MyFirstLight",
    "MyFirstLightChannel",
    "RGBW",
    LIGHT_SPECTRUM_RGB_CCT_WHITE,
    "True",
    100,
    "desklamp"
  );

  // Send heyoocsi
  prototype.sayHi();

  // Listen to oocsi channel messages
  oocsi.subscribe("MyFirstLightChannel");
}
// put your main code here, to run repeatedly:
void loop() {
  oocsi.check();
  delay(500);
}
void processOOCSI() {
  Serial.println("open");

  Serial.println("stepperset");


// still broken :(
  // Toggle leds on or off, and use last used colour
  // if (oocsi.has("state")){
  //   String State = oocsi.getString("state","False");
  //   if (State == "True"){
  //     stripL.setBrightness(100);
  //     // stepper.moveRelativeInSteps(-20000);
  //     stripR.setBrightness(100);
  //     for (int i = 0; i < LED_COUNTL; i++){
  //       stripL.setPixelColor(i, stripL.Color(lastColour[0], lastColour[1], lastColour[2], lastColour[3]));
  //       Serial.println("leftcol");
  //       stripR.setPixelColor(i, stripR.Color(lastColour[0], lastColour[1], lastColour[2], lastColour[3]));
  //       Serial.println("rightcol");

  //       digitalWrite(EN_PIN, LOW); 

  //       // stepper.moveToPositionInRevolutions(-3.5);

  //       digitalWrite(EN_PIN, HIGH); 
  //       }
  //     }
  //   else{
  //     stripL.setBrightness(0);
  //     stripR.setBrightness(0);
  //     // stepper.moveRelativeInSteps(20000);
  //     }
  //   }

    
  

  // Change colour on demand
  if (oocsi.has("colorrgbw")){
    int standardarray[] = {255, 255, 255, 255};
    // int results[] = {0, 0, 0, 0};
    oocsi.getIntArray("colorrgbw", standardarray, lastColour, 4);
    // lastColour = results[];

    // Change strip colour on command
    for (int colors = 0; colors < LED_COUNTL; colors++){
      stripL.setPixelColor(colors, stripL.Color(lastColour[0], lastColour[1], lastColour[2], lastColour[3]));
      Serial.println("leftcol");
      stripR.setPixelColor(colors, stripR.Color(lastColour[0], lastColour[1], lastColour[2], lastColour[3]));
      Serial.println("rightcol");
      }
  }

  // if (oocsi.has("colortemp in rgb)){
  //   int standardarray[] = {255, 255, 255, 255};
  //   int results[] = {0, 0, 0,0};
  //   oocsi.getIntArray("colorrgbw", standardarray, results, 4);
  //   lastColour = results
  //   for (int i=0; i< LED_COUNTL;i++){
  //     stripL.setPixelColor(i, stripL.Color(results[0], results[1], results[2], results[3]));
  //     Serial.println("leftcol");
  //     stripR.setPixelColor(i, stripR.Color(results[0], results[1], results[2], results[3]));
  //     Serial.println("rightcol");}
  // }


  // Change brightness on demand
  if (oocsi.has("brightness")){
    Serial.println("brightness");
    brightnessNormal = oocsi.getInt("brightness", 0);
    
    // Set strip brightness
    stripL.setBrightness(brightnessNormal);
    Serial.println("leftbright");
    stripR.setBrightness(brightnessNormal);
    Serial.println("rightbright");
    stripL.show();
    stripR.show();
    digitalWrite(EN_PIN, LOW);
    // Serial.println("pin low");

    // Move steppers according to brightness
    stepper.setStepsPerMillimeter(1600/8);
    stepper.setSpeedInMillimetersPerSecond(40);
    stepper.setAccelerationInMillimetersPerSecondPerSecond(30);
    int distance = (260 / 255 * brightnessNormal);
    stepper.moveToPositionInMillimeters(distance);
    Serial.println("turn");
    digitalWrite(EN_PIN, HIGH); 
    brightnessNormalOld = brightnessNormal;
    // Serial.println("pin high");
  }

  // Change to white mode brightness on demand
  if (oocsi.has("brightnessWhite")){
    
    // Set white mode brightness
    int brightnessWhite = oocsi.getInt("brightnessWhite", 0);
    stripL.setBrightness(brightnessWhite);
    Serial.println("leftbrightwhite");
    stripR.setBrightness(brightnessWhite);
    Serial.println("rightbrightwhite");

    // Change colors to white
    for (int i2 = 0; i2 < LED_COUNTL; i2 ++){
      stripL.setPixelColor(i2, stripL.Color(0,0,0, 255));
      Serial.println("leftwhite");
      stripR.setPixelColor(i2, stripR.Color(0,0,0, 255));
      Serial.println("rightwhite");
      }
  }
  stripL.show();
  stripR.show();
  
}
