#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include <MCP3008.h>
#include <LCD.h>
#include <LiquidCrystal_I2C.h>

#define CS_PIN 12
#define CLOCK_PIN 9
#define MOSI_PIN 11
#define MISO_PIN 10

// Global Variables
// __________________________________________________________________________________________________
// Tri-color LED pins
const int ciLedRedPin = 7; //Red
const int ciLedGreenPin = 6; //Green
const int ciLedBluePin = 5; //Blue
const long interval = 1000;
const int ciDemoPeriod = 60;  //Interval of each running period
const int ciRestPeriod = 15;  //Interval of each resting period before the demo mode can be run again.

// Create global timer variables
const long blinkRate = 250;

int servodata[][4] = {  // Min, Max, Home, current position
  {95, 460, 277, 277}, // Base servo
  {130, 380, 330, 330}, // Shoulder servo
  {270, 460, 445, 445}, // Elbow
  {95, 460, 170, 170}, // Wrist 1
  {250, 420, 350, 350}, // Gripper
  {130, 400, 275, 275} // Wrist 2
};

// Create global status variable
int systemStatus = 1;
unsigned long previousMillis = 0;
int thisSecond = ciDemoPeriod;


/*Object Init***********************************************************************************************************/
// Connect to the LCD via i2c, default address #0 (A0-A2 not jumpered)
LiquidCrystal_I2C  lcd(0x3F,2,1,0,4,5,6,7); 
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();
MCP3008 adc(CLOCK_PIN, MOSI_PIN, MISO_PIN, CS_PIN);

void initLEDs()
{
  int td1 = 200;
  int td2 = 100;
  
  // Test RGB LED on startup
  digitalWrite(ciLedRedPin, LOW);
  digitalWrite(ciLedGreenPin, LOW);
  digitalWrite(ciLedBluePin, LOW);

  // Test RED
  digitalWrite(ciLedRedPin, HIGH);
  digitalWrite(ciLedGreenPin, LOW);
  digitalWrite(ciLedBluePin, LOW);
  delay(td1);
  digitalWrite(ciLedRedPin, LOW);
  digitalWrite(ciLedGreenPin, LOW);
  digitalWrite(ciLedBluePin, LOW);
  delay(td2);
  
  // Test GREEN
  digitalWrite(ciLedRedPin, LOW);
  digitalWrite(ciLedGreenPin, HIGH);
  digitalWrite(ciLedBluePin, LOW);
  delay(td1);
  digitalWrite(ciLedRedPin, LOW);
  digitalWrite(ciLedGreenPin, LOW);
  digitalWrite(ciLedBluePin, LOW);
  delay(td2);

  // Test BLUE
  digitalWrite(ciLedRedPin, LOW);
  digitalWrite(ciLedGreenPin, LOW);
  digitalWrite(ciLedBluePin, HIGH);
  delay(td1);
  digitalWrite(ciLedRedPin, LOW);
  digitalWrite(ciLedGreenPin, LOW);
  digitalWrite(ciLedBluePin, LOW);
  delay(td2);

  // Test YELLOW
  digitalWrite(ciLedRedPin, HIGH);
  digitalWrite(ciLedGreenPin, HIGH);
  digitalWrite(ciLedBluePin, LOW);
  delay(td1);
  digitalWrite(ciLedRedPin, LOW);
  digitalWrite(ciLedGreenPin, LOW);
  digitalWrite(ciLedBluePin, LOW);
  delay(td2);
}

void goHome(){
  
  for (uint8_t servonum = 6; servonum > 0; servonum--){
    pwm.setPWM(servonum-1, 0, servodata[servonum-1][2]);
    servodata[servonum-1][3] == servodata[servonum-1][2];
    delay(750);
  }
  
}

void displayStatus(int state) {
  //Serial.println(state);
  unsigned long previousMillis = 0;
  int ledState = LOW;
  systemStatus = state;
  int x=0;
  switch (state) {
    case 1: //Initializing State 'Blue'
      digitalWrite(ciLedRedPin, LOW);
      digitalWrite(ciLedGreenPin, LOW);
      digitalWrite(ciLedBluePin, HIGH);
      lcd.home ();
      lcd.clear();
      lcd.print("Initializing...");
      break;
    case 2: //Run State 'Green'
      digitalWrite(ciLedRedPin, LOW);
      digitalWrite(ciLedGreenPin, HIGH);
      digitalWrite(ciLedBluePin, LOW);
      lcd.home ();
      lcd.clear();
      lcd.print("** Demo  Mode **");
      lcd.setCursor(0,1);
      lcd.print("Run  Time:");
      break;
    case 3: //Fatal Error on Initialization 'Blinking Blue'
      Serial.println("error");
      digitalWrite(ciLedRedPin, LOW);
      digitalWrite(ciLedGreenPin, LOW);
      digitalWrite(ciLedBluePin, HIGH);
      lcd.home ();
      lcd.print("Error           ");
      while(x==0){
        unsigned long currentMillis = millis();
        if(currentMillis - previousMillis >= blinkRate) {
          // reset the previous timer variable to the current time for the next loop around 
          previousMillis = currentMillis;
          
          // set the ledState variable to the opposite of whatever value it currently is
          if (ledState == HIGH)
            ledState = LOW;
          else
            ledState = HIGH;
      
          // set the LED with the ledState of the variable:
          digitalWrite(ciLedBluePin, ledState);
        } // Put controller into endless loop until reset
      }
      break;
    case 4: //Run State Time Low - 'Yellow'
      digitalWrite(ciLedRedPin, HIGH);
      digitalWrite(ciLedGreenPin, HIGH);
      digitalWrite(ciLedBluePin, LOW);
      break;
    case 5: //Run State Fatal Error 'Blinking Red'
      Serial.println("error");
      digitalWrite(ciLedRedPin, HIGH);
      digitalWrite(ciLedGreenPin, LOW);
      digitalWrite(ciLedBluePin, LOW);
      while(x==0){
        unsigned long currentMillis = millis();
        if(currentMillis - previousMillis >= blinkRate) {
          // reset the previous timer variable to the current time for the next loop around 
          previousMillis = currentMillis;
          
          // set the ledState variable to the opposite of whatever value it currently is
          if (ledState == HIGH)
            ledState = LOW;
          else
            ledState = HIGH;
      
          // set the LED with the ledState of the variable:
          digitalWrite(ciLedRedPin, ledState);
        } // Put controller into endless loop until reset
      }
      break;
    case 6: //Run State Resting
      digitalWrite(ciLedRedPin, HIGH);
      digitalWrite(ciLedGreenPin, LOW);
      digitalWrite(ciLedBluePin, LOW);
      lcd.home ();
      lcd.clear();
      lcd.print("** Demo  Mode **");
      lcd.setCursor(0,1);
      lcd.print("Rest Time:");
      break;
    case 7: //Run State waiting 'Green'
      digitalWrite(ciLedRedPin, LOW);
      digitalWrite(ciLedGreenPin, HIGH);
      digitalWrite(ciLedBluePin, LOW);
      lcd.home ();
      lcd.clear();
      lcd.print("** Demo  Mode **");
      lcd.setCursor(0,1);
      lcd.print("     Ready!");
      break;
  }
}


void setup() {
  //Serial.begin(9600);
  //Serial.println("Robotic Arm test 1");
  
  // initialize digital pins as outputs.
  pinMode (ciLedRedPin, OUTPUT); 
  pinMode (ciLedGreenPin, OUTPUT);
  pinMode (ciLedBluePin, OUTPUT);
  
  // set up the LCD's number of rows and columns: 
  lcd.begin(16, 2);
  lcd.setBacklightPin(3,POSITIVE);
  lcd.setBacklight(HIGH);
  initLEDs();

  displayStatus(1);
  
  pwm.begin();
  pwm.setPWMFreq(50);

  goHome();
  
  displayStatus(7);

}

void loop() {
  // put your main code here, to run repeatedly:
  //int val = adc.readADC(0);
  //Serial.println(thisSecond);
  unsigned long currentMillis = millis();
  int val;
  switch (systemStatus){
    case 2:
        //int val = 0;
        if (currentMillis - previousMillis >= interval) {
        previousMillis = currentMillis;
        thisSecond--;
        if (thisSecond > 9) {
          lcd.setCursor ( 12,1 );
        }else{
          displayStatus(4);
          lcd.setCursor ( 12,1 );
          lcd.print(" ");
          lcd.setCursor ( 13,1 );
        }
          lcd.print(thisSecond);
        }

      for(int x=0; x<6;x++){
        val = adc.readADC(x);
        if (val < 400){
          if (servodata[x][3]>servodata[x][0]){
            servodata[x][3]--;
            pwm.setPWM(x, 0, servodata[x][3]);
          }
        }
        if (val >600){
          if (servodata[x][3]<servodata[x][1]){
            servodata[x][3]++;
            pwm.setPWM(x, 0, servodata[x][3]);
          }
        }
      }
      delay(5);
    break;
    case 4:
      if (currentMillis - previousMillis >= interval) {
        previousMillis = currentMillis;
        thisSecond--;
        if (thisSecond > 0){
          lcd.setCursor ( 12,1 );
          lcd.print(" ");
          lcd.setCursor ( 13,1 );
          lcd.print(thisSecond);
        } else{
          thisSecond=ciRestPeriod;
          displayStatus(6);
        }
      }
      for(int x=0; x<6;x++){
        val = adc.readADC(x);
        if (val < 400){
          if (servodata[x][3]>servodata[x][0]){
            servodata[x][3]--;
            pwm.setPWM(x, 0, servodata[x][3]);
          }
        }
        if (val >600){
          if (servodata[x][3]<servodata[x][1]){
            servodata[x][3]++;
            pwm.setPWM(x, 0, servodata[x][3]);
          }
        }
      }
      delay(5);
      break;
    case 6:
      if (currentMillis - previousMillis >= interval) {
        previousMillis = currentMillis;
        thisSecond--;
        if (thisSecond > 0){
          if (thisSecond > 9) {
            lcd.setCursor ( 12,1 );
          }else{
            lcd.setCursor ( 12,1 );
            lcd.print(" ");
            lcd.setCursor ( 13,1 );
          }
          lcd.print(thisSecond);
        } else{
          thisSecond=ciDemoPeriod;
          displayStatus(7);
        }
        
      }
      break;
    case 7:
      for(int x=0; x<6;x++){
        int val = adc.readADC(x);
        //Serial.print (val);
        //Serial.print ("   ");
        if (val < 400 || val > 600){
          displayStatus(2);
          //thisSecond=ciDemoPeriod;
          break;
        }
      }
      
      break;
  }
}

