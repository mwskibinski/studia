#include <Wire.h>
#include <LiquidCrystal_PCF8574.h>
#include <DS3231.h>

#define LCD_ADDR 0x27
#define GUZIK_ENTER 3
#define GUZIK_NEXT 4
#define GUZIK_BACK 5

LiquidCrystal_PCF8574 lcd(LCD_ADDR);
DS3231 clock;
bool display_time = true;

void setup() {
  Wire.begin();
  lcd.begin(16, 2);
  lcd.setBacklight(true);

  pinMode(GUZIK_ENTER, INPUT_PULLUP);
  pinMode(GUZIK_NEXT, INPUT_PULLUP);
  pinMode(GUZIK_BACK, INPUT_PULLUP);

  init_display();
}

signed int prev_sec = 0;

void loop() {  
  
  if(guzik(GUZIK_NEXT) || guzik(GUZIK_BACK))
    init_display();
  else if(guzik(GUZIK_ENTER))
    ustaw_date();
  else if(clock.getSecond() != prev_sec) {
    prev_sec = clock.getSecond();
    displayDateTime();
  }
  
}

void displayDateTime() {
  lcd.clear();
  lcd.print(getDateString());
  lcd.setCursor(0, 1);
  lcd.print(getTimeString());
}

String getDateString() {
  bool century;

  String day = toStringWithLeadingZeros(clock.getDate());
  String month = toStringWithLeadingZeros(clock.getMonth(century));
  String year = String(clock.getYear() + 2000);

  return day + "." + month + "." + year + "r.";
}

String getTimeString() {
  bool h12, pm;

  String hour = toStringWithLeadingZeros(clock.getHour(h12, pm));
  String minute = toStringWithLeadingZeros(clock.getMinute());
  String second = toStringWithLeadingZeros(clock.getSecond());

  return hour + ":" + minute + ":" + second;
}

String toStringWithLeadingZeros(byte number) {
  String result = String(number);
  if(number < 10)
    result = "0" + result;

  return result;
}

void init_display() {
  lcd.clear();

  for(byte i = 0; i < 12; i++, delay(50)) {
    lcd.setCursor(i, 0);
    lcd.print("/**/");
    lcd.setCursor(i - 1, 1);
    lcd.print("/**/");
  }
  
  lcd.clear();
}

void ustaw_date() {
  byte temp = 0;
  lcd.clear();

  // ROK
  lcd.clear();
  lcd.print("Rok: ");
  lcd.print(temp + 2000);
  
  while(!guzik(GUZIK_ENTER)) {
    if(guzik(GUZIK_NEXT)) {
      if(temp == 99)
        temp = 0;
      else
        temp++;

      lcd.clear();
      lcd.print("Rok: ");
      lcd.print(temp + 2000);
    }
    else if(guzik(GUZIK_BACK)) {
      if(temp == 0)
        temp = 99;
      else
        temp--;
      
      lcd.clear();
      lcd.print("Rok: ");
      lcd.print(temp + 2000);
    }
  }
  clock.setYear(temp);
  temp = 0;  

  // MIESIAC
  temp = 1;
  lcd.clear();
  lcd.print("Miesiac: ");
  lcd.print(temp);
  while(!guzik(GUZIK_ENTER)) {
    if(guzik(GUZIK_NEXT)) {
      if(temp == 12)
        temp = 1;
      else
        temp++;

      lcd.clear();
      lcd.print("Miesiac: ");
      lcd.print(temp);
    }
    else if(guzik(GUZIK_BACK)) {
      if(temp == 1)
        temp = 12;
      else
        temp--;
      
      lcd.clear();
      lcd.print("Miesiac: ");
      lcd.print(temp);
    }
  }
  clock.setMonth(temp);
  temp = 0;

  // DZIEN
  temp = 1;
  lcd.clear();
  lcd.print("Dzien: ");
  lcd.print(temp);
  while(!guzik(GUZIK_ENTER)) {
    if(guzik(GUZIK_NEXT)) {
      if(temp == 31)
        temp = 1;
      else
        temp++;
      
      lcd.clear();
      lcd.print("Dzien: ");
      lcd.print(temp);
    }
    else if(guzik(GUZIK_BACK)) {
      if(temp == 1)
        temp = 31;
      else
        temp--;

      lcd.clear();
      lcd.print("Dzien: ");
      lcd.print(temp);
    }
  }
  clock.setDate(temp);
  temp = 0;

  // GODZINA
  lcd.clear();
  lcd.print("Godzina: ");
  lcd.print(temp);
  while(!guzik(GUZIK_ENTER)) {  
    if(guzik(GUZIK_NEXT)) {
      if(temp == 23)
        temp = 0;
      else
        temp++;

      lcd.clear();
      lcd.print("Godzina: ");
      lcd.print(temp);
    }
    else if(guzik(GUZIK_BACK)) {
      if(temp == 0)
        temp = 23;
      else
        temp--;

      lcd.clear();
      lcd.print("Godzina: ");
      lcd.print(temp);
    }
  }
  clock.setHour(temp);
  temp = 0;

  // MINUTA
  lcd.clear();
  lcd.print("Minuta: ");
  lcd.print(temp);
  while(!guzik(GUZIK_ENTER)) {
    if(guzik(GUZIK_NEXT)) {
      if(temp == 59)
        temp = 0;
      else
        temp++;

      lcd.clear();
      lcd.print("Minuta: ");
      lcd.print(temp);
    }
    else if(guzik(GUZIK_BACK)) {
      if(temp == 0)
        temp = 59;
      else
        temp--;

      lcd.clear();
      lcd.print("Minuta: ");
      lcd.print(temp);
    }
  }
  clock.setMinute(temp);
  temp = 0;

  // SEKUNDA
  lcd.clear();
  lcd.print("Sekunda: ");
  lcd.print(temp);
  while(!guzik(GUZIK_ENTER)) {  
    if(guzik(GUZIK_NEXT)) {
      if(temp == 59)
        temp = 0;
      else
        temp++;
        
      lcd.clear();
      lcd.print("Sekunda: ");
      lcd.print(temp);
    }
    else if(guzik(GUZIK_BACK)) {
      if(temp == 0)
        temp = 59;
      else
        temp--;

      lcd.clear();
      lcd.print("Sekunda: ");
      lcd.print(temp);
    }
  }
  clock.setSecond(temp);
  lcd.clear();
}

#define DEBOUNCING_LIMIT 50

bool guzik(byte pin) {
  if(digitalRead(pin) == HIGH)
    return false;
  else  
    for(int i = 0; i < DEBOUNCING_LIMIT; i++) {
      if(digitalRead(pin) == HIGH)
        i++;
      else
        i = 0;
    }

  return true;
}