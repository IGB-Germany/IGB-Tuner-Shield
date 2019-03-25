/*TFT grafic functions*/
#include "printTft.h"


/*initalize*/
const unsigned char tftDcPin = 9;
const unsigned char tftCsPin = 10;
Adafruit_ILI9341 myScreen = Adafruit_ILI9341(tftCsPin, tftDcPin);
/*Cursor coordinates*/
struct coordinates_t cursorCoordinates1 = {/*positionX*/0, 0/*positionY*/};


const unsigned char numberBtns = 8;
Adafruit_GFX_Button buttons[numberBtns];
char buttonLabels[numberBtns][lengthLabel] = {"Suche", "Info", "<<", ">>", "1", "2", "3", "4"};
uint16_t buttonColors[numberBtns] = {ILI9341_RED, ILI9341_GREEN, ILI9341_NAVY, ILI9341_NAVY,
                                     ILI9341_BLUE, ILI9341_BLUE, ILI9341_BLUE, ILI9341_BLUE
                                    };
                                    
const unsigned char numberButtonsStyleCircle=1;
gfxButton buttonsStyleCircle[numberButtonsStyleCircle];


/*Rectangle grafic*/
struct rectangleGraphic_t rectangleGraphic1 = {/*Width*/310,/*Height*/40,/*Color*/ILI9341_WHITE};
struct rectangleGraphic_t rectangleGraphic2 = {/*Width*/310,/*Height*/40,/*Color*/ILI9341_WHITE};
/*Rectangle Coordinates*/
struct coordinates_t rectangleCoordinates1 = {/*positionX*/5,/*positionY*/130};
struct coordinates_t rectangleCoordinates2 = {/*positionX*/5,/*positionY*/190};

/*Text grafic*/
struct graphic_t textGraphic1 = {/*textSize*/3, /*textColor*/ILI9341_WHITE, /*textColorBackground*/ILI9341_BLACK};
struct graphic_t textGraphic2 = {/*textSize*/3, /*textColor*/ILI9341_RED,  /*textColorBackground*/ILI9341_BLACK};
/*Text coordinates*/
struct coordinates_t rectangleTextCoordinates1 = {/*positionX*/15,/*positionY*/140};
struct coordinates_t recctangleTextCoordinates2 = {/*positionX*/15,/*positionY*/200};

/*Time grafic*/
struct graphic_t timeGraphic1 = {3,/*size*/ILI9341_WHITE,/*color*/ILI9341_BLACK/*colorBackground*/};
/*Time Coordinates*/
struct coordinates_t timeCoordinates1 = {15,/*positionX*/ 200/*positionY*/};
/*Date grafic*/
struct graphic_t dateGraphic1 = {3,/*size*/ILI9341_WHITE,/*color*/ILI9341_BLACK/*colorBackground*/};
/*Date Coordinates*/
struct coordinates_t dateCoordinates1 = {15,/*positionX*/ 140/*positionY*/};


/*Textfield buffer*/
char rectangleText[rectangleTextLength + 1] = {' '};
/*Position of char in textfield*/
uint8_t rectangleTextPosition = 0;

/*Init display controller ILI9341*/
bool initDisplayIli9341()
{
  myScreen.begin();
  //Serial.println(myScreen.width());
  //Serial.println(myScreen.height());

  return true;
}

/*Clear Display*/
bool clearDisplayIli9341(struct coordinates_t cursorCoordinates, uint8_t orientation, int16_t color)
{
  /*
     Rotation: 0, 1, 2, 3
     0: 0°  no rotation
     1: 90° clockwise
     2:180° clockwise
     3:270° clockwise
  */
  myScreen.setRotation(orientation);
  myScreen.fillScreen(color);
  myScreen.setCursor(cursorCoordinates.positionX, cursorCoordinates.positionY);

  return true;
}

/*Init Buttons*/
bool initButtons(unsigned char rowsBtns, unsigned char colBtns)
{
  /*for (uint8_t row = 0; row < rowsBtns; row++)
  {
    for (uint8_t col = 0; col < colBtns; col++)
    {
      buttons[col + row * colBtns].initButtonUL(&myScreen, BUTTON_X + col * (BUTTON_W + BUTTON_SPACING_X),
          BUTTON_Y + row * (BUTTON_H + BUTTON_SPACING_Y), // x, y, w, h, outline, fill, text
          BUTTON_W, BUTTON_H, buttonFrameColor, buttonColors[col + row * colBtns], buttonFrameColor,
          buttonLabels[col + row * colBtns], BUTTON_TEXTSIZE);
    }
  }
*/
  buttonsStyleCircle[0].initStyleCircle(&myScreen, 0, 0, 20, ILI9341_WHITE, ILI9341_BLUE,
                                ILI9341_WHITE, "1", 2);
  
  return true;
}

/*Draw buttons*/
bool drawButtons(unsigned char rowsBtns, unsigned char colBtns)
{
  /*for (uint8_t row = 0; row < rowsBtns; row++)
  {
    for (uint8_t col = 0; col < colBtns; col++)
    {
      buttons[col + row * colBtns].drawButton();
    }
  }
*/
  buttonsStyleCircle[0].drawStyleCircle();
  
  return true;
}

/*Check if touched where button is*/
bool checkTouchButtons(int16_t x, int16_t y)
{
  // go thru all the buttons, checking if they were pressed
  for (uint8_t b = 0; b < numberBtns; b++)
  {
    if (buttons[b].contains(x, y))
    {
      buttons[b].press(true);  // tell the button it is pressed
    }

    else
    {
      buttons[b].press(false);  // tell the button it is NOT pressed
    }
  }
  return true;
}

/*If touched redraw*/
bool redrawTouchButtons()
{
  // now we can ask the buttons if their state has changed
  for (uint8_t b = 0; b < numberBtns; b++)
  {
    if (buttons[b].justReleased())
    {
      //Serial.print("Released: "); Serial.println(b);
      buttons[b].drawButton();  // draw normal
    }

    else if (buttons[b].justPressed())
    {
      //Serial.print("justPressed: "); Serial.println(b);
      buttons[b].drawButton(true);  // draw invert!

    }
    else if (buttons[b].isPressed())

    {
      //Serial.print("Is pressed: "); Serial.println(b);
    }

    else
    {

    }
  }

  return true;
}

/*Process Action Buttons*/
bool processActionButtons()
{

  if (buttons[0].justReleased())
  {
    /*Write a message*/
    writeTextField("30.01.1973");
  }

  if (buttons[1].justReleased())
  {
    /*Clear textfield*/
    clearTextField();
  }

  if (buttons[2].justReleased())
  {
    /*Delete last char in textfield*/
    deleteLastChar();
  }

  if (buttons[3].justReleased())
  {
    appendMessage("Hallo", 4 + 1);
  }

  if (buttons[4].justReleased())
  {
    /*Write a message*/
    writeTextField("FFH");
  }

  if (buttons[5].justReleased())
  {
    /*Write a message*/
    writeTextField("Radio Teddy");
  }

  if (buttons[6].justReleased())
  {
    /*Write a message*/
    writeTextField("SWR3");
  }

  if (buttons[7].justReleased())
  {
    /*Write a message*/
    writeTextField("298.000");
  }

  return true;
}

/*Draw rectangle*/
bool drawRectangle(struct coordinates_t rectangleCoordinates, struct rectangleGraphic_t rectangleGraphic)
{
  myScreen.drawRect(rectangleCoordinates.positionX,
                    rectangleCoordinates.positionY,
                    rectangleGraphic.rectangleWidth,
                    rectangleGraphic.rectangleHeight,
                    rectangleGraphic.rectangleColor);
  return true;
}

/*Write a message*/
bool writeTextField(char* msg)
{
  /*Clear textfield*/
  clearTextField();

  unsigned char len = 0;
  len = strlen(msg);
  strncpy(rectangleText, msg, len);

  /*set to new position*/
  rectangleTextPosition = len;
  /*Draw text*/
  drawText(rectangleText);

  return true;
}

/*Update text field*/
bool drawText(char* text, struct coordinates_t textCoordinates, struct graphic_t textGraphic)
{
  /*
     String:  886 Bytes RAM, 16476 Bytes Flash
     Char*:   868 Bytes RAM, 14986 Bytes Flash
  */
  myScreen.setCursor(textCoordinates.positionX, textCoordinates.positionY);
  myScreen.setTextSize(textGraphic.size);
  myScreen.setTextColor(textGraphic.color, textGraphic.colorBackground);
  myScreen.print(text);

  return true;
}

/*Clear textfield*/
bool clearTextField()
{
  /*write space*/
  for (unsigned char i = 0; i < rectangleTextLength; i++)
  {
    rectangleText[i] = ' ';
  }

  /*set to position 0*/
  rectangleTextPosition = 0;

  /*Draw text*/
  drawText(rectangleText);

  return true;
}

/*Append character*/
bool appendCharacter(char* msg)
{
  /*position == 0*/
  if (rectangleTextPosition == 0)
  {
    rectangleText[0] = msg[0];
    rectangleTextPosition++;
    Serial.print(F("Pos: "));
    Serial.println(rectangleTextPosition);

  }
  /*0 < position < 16*/
  else if ((rectangleTextPosition > 0) && (rectangleTextPosition < rectangleTextLength))
  {

    rectangleText[rectangleTextPosition] = msg[0];
    rectangleTextPosition++;

    Serial.print(F("Pos: "));
    Serial.println(rectangleTextPosition);
  }

  /*position => 16*/
  else
  {

  }

  /*Draw text*/
  drawText(rectangleText);

  return true;
}

/*Delete last char in textfield*/
bool deleteLastChar()
{
  /*0 < position < 16*/
  if (rectangleTextPosition > 0)
  {
    rectangleTextPosition--;
    rectangleText[rectangleTextPosition] = ' ';
  }
  /*position 0*/
  else rectangleText[rectangleTextPosition] = ' ';

  /*Draw text*/
  drawText(rectangleText);

  return true;
}

/*Append message*/
bool appendMessage(char* msg, uint8_t len)
{
  /* too long*/
  if (len > rectangleTextLength) len = rectangleTextLength;

  for (uint8_t i = 0; i < len; i++)
  {
    appendCharacter(&msg[i]);
  }

  return true;

}


/*backlight control 0..255*/
bool setBacklight(uint8_t duty, uint8_t backlightPin)
{
  /*duty values from 0 to 255*/
  pinMode(backlightPin, OUTPUT);
  analogWrite(backlightPin, duty);

  return true;
}

/*Print time*/
void printTime(coordinates_t timeCoordinates1, graphic_t timeGraphic1, uint8_t h, uint8_t m, uint8_t s)
{
  /*
    Serial.print(hour());
    Serial.print(":");
    Serial.print(minute());
    Serial.print(":");
    Serial.print(second());
    Serial.println();
  */

  myScreen.setTextSize(timeGraphic1.size);
  myScreen.setTextColor(timeGraphic1.color, timeGraphic1.colorBackground);
  myScreen.setCursor(timeCoordinates1.positionX, timeCoordinates1.positionY);

  /*Hour*/
  /*Add a zero, if necessary*/
  if (h < 10) {
    myScreen.print(0);
  }
  myScreen.print(h);
  myScreen.print(':');

  /*Minute*/
  /*Add a zero, if necessary*/
  if (m < 10) {
    myScreen.print(0);
  }
  myScreen.print(m);
  myScreen.print(':');

  /*Second*/
  /*Add a zero, if necessary*/
  if (s < 10) {
    myScreen.print(0);
  }
  myScreen.print(s);
}

/*Print Date*/
void printDate(coordinates_t dateCoordinates1, graphic_t dateGraphic1, uint8_t d, uint8_t m, uint8_t y)
{
  /*
    Serial.print(day());
    Serial.print(".");
    Serial.print(month());
    Serial.print(".");
    Serial.print(year());
    Serial.println();
  */


  myScreen.setTextSize(dateGraphic1.size);
  myScreen.setTextColor(dateGraphic1.color, dateGraphic1.colorBackground);
  myScreen.setCursor(dateCoordinates1.positionX, dateCoordinates1.positionY);

  /*Day*/
  /*Add a space, if necessary*/
  if (d < 10) {
    myScreen.print("0");
  }
  myScreen.print(d);
  myScreen.print(".");

  /*Month*/
  /*Add a space, if necessary*/
  if (m < 10) {
    myScreen.print("0");
  }
  myScreen.print(m);
  myScreen.print(".");

  /*Year*/
  /*4 digits*/
  myScreen.print(y);
}

/*Draw page*/
bool drawPage(struct coordinates_t buttonCoordinates,
              unsigned char rowsBtns,
              unsigned char colBtns,
              struct coordinates_t rectangleCoordinates,
              struct rectangleGraphic_t rectangleGraphic
              )
{
  /*Clear Display*/
  clearDisplayIli9341();
  
  /*Init Buttons*/
  initButtons(rowsBtns, colBtns);

  /*Draw buttons*/
  drawButtons(rowsBtns, colBtns);

  /*Draw rectangle*/
  drawRectangle(rectangleCoordinates, rectangleGraphic);

}

