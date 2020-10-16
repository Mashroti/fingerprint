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
#define OK          5
#define EXIT        5
#define NUM1        7
#define NUM2        8
#define NUM3        9


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

    if(digitalRead(fingersens) == 0)
    {
      int result = getFingerprintID();
      if(result >= 0)
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

    int key = Check_IR_sensor();
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
  int key;
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
  int key;
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
  int key;
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
  int key;
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
  int key, p = -1;
  //Serial.print("Waiting for valid finger to enroll as #"); Serial.println(id);
  lcd.clear();
  lcd.print("  Place Finger");
  while (p != FINGERPRINT_OK) 
  {
    key = Check_IR_sensor();
    if(key == EXIT) return 0;
    
    if(digitalRead(fingersens) == 0)
    {
      p = finger.getImage();
      switch (p) 
      {
        case FINGERPRINT_OK:
          //Serial.println("Image taken");
          break;
        case FINGERPRINT_NOFINGER:
          //Serial.println(".");
          break;
        case FINGERPRINT_PACKETRECIEVEERR:
          //Serial.println("Communication error");
          break;
        case FINGERPRINT_IMAGEFAIL:
          //Serial.println("Imaging error");
          break;
        default:
          //Serial.println("Unknown error");
          break;
      }
    }
  }

  // OK success!

  p = finger.image2Tz(1);
  switch (p) 
  {
    case FINGERPRINT_OK:
      //Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      //Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      //Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      //Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      //Serial.println("Could not find fingerprint features");
      return p;
    default:
      //Serial.println("Unknown error");
      return p;
  }

  //Serial.println("Remove finger");
  lcd.clear();
  lcd.print(" Remove Finger");
  delay(2000);
  p = 0;
  while (p != FINGERPRINT_NOFINGER) {
    p = finger.getImage();
  }
  //Serial.print("ID "); Serial.println(id);
  p = -1;
  lcd.clear();
  lcd.print("PlaceFingerAgain");
  //Serial.println("Place same finger again");
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
    case FINGERPRINT_OK:
      //Serial.println("Image taken");
      break;
    case FINGERPRINT_NOFINGER:
      //Serial.print(".");
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      //Serial.println("Communication error");
      break;
    case FINGERPRINT_IMAGEFAIL:
      //Serial.println("Imaging error");
      break;
    default:
      //Serial.println("Unknown error");
      break;
    }
  }

  // OK success!

  p = finger.image2Tz(2);
  switch (p) {
    case FINGERPRINT_OK:
      //Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      //Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      //Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      //Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      //Serial.println("Could not find fingerprint features");
      return p;
    default:
      //Serial.println("Unknown error");
      return p;
  }

  // OK converted!
  //Serial.print("Creating model for #");  Serial.println(id);

  p = finger.createModel();
  if (p == FINGERPRINT_OK) {
    //Serial.println("Prints matched!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    //Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_ENROLLMISMATCH) {
    //Serial.println("Fingerprints did not match");
    return p;
  } else {
    //Serial.println("Unknown error");
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
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    //Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_BADLOCATION) {
    //Serial.println("Could not store in that location");
    return p;
  } else if (p == FINGERPRINT_FLASHERR) {
    //Serial.println("Error writing to flash");
    return p;
  } else {
    //Serial.println("Unknown error");
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
  else if (p == FINGERPRINT_PACKETRECIEVEERR) 
  {
    //Serial.println("Communication error");
    return p;
  } 
  else if (p == FINGERPRINT_BADLOCATION) 
  {
    //Serial.println("Could not delete in that location");
    return p;
  } 
  else if (p == FINGERPRINT_FLASHERR)
  {
    //Serial.println("Error writing to flash");
    return p;
  } 
  else 
  {
    //Serial.print("Unknown error: 0x"); Serial.println(p, HEX);
    return p;
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