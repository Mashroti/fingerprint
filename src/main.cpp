#include<LiquidCrystal.h>
#include <Adafruit_Fingerprint.h>
#include <IRremote.h>

//SoftwareSerial mySerial(10, 11);
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&Serial1);
LiquidCrystal lcd(12, 13, 14, 15, 16, 17);


#define fingersens  9
#define LED_open    8
#define LED_close   7

#define UP          6375
#define DOWN        19125
#define LEFT        4335
#define RIGHT       23205
#define OK          14535
#define EXIT        -20401
#define opendoor    26775
#define NUM1        -23971
#define NUM2        25245
#define NUM3        -7651
#define NUM4        8925
#define NUM5        765
#define NUM6        -15811
#define NUM7        -7161
#define NUM8        -22441
#define NUM9        -28561


int RECV_PIN = 3; // the pin where you connect the output pin of IR sensor
IRrecv irrecv(RECV_PIN);
decode_results results;

uint8_t getFingerprintEnroll(uint8_t id);
uint8_t deleteFingerprint   (uint8_t id);
int8_t  getFingerprintID    (void); 
int     Check_IR_sensor     (void);
void    menu                (void);
void    ENROLL              (void);
void    DELETE              (void);
void    ADMIN               (void);

void setup()
{
  //Serial.begin(9600);
  //delay(100);

  pinMode(fingersens,INPUT);
  pinMode(LED_close,OUTPUT);
  pinMode(LED_open,OUTPUT);

  lcd.begin(16, 2);
  lcd.clear();
  lcd.print("   Biometric ");
  lcd.setCursor(0, 1);
  lcd.print("Security System");
  delay(2000);
  lcd.clear();

  irrecv.enableIRIn();

  // set the data rate for the sensor serial port
  finger.begin(57600);

  if (!finger.verifyPassword()) 
  {
    //Serial.println("Did not find fingerprint sensor :(");
    lcd.print("Module Not Found");
    lcd.setCursor(0, 1);
    lcd.print("Check Connections");
    while (1) { delay(1); }
  }
}


void loop()
{
  bool x = true;

  while (1)
  {
    if(x)
    {
      lcd.clear();
      lcd.print("  Place Finger");
      x = false;
    }

    int key = Check_IR_sensor();

    if(digitalRead(fingersens) == 0 || key == opendoor)
    {
      int result = getFingerprintID();
      if(result >= 0 || key == opendoor)
      {
        lcd.clear();
        digitalWrite(LED_open, HIGH);
        digitalWrite(LED_close, LOW);
        lcd.setCursor(4, 0);
        lcd.print("Allowed");
        lcd.setCursor(2, 1);
        lcd.print("Gate Opened");
        delay(5000);
        digitalWrite(LED_open, LOW);
        digitalWrite(LED_close, HIGH);
        lcd.clear();
        lcd.setCursor(2, 1);
        lcd.print("Gate Closed");
        delay(2000);
        x = true;
      }
      else
      {
        lcd.clear();
        lcd.setCursor(5, 0);
        lcd.print("ERROR");
        delay(2000);
        x = true;
      }
    }

    if(key == OK) 
    {
      menu();
      x = true;
    }
  }
}

int  Check_IR_sensor(void)
{
  int value = -1;
  if (irrecv.decode(&results))
  {
    value = results.value;
    irrecv.resume();
  }
  return value;
}


void menu(void)
{
  int key = -1;
  bool x = true;

  while (key != EXIT)
  {
    if(x)
    {
      lcd.clear();
      lcd.print("1>Enroll 3>Admin");
      lcd.setCursor(0, 1);
      lcd.print("2>Delete");
      x = false;
    }
    key = Check_IR_sensor();
    switch (key)
    {
      case NUM1:
        ENROLL();
        x = true;
        break;
      case NUM2:
        DELETE();
        x = true;
        break;
      case NUM3:
        ADMIN();
        x = true;
        break;
    } 
  }
}

void ENROLL (void)
{
  int key = -1;
  uint8_t id = 1;
  lcd.clear();
  lcd.print(" Enroll Finger");
  lcd.setCursor(0, 1);
  lcd.print("Finger Code:");
  while (key != EXIT)
  {
    key = Check_IR_sensor();
    switch (key)
    {
      case OK:
        if(id != 0 && id > 0) 
        {
          getFingerprintEnroll(id);
          key = EXIT;
        }
        break;
      case UP:
        id++;
        if(id >= 51) id = 1;
        break;
      case DOWN:
        id--;
        if(id < 1) id = 50;
        break;
    } 
    lcd.setCursor(13, 1);
    lcd.print(id);
    lcd.print(" ");
  }
}

void DELETE (void)
{
  int key = -1;
  uint8_t id = 1;
  lcd.clear();
  lcd.print(" Delete Finger");
  lcd.setCursor(0, 1);
  lcd.print("Finger Code:");
  while (key != EXIT)
  {
    key = Check_IR_sensor();
    switch (key)
    {
      case OK:
        if(id != 0 && id > 0) 
        {
          deleteFingerprint(id);
          key = EXIT;
        }
        break;
      case UP:
        id++;
        if(id >= 51) id = 1;
        break;
      case DOWN:
        id--;
        if(id < 1) id = 50;
        break;
    }

    lcd.setCursor(13, 1);
    lcd.print(id);
    lcd.print(" ");
  }
}

void admin_new(void)
{
  lcd.clear();
  lcd.print(" Remove Finger");

  int p = 0;
  while (p != FINGERPRINT_NOFINGER) {
    p = finger.getImage();
  }

  lcd.clear();
  lcd.print("Place New Finger");
  getFingerprintEnroll(0);
}
void ADMIN (void)
{
  int key = -1;
  bool x = true;

  while (key != EXIT)
  {
    key = Check_IR_sensor();

    if(x)
    {
      lcd.clear();
      lcd.print("PlaceAdminFinger");
      x = false;
    }

    if(digitalRead(fingersens) == 0)
    {
      int result = getFingerprintID();
      switch (result)
      {
        case 0:
          admin_new();
          key = EXIT;
          break;
        default:
          lcd.clear();
          lcd.print(" your not Admin");
          delay(2000);
          x = true;
          break;
      }
    }
  }
}


uint8_t getFingerprintEnroll(uint8_t id)
{
  int key = -1, p = -1;
  //Serial.print("Waiting for valid finger to enroll as #"); Serial.println(id);
  lcd.clear();
  lcd.print("  Place Finger");
  while (p != FINGERPRINT_OK) 
  {
    key = Check_IR_sensor();
    if(key == EXIT) return 0;
    
    p = finger.getImage();
    if(p == FINGERPRINT_OK) break;
  }

  // OK success!

  p = finger.image2Tz(1);
  if(p != FINGERPRINT_OK) 
  {
    lcd.clear();
    lcd.setCursor(6, 1);
    lcd.print("ERROR!");
    delay(2000);
    return p;
  }


  //Serial.println("Remove finger");
  lcd.clear();
  lcd.print(" Remove Finger");
  delay(2000);
  p = 0;
  while (p != FINGERPRINT_NOFINGER) 
  {
    p = finger.getImage();
    key = Check_IR_sensor();
    if(key == EXIT) return 0;
  }
  //Serial.print("ID "); Serial.println(id);
  p = -1;
  lcd.clear();
  lcd.print("PlaceFingerAgain");

  //Serial.println("Place same finger again");
  while (p != FINGERPRINT_OK) 
  {
    p = finger.getImage();
    key = Check_IR_sensor();
    if(key == EXIT) return 0;

    if(p == FINGERPRINT_OK) break;
  }

  // OK success!

  p = finger.image2Tz(2);
  if(p != FINGERPRINT_OK)
  {
    lcd.clear();
    lcd.setCursor(6, 1);
    lcd.print("ERROR!");
    delay(2000);
    return p;
  }

  // OK converted!
  //Serial.print("Creating model for #");  Serial.println(id);

  p = finger.createModel();
  if(p != FINGERPRINT_OK)
  {
    lcd.clear();
    lcd.setCursor(6, 1);
    lcd.print("ERROR!");
    delay(2000);
    return p;
  }

  //Serial.print("ID "); Serial.println(id);
  p = finger.storeModel(id);
  if (p == FINGERPRINT_OK) {
    lcd.clear();
    lcd.setCursor(4, 0);
    lcd.print("Stored!");
    delay(1000);
    //Serial.println("Stored!");
  } 
  else 
  {
    lcd.clear();
    lcd.setCursor(6, 1);
    lcd.print("ERROR!");
    delay(2000);    
    return p;
  }

  return true;
}


uint8_t deleteFingerprint(uint8_t id) 
{
  uint8_t p = -1;
  p = finger.deleteModel(id);
  
  if (p == FINGERPRINT_OK) 
  {
    lcd.clear();
    lcd.setCursor(4, 1);
    lcd.print("Deleted!");
    delay(1000);
    //Serial.println("Deleted!");
  } 
  else 
  {
    lcd.clear();
    lcd.setCursor(6, 1);
    lcd.print("ERROR!");
    delay(2000);
  }
}

int8_t getFingerprintID(void) 
{
  int8_t ret = -1;
  uint8_t p = finger.getImage();

  if (p == FINGERPRINT_OK)
  {
    p = finger.image2Tz();
    if (p == FINGERPRINT_OK)
    {
      p = finger.fingerFastSearch();
      if (p == FINGERPRINT_OK)
      {
        ret = finger.fingerID;
      }
    }
  }
  return ret;
}