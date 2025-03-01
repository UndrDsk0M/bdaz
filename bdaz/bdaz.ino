#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Keyboard.h>
#include <Mouse.h> 

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels 

#define OLED_RESET    -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C 
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET); 

const int xPin = A5;
const int yPin = A0;
// const int buttonPin = 8; 

byte page = 0;
const String menu_list[] = {"Mouse", "Keyboard", "Super Keyboard", "Brute Forcer", "Auto Clicker"};
int section = 0; 

// mouse part 


// keyboatd part 
int col;
int line;
int current_char = 97;
String simple; 
bool enter_key = false;

// super keyboard 
int col_s;
int line_s = 20;
int current_char_s = 194;
bool pressed_key = false;

// brute forcer
byte page_bf = 1;
int pass_range = 4;
int maximum;

// auto clicker
byte page_ac = 1;
unsigned int delay_time = 1000;
byte counter = 5;
unsigned int clicks;

void setup()
{
 Serial.begin(9600);
 if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
 Serial.println(F("SSD1306 allocation failed"));
 for(;;); // Don't proceed, loop forever
 } 

 Mouse.begin() ;
 Keyboard.begin();
 pinMode(xPin, INPUT);
 pinMode(yPin, INPUT);
 pinMode(8, INPUT_PULLUP); 
 display.display();
 delay(1500); 
} 

void loop() {     

 display.clearDisplay();
 String xDir;
 int xPosition = analogRead(xPin);
 if (xPosition <= 200) xDir = "LEFT";
 else if (xPosition >= 600) xDir = "RIGHT";
 String yDir;
 int yPosition = analogRead(yPin);
 if (yPosition <= 200) yDir = "DOWN";
 else if (yPosition >= 600) yDir = "UP";
 Serial.print("y: ");   
 Serial.println(yPosition);
 Serial.print("x: ");  
 Serial.println(xPosition);
  
 int buttonState = digitalRead(8);
 
 if (buttonState == 0 ){
   if (xDir == "LEFT") {
     page = 0;
     return;
   }
 }
 
 if (page == 0){
   display.setTextColor(WHITE);
   display.setCursor(0,0);
   display.setTextSize(1);
 
   if ( yDir == "DOWN" ) {section--;}
   else if ( yDir == "UP" ) {section++;}
   if (buttonState == 0){
     if (section == 0) page = 1;
     else if (section == 1) page = 2;
     else if (section == 2) page = 3;
     else if (section == 3) page = 4;
     else if (section == 4) page = 5;
     
     else section = 0;
  }
  for (byte i = 0; i < 5; i++){
    String item_text = menu_list[i];
    if (section == i){
       item_text = item_text + " *"; 
    }
    display.println(item_text);
  }
  display.display();
  delay(150);
  } 
  else if (page == 1){
   display.setTextColor(WHITE);
   display.setCursor(0,0);
   display.setTextSize(2);
   display.println("- Mouse"); 
   display.setTextSize(1);
   display.setCursor(40, 30);
   display.println(xDir);
   display.setCursor(80, 30);
   display.println(yDir);
   display.setCursor(60, 50);
   display.println(buttonState);
 
   if (buttonState == 0 ){
     if (yDir == "UP"){
       Mouse.move(0,0, 10);
     }
     else if (yDir == "DOWN"){
       Mouse.move(0,0, -10);
     }
     else{
       Mouse.click();
     }
    }
   else {
     if (xDir == "LEFT") Mouse.move(-10, 0);
     if (xDir == "RIGHT") Mouse.move(10, 0);
     if (yDir == "UP") Mouse.move(0, -10);
     if (yDir == "DOWN") Mouse.move(0, 10);
   }
  display.display();
  delay(10);
  }
  
  else if (page == 2){
    display.setTextColor(WHITE);
    display.setCursor(0,0);
    display.setTextSize(1);
    display.print(simple);
    line = 97;
    col = 20;
    if (xDir == "LEFT") current_char++;
    else if (xDir == "RIGHT") current_char--;
 
    else if (yDir == "DOWN") {
      if (current_char > 103) current_char -= 7;
    } 
    else if (yDir == "UP") {
      if (current_char < 118) current_char += 7;
    }
    else if (buttonState == 0) {
      if (current_char == 123){ 
        if (enter_key == false){
           Keyboard.print(' '); // space
           simple += ' ';
           enter_key = true;
           if (pressed_key) {
             Keyboard.releaseAll();
             pressed_key = false;
           }
        }
        else {
           Keyboard.write(13); // enter 
           simple = "";
           enter_key = false;
        }
      }
      else if (current_char == 124) Keyboard.write(8); // delete button
      else {
        Keyboard.write(current_char);
        enter_key = false;
        simple += char(current_char);
      }
    }

    if (current_char <= 103 and current_char >= 97) {
      int x = ((current_char - 97 )*20)-2;
      display.drawRoundRect(x, 20, 10, 10, 2, WHITE); 
    }
    else if (current_char <= 110 and current_char >= 104) {
      int x = ((current_char - 104 )*20)-2;
      display.drawRoundRect(x, 30, 10, 10, 2, WHITE); 
    }
    else if (current_char <= 117 and current_char >= 111) {
      int x = ((current_char - 111 )*20)-2;
      display.drawRoundRect(x, 40, 10, 10, 2, WHITE); 
    }
    else if (current_char <= 124 and current_char >= 118) {
      int x = ((current_char - 118 )*20)-2;
      display.drawRoundRect(x, 50, 10, 10, 2, WHITE); 
    }
    else {current_char =97;}
 
    for (byte c=97; c<=124; c++){
      if ((c == 104) or (c == 111) or (c == 118)) {
        col += 10;
        line = c;
      }
      byte x = (c - line) * 20;
      display.setCursor(x, col);
      if (c == 123) display.write('-'); 
      else if (c==124) display.write('<');
      else {
        display.write(c);
      }
    }

    display.display();
    delay(100);
  }
  
  else if (page == 3){
    display.setTextColor(WHITE);
    display.setTextSize(1);
        
    if (xDir == "LEFT") current_char_s++;
    else if (xDir == "RIGHT") current_char_s--; 
    else if (yDir == "UP") {
      if (current_char_s <= 205) current_char_s += 6;
    }
    else if (yDir == "DOWN") {
      if (current_char_s >= 201) current_char_s -= 6;
    }
    else if (buttonState == 0) {
      if (current_char_s == 209){ 
        // alt
        Keyboard.press(KEY_LEFT_ALT) ;
        //delay(10);
        //Keyboard.release(130);
      }
      else if (current_char_s == 207){ 
        // ctrl
        Keyboard.press(KEY_LEFT_CTRL);
      }
      else if (current_char_s == 208){ 
        // win
        Keyboard.press(KEY_LEFT_GUI);
      }
      else if (current_char_s == 206){ 
        // tab
        Keyboard.press(KEY_TAB);
      }
      else if (current_char_s == 210){
        // shift
        Keyboard.press(KEY_RIGHT_SHIFT);
      }
      else if (current_char_s == 211){
        // release button
        Keyboard.releaseAll();
      }
      else{
        Keyboard.press(current_char_s);
        delay(10);
        Keyboard.release(current_char_s);
      }
    }
    if (current_char_s >= 194 and current_char_s <= 199) {
      int x = ((current_char_s - 194)*20)-2;
      display.drawRoundRect(x, 20, 17, 10, 2, WHITE); 
    }
    else if (current_char_s >= 200 and current_char_s <= 205) {
      int x = ((current_char_s - 200 )*20)-2;
      display.drawRoundRect(x, 30, 17, 10, 2, WHITE); 
    }
    else if (current_char_s >= 206 and current_char_s <= 211) {
      int x = ((current_char_s - 206 )*20)-2;
      display.drawRoundRect(x, 40, 17, 10, 2, WHITE); 
    }
    else {current_char_s =194;}
      for (int i=194; i<212; i++){
        display.setCursor(col_s, line_s);
        if ((i == 199)or(i == 205)) {
          col_s = 0;
          line_s += 10;
        }
        else col_s += 20 ; 
        String text;
        if (i <= 205){
          text = "f" + String(i - 193); 
        }  
        else {
          if (i == 206) text = "tb";
          else if (i == 207) text = "crl";
          else if (i == 208) text = " wn";
          else if (i == 209) text = "alt";
          else if (i == 210) text = " shf";
          else if (i == 211) text = " -R";
        }
           display.println(text);
        }
        col_s = 0;
        line_s = 20;
        display.display();
        delay(100);
    }
  else if (page == 4){
    display.setTextColor(WHITE);
    display.setCursor(0, 0);
    display.setTextSize(1);
    display.print("Password size (num only)");
    display.setCursor(0, 58);
    display.print("press to select");
    display.setCursor(0, 30);
    display.setTextSize(3);
        
    if (page_bf == 1){
      display.setTextSize(1);
      display.print(pass_range);
      display.setTextSize(2);
      for (byte i=0; i<pass_range; i++){
          display.print("_ ");
      }
      
      if (xDir == "RIGHT") pass_range++;
      else if (yDir == "UP") pass_range++; 
      else if ((xDir == "LEFT") and pass_range>2) pass_range--;
      else if ((yDir == "DOWN") and pass_range>2) pass_range--;
      if (buttonState == 0) {
        page_bf = 2;
        String max_text;
        for (byte i=0; i<pass_range; i++){
            max_text += "9"; 
        }
        maximum = (max_text.toInt());
        return;
      }
      delay(25);
    }
    else {
      if (maximum > 0){
        display.print(maximum);
        Keyboard.print(maximum);
        Keyboard.press(KEY_RETURN);
        delay(100); 
        Keyboard.release(KEY_RETURN); 
        maximum--;
      }
      else {
        display.print("limit!");
      }
            if (buttonState == 0){
                page = 0;
            }
            
        }
        display.display();
    }
  else if (page == 5){
  if (page_ac == 1){
    display.setTextColor(WHITE);
    display.setCursor(0, 0);
    display.print("Click per mili second");
    display.setTextSize(1);
    display.setCursor(0, 58);
    display.print("press to select");
    display.setCursor(25, 30);
    display.setTextSize(3);
    display.print(delay_time);
    display.setTextSize(1);
    display.print("c/ms");

    if (xDir == "RIGHT") delay_time += 25;
    else if (yDir == "UP") delay_time += 25; 
    else if ((xDir == "LEFT") and delay_time>=25) delay_time -= 25; 
    else if ((yDir == "DOWN") and delay_time>=25) delay_time -= 25;
    if (buttonState == 0) {
      page_ac = 2;
      Mouse.move(1, 0);
    }
    delay(25);
  }
  else if (page_ac == 2){
    display.setTextColor(WHITE);
    display.setCursor(0, 0);
    display.setTextSize(1);
    display.print("- Auto Clicker");
    display.setCursor(0, 55);
    display.print("delay time: ");
    display.print(delay_time);
    display.setCursor(30, 20);
    display.setTextSize(4);
    if (counter != 0){      
      display.print(counter);
      counter--;
      delay(1000);
    }
    else {
      if (buttonState == 0){
        counter = 5 ;
        page_ac = 1 ;
        page = 0;
      }
      Mouse.click();
      clicks++ ;
      display.print(clicks);
      delay(delay_time);
            }
    }
  display.display();
  }
  else {
    page = 0 ;
  }
}
