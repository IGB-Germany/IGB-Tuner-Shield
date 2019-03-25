/*TFT grafic functions*/
#ifndef PRINT_TFT_H
#define PRINT_TFT_H
/*
   Dot arrangement:
   240 x 320
   xPosMin = 0;
   xPosMax = 240;

   yPosMin = 0;
   yPosMax = 320;

   Viewing Direction:
   6:00

*/

/*FT6206 driver functions*/
#include "touchTft.h"
/*ILI9341 driver functions*/
#include "Adafruit_ILI9341.h"

/*Graphics for Buttons*/
#include "gfx_button.h"

/*create screen*/
extern Adafruit_ILI9341 myScreen;

/*create buttons*/
extern const unsigned char numberBtns;
extern Adafruit_GFX_Button buttons[];

extern const unsigned char numberButtonsStyleCircle;
extern gfxButton buttonsStyleCircle[];

/*Length of character labels*/
const unsigned char lengthLabel = 6;
/*Labels*/
extern char buttonLabels[][lengthLabel];

/*Button grafics details */

/*Grafic details*/
extern struct graphicButton_t
{
  uint8_t width;
  uint8_t height;
  uint16_t colorFrame;
  int16_t colorBackground;
  int16_t colorText;
  uint8_t sizeText;

} buttonGraphic1;

const unsigned char BUTTON_W = 70;
const unsigned char BUTTON_H = 40;
const unsigned char BUTTON_SPACING_X = 10;
const unsigned char BUTTON_SPACING_Y = 20;
const unsigned char BUTTON_TEXTSIZE  = 2;
const uint16_t buttonFrameColor  = ILI9341_WHITE;

//buttonCoordinates1,
const unsigned char BUTTON_X = 5;
const unsigned char BUTTON_Y = 10;

/*Colors*/
extern uint16_t buttonColors[];

struct button_t_2
{
  struct coordinates_t_2;
  struct graphicButton_t_2;
  struct graphicText_t_2;
  char text_2;
};

/*Coordinates*/
struct coordinates_t_2
{
  uint8_t positionX;
  uint8_t positionY;
};

struct graphicButton_t_2
{
  uint8_t width;
  uint8_t height;
  uint16_t colorFrame;
  int16_t colorBackground;
};

struct graphicText_t_2
{
  int16_t colorText;
  uint8_t sizeText;
  struct coordinates_t_2;
};



/*Rectangle grafic details*/
extern struct rectangleGraphic_t
{
  uint16_t rectangleWidth;
  uint8_t rectangleHeight;
  int16_t rectangleColor;
} rectangleGraphic1, rectangleGraphic2;


/*Grafic details*/
extern struct graphic_t
{
  uint8_t size;
  int16_t color;
  int16_t colorBackground;

} timeGraphic1, dateGraphic1, textGraphic1, textGraphic2;

/*Coordinates*/
extern struct coordinates_t
{
  uint8_t positionX;
  uint8_t positionY;
}
buttonCoordinates1,
buttonTextCoordinates1,
buttonCoordinates2,
buttonTextCoordinates2,
cursorCoordinates1,

timeCoordinates1,
dateCoordinates1,

rectangleCoordinates1,
rectangleCoordinates2,
rectangleTextCoordinates1,
rectangleTextCoordinates2;


/*Rectangle text grafic details*/

const uint8_t rectanglePosX   = 5;
const uint8_t rectanglePosY   = 130;
const unsigned char rectangleTextPosX = rectanglePosX + 10;
const unsigned char rectangleTextPosY = rectanglePosY + 10;


/*Textfield buffer*/
const unsigned char rectangleTextLength = 16;
extern char rectangleText[rectangleTextLength + 1];
/*Position of char in textfield*/
extern uint8_t rectangleTextPosition;

/*Init display controller ILI9341*/
bool initDisplayIli9341();

/*Clear Display*/
bool clearDisplayIli9341(struct coordinates_t cursorCoordinates = cursorCoordinates1, uint8_t orientation = 3, int16_t color = ILI9341_BLACK);

/*Init Buttons*/
bool initButtons(unsigned char rowsBtns = 2, unsigned char colBtns = 4);

/*Draw buttons*/
bool drawButtons(unsigned char rowsBtns = 2, unsigned char colBtns = 4);

/*Compare touch with coordinates*/
bool checkTouchButtons(int16_t x, int16_t y);

/*If touched redraw*/
bool redrawTouchButtons();

/*Process Action Buttons*/
bool processActionButtons();

/*Draw rectangle*/
bool drawRectangle(struct coordinates_t rectangleCoordinates, struct rectangleGraphic_t);

/*Write a message*/
bool writeTextField(char* msg);

/*Draw text*/
bool drawText(char* text = "Template", struct coordinates_t textCoordinates = rectangleTextCoordinates1, struct graphic_t textGrafic = textGraphic1);

/*Clear textfield*/
bool clearTextField();

/*Append character*/
bool appendCharacter(char* msg);

/*Delete last char in textfield*/
bool deleteLastChar();

/*Append message*/
bool appendMessage(char* msg, uint8_t len);

/*backlight control 0..255*/
bool setBacklight(uint8_t duty = 10, uint8_t backlightPin = 5);

/*Print time*/
void printTime(coordinates_t timeCoordinates1, graphic_t timeGraphic1, uint8_t h, uint8_t m, uint8_t s);

/*Print Date*/
void printDate(coordinates_t dateCoordinates1, graphic_t dateGraphic1, uint8_t d, uint8_t m, uint8_t y);

/*Draw page*/
bool drawPage(struct coordinates_t buttonCoordinates,
              unsigned char rowsBtns,
              unsigned char colBtns,
              struct coordinates_t rectangleCoordinates,
              struct rectangleGraphic_t rectangleGraphic
             );
#endif /*PRINT_TFT_H*/



