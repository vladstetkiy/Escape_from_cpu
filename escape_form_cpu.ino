#include <Adafruit_ST7789.h>
#include <Adafruit_GFX.h>
#include <SPI.h>
#include <SD.h> 
#include <avr/pgmspace.h>


#define TFT_CS   10
#define TFT_RST  9
#define TFT_DC   8


Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);

#define SCREEN_WIDTH  320
#define SCREEN_HEIGHT 240

#define FOV  60.0 * (3.14 / 180.0)
#define ANGLE_STEP  FOV / SCREEN_WIDTH

#define JOY_MOVE_X A3  // Джойстик движения (ось X)
#define JOY_MOVE_Y A2  // Джойстик движения (ось Y)
#define JOY_MOVE_BUTTON 2
#define JOY_LOOK_X A1 // Джойстик камеры (ось X)
#define JOY_LOOK_Y A0  // Джойстик камеры (ось Y)
#define JOY_LOOK_BUTTON 3

#define SELECT_COLOR ST77XX_GREEN

#define MAP_SIZE 15

// Настройки прогресс-бара
#define BAR_X 60
#define BAR_Y 20
#define BAR_WIDTH 200
#define BAR_HEIGHT 20
#define BAR_RADIUS 10

//------------------------------------------------------------------------
//1.функции и переменные для реализации меню


bool processMenuInput();
void drawMenu();
void gameMenu();

int selectedOption = 0;  // 0 - Играть, 1 - Закончить игру

//------------------------------------------------------------------------
//2.функции и перменные для взаимодействия игрока и карты


float approx_dist(float dx, float dy);
void movePlayer(float dx, float dy);
void isKey();

float playerX = 1.5, playerY = 1.5;
float playerAngle = 240;
const float moveSpeed = 0.25;
const float rotSpeed = 0.15;

int floarRoofColor[3] = {0, 0, 0};
int wallColor[3] = {255, 0, 0};

uint8_t gameMap[MAP_SIZE][MAP_SIZE];

float initialState[3] = {1.5, 1.5, 240};

const uint8_t gameMapFlash[MAP_SIZE][MAP_SIZE] PROGMEM = {
  {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
  {1,0,1,7,1,9,0,0,0,0,8,0,0,0,1},
  {1,0,1,0,6,0,0,11,1,1,1,1,10,0,1},
  {1,0,3,0,1,1,1,1,1,0,0,0,0,0,1},
  {1,2,1,0,1,0,0,0,0,12,13,1,0,0,1},
  {1,0,1,4,1,0,1,1,1,1,1,1,1,0,1},
  {1,0,0,0,1,0,19,1,1,1,1,1,15,0,17},
  {1,1,1,5,1,0,1,21,1,1,1,1,1,0,1},
  {1,0,18,16,20,0,0,0,0,0,14,0,0,0,1},
  {1,0,1,1,1,1,1,1,1,1,1,1,1,1,1},
  {1,0,0,0,0,0,0,0,0,27,1,1,0,0,1},
  {1,0,1,23,1,25,1,0,1,1,1,1,0,0,1},
  {1,0,29,1,31,1,1,0,1,1,1,1,0,0,1},
  {1,0,0,0,0,0,0,0,22,24,26,28,30,0,1},
  {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
};

// тесты
const char test1[] PROGMEM = "1The clock speed of a CPU is measured in: 1:Gigahertz (GHz) 2:Cores 3:Cache size";
const char test2[] PROGMEM = "1The fastest memory in a CPU is: 1:Cache 2:RAM 3:Hard drive";
const char test3[] PROGMEM = "2The main function of a CPU is: 1:Store data 2:Execute instructions 3:Render graphics";
const char test4[] PROGMEM = "3Which company manufactures CPUs? 1:NVIDIA 2:Samsung 3:AMD"; 
const char test5[] PROGMEM = "2The purpose of multiple CPU cores is: 1:Reduce performance 2:Parallel processing 3:Increase clock speed";
const char test6[] PROGMEM = "3The CPU component that performs calculations is: 1:BIOS 2:GPU 3:ALU";
const char test7[] PROGMEM = "1CPU thermal design power (TDP) is measured in: 1:Watts 2:Millimeters 3:Volts";
const char test8[] PROGMEM = "2Intel's technology for thread doubling is called: 1:Hyper-V 2:Hyper-Threading 3:Turbo Boost";
const char test9[] PROGMEM = "3Modern CPUs typically support: 1:8-bit 2:16-bit 3:64-bit architecture";
const char test10[] PROGMEM = "1An example of CPU socket type is: 1:LGA 1700 2:DDR5 3:PCIe 4.0";
const char test11[] PROGMEM = "1What does CPU stand for? 1:Central Processing Unit 2:Computer Processing Unit 3:Core Processing Unit";
const char test12[] PROGMEM = "3Which component temporarily holds data for the CPU? 1:GPU 2:SSD 3:Cache";
const char test13[] PROGMEM = "2What is pipelining in CPUs? 1:Reducing clock speed 2:Parallel instruction execution 3:Increasing cache size";
const char test14[] PROGMEM = "1What is an integrated GPU? 1:GPU built into the CPU 2:External graphics card 3:VRAM module";
const char test15[] PROGMEM = "3Which process improves CPU performance by predicting branches? 1:Overclocking 2:Caching 3:Speculative execution";
const char test16[] PROGMEM = "2What does SIMD stand for? 1:SMultiple Data 2:Single Instruction 3:Sequential Memory Device";
const char test17[] PROGMEM = "1Which material is used for CPU thermal paste? 1:Silicon compound 2:Copper powder 3:Aluminum oxide";
const char test18[] PROGMEM = "3What is a CPU register? 1:External storage 2:Motherboard chip 3:Small fastest memory inside CPU";
const char test19[] PROGMEM = "2What controls the CPU clock speed? 1:Operating system 2:Base clock generator 3:Power supply unit";
const char test20[] PROGMEM = "1Which architecture uses RISC principles? 1:ARM 2:x86 3:PowerPC";
const char test21[] PROGMEM = "1Raycasting is mainly used for: 1:2.5D rendering 2:AI pathfinding 3:Sound processing";
const char test22[] PROGMEM = "3In raycasting, DDA stands for: 1:Data Display Algorithm 2:Digital Differential Analyzer 3:Depth Detection Array";
const char test23[] PROGMEM = "2Raycasting differs from raytracing by: 1:Using GPUs 2:Speed over accuracy 3:Color depth";
const char test24[] PROGMEM = "1Wolfenstein 3D used raycasting for: 1:Walls only 2:Enemies 3:Lighting";
const char test25[] PROGMEM = "3Raycasting typically works with: 1:Polygons 2:Voxels 3:2D grids";
const char test26[] PROGMEM = "2Primary raycast optimization is: 1:Texture filtering 2:Skip empty spaces 3:Anti-aliasing";
const char test27[] PROGMEM = "1Raycast hits are detected by: 1:Grid traversal 2:Physics engine 3:Collision boxes";
const char test28[] PROGMEM = "3Fishbowl effect in raycasting is due to: 1:Textures 2:Enemies 3:No perspective correction";
const char test29[] PROGMEM = "2Raycasting was popular in: 1:1990s PCs 2:1980s-90s 3:2000s consoles";
const char test30[] PROGMEM = "1For floor/ceiling in raycasting: 1:Project rays 2:Use sprites 3:Skip rendering";
const char test31[] PROGMEM = "3Raycasting limitation: 1:Speed 2:Colors 3:No curved walls";
const char test32[] PROGMEM = "2Raycasting first used in: 1:Games 2:Radar systems 3:Medical imaging";
const char test33[] PROGMEM = "1Key raycast math uses: 1:Trigonometry 2:Calculus 3:Algebra";
const char test34[] PROGMEM = "3Raycasting textures are: 1:3D models 2:SVG files 3:Pre-stretched";
const char test35[] PROGMEM = "2Raycasting Z-buffer: 1:Stores colors 2:Depth sorting 3:Light values";
const char test36[] PROGMEM = "1Raycast view is: 1:Fixed height 2:Adjustable 3:Isometric";
const char test37[] PROGMEM = "3Raycasting pseudo-3D uses: 1:Shaders 2:Multiple CPUs 3:Vertical slices";
const char test38[] PROGMEM = "2Raycasting speed depends on: 1:GPU 2:Resolution 3:Texture size";
const char test39[] PROGMEM = "1Early raycast games avoided: 1:Overlapping rooms 2:Enemies 3:Weapons";
const char test40[] PROGMEM = "3Modern raycasting uses: 1:Software only 2:OpenGL 3:Both approaches";
const char test41[] PROGMEM = "2C++ was created in: 1:1985 2:1983 3:1990";
const char test42[] PROGMEM = "3Which is a C++ container? 1:array 2:vector 3:Both";
const char test43[] PROGMEM = "1RAII in C++ stands for: 1:Resource Acquisition Is Initialization 2:Runtime Allocation 3:Random";
const char test44[] PROGMEM = "2C++ smart pointer: 1:malloc 2:unique_ptr 3:new";
const char test45[] PROGMEM = "3C++ lambda starts with: 1:func 2:def 3:[]";
const char test46[] PROGMEM = "1Which is C++ keyword? 1:class 2:tuple 3:lambda";
const char test47[] PROGMEM = "2C++ template uses: 1:<> 2:() 3:{}";
const char test48[] PROGMEM = "3C++ exception uses: 1:try/catch 2:error() 3:Both";
const char test49[] PROGMEM = "1C++ virtual functions: 1:Enable polymorphism 2:Increase speed 3:Avoid inheritance";
const char test50[] PROGMEM = "2C++20 feature: 1:auto 2:concepts 3:nullptr";
const char test51[] PROGMEM = "3C++ reference uses: 1:* 2:& 3:->";
const char test52[] PROGMEM = "1constexpr means: 1:Compile-time eval 2:Runtime const 3:External linkage";
const char test53[] PROGMEM = "2C++ STL stands for: 1:Standard Template Library 2:Standard Type List 3:Static Template";
const char test54[] PROGMEM = "3C++ move semantics use: 1:&& 2:|| 3:<<";
const char test55[] PROGMEM = "1C++ namespace purpose: 1:Avoid collisions 2:Memory mgmt 3:Type safety";
const char test56[] PROGMEM = "2C++ constructor has: 1:Return type 2:Same name as class 3:static keyword";
const char test57[] PROGMEM = "3C++ friend function: 1:Is virtual 2:Inheritable 3:Access private";
const char test58[] PROGMEM = "1C++ auto keyword: 1:Type deduction 2:Automatic storage 3:Thread-local";
const char test59[] PROGMEM = "2C++ pure virtual: 1:=0 2:=pure 3:=abstract";
const char test60[] PROGMEM = "3C++ compilation step: 1:Preprocess 2:Compile 3:All above";


const char* const tests[] PROGMEM = {
  test1, test2, test3, test4, test5, test6, test7, test8, test9, test10,
  test11, test12, test13, test14, test15, test16, test17, test18, test19, test20,
  test21, test22, test23, test24, test25, test26, test27, test28, test29, test30,
  test31, test32, test33, test34, test35, test36, test37, test38, test39, test40,
  test41, test42, test43, test44, test45, test46, test47, test48, test49, test50,
  test51, test52, test53, test54, test55, test56, test57, test58, test59, test60
};
char testBuffer[130];

const char tutorial1[] PROGMEM = "Welcome to the tutorial for new operators";
const char tutorial2[] PROGMEM = "Here is explanation of what you will see during work";
const char tutorial3[] PROGMEM = "to open the menu press the right stick";
const char tutorial4[] PROGMEM = "Use the right stick to move";
const char tutorial5[] PROGMEM = "To turn the camera right and left, use the left stick";
const char tutorial6[] PROGMEM = "while work you can see a flickering area";
const char tutorial7[] PROGMEM = "This is a broken section, you need to get close to it";
const char tutorial8[] PROGMEM = "when you get close, press the left stick";
const char tutorial9[] PROGMEM = "If you answer the question correctly, the area will be repaired";
const char tutorial10[] PROGMEM = "after repairing, somewhere a wall will disappear, and you will be able to go further";
const char tutorial11[] PROGMEM = "tutorial is finished, good job";

const char* const tutorialMessages[] PROGMEM = {
  tutorial1, tutorial2, tutorial3, tutorial4, tutorial5, tutorial6,
  tutorial7, tutorial8, tutorial9, tutorial10, tutorial11
};

int count_Doors = 0;
int rapaidDoors = 0;
void countDoors() {
  for (uint8_t y = 0; y < MAP_SIZE; y++) {
    for (uint8_t x = 0; x < MAP_SIZE; x++) {
      if (gameMap[y][x] % 2 == 1 && gameMap[y][x] != 1) count_Doors++;
    }
  }
}
void drawProgressBar(float progress) {
  progress = constrain(progress, 0.0, 1.0);
  
  // Обводка
  tft.drawRoundRect(BAR_X, BAR_Y, BAR_WIDTH, BAR_HEIGHT, BAR_RADIUS, ST77XX_WHITE);

  // Фон прогресс-бара
  tft.fillRoundRect(BAR_X + 1, BAR_Y + 1, BAR_WIDTH - 2, BAR_HEIGHT - 2, BAR_RADIUS - 1, ST77XX_BLACK);
  
  // Заполненная часть
  int filled = (int)((BAR_WIDTH - 2) * progress);
  if (filled > 0) {
    tft.fillRoundRect(BAR_X + 1, BAR_Y + 1, filled, BAR_HEIGHT - 2, BAR_RADIUS - 1, ST77XX_GREEN);
  }
  // Пауза 1 секунда
  delay(1000);

  // Очистка всей области
  tft.fillRoundRect(BAR_X, BAR_Y, BAR_WIDTH, BAR_HEIGHT, BAR_RADIUS, ST77XX_BLACK);
}

//------------------------------------------------------------------------
//3.функции для вывода на дисплей (изображения, сообщеня, миниигры)

int gameOption = 0;
int rightAnswer = 0;

void WindowMessage(String text, int del);
void drawScene();
void clearScene();
int miniGames(int rayType);
int processTestInput();
void drawOptions();
int WindowTest(String text, int rAnswer);
void printTutorialMessage(uint8_t index) {
  char buffer[80]; // размер должен хватать под самую длинную строку
  strcpy_P(buffer, (PGM_P)pgm_read_ptr(&(tutorialMessages[index])));
  WindowMessage(buffer, 4000);
}

//------------------------------------------------------------------------
//4.управляющая функция

void processGame();
void processEducations();

//------------------------------------------------------------------------
//5. функции для работы с флеш память

void loadMapFromFlash();
void applyPlayerInitialState();


//========================================================================
//========================================================================
//реализация


//------------------------------------------------------------------------
//1.функции и переменные для реализации меню

void drawMenu() {
  tft.setTextSize(2);
  
  if (selectedOption == 0) {
    tft.setTextColor(SELECT_COLOR);
  } else {
    tft.setTextColor(ST77XX_WHITE);
  }
  tft.setCursor(60, 40);
  tft.println("PLAY");

  if (selectedOption == 1) {
    tft.setTextColor(SELECT_COLOR);
  } else {
    tft.setTextColor(ST77XX_WHITE);
  }
  tft.setCursor(60, 80);
  tft.println("RESTART");

  if (selectedOption == 2) {
    tft.setTextColor(SELECT_COLOR);
  } else {
    tft.setTextColor(ST77XX_WHITE);
  }
  tft.setCursor(60, 120);
  tft.println("AUTHOR");

  if (selectedOption == 3) {
    tft.setTextColor(SELECT_COLOR);
  } else {
    tft.setTextColor(ST77XX_WHITE);
  }
  tft.setCursor(60, 160);
  tft.println("TUTORIAL");
}

bool processMenuInput() {
  float moveY = (analogRead(JOY_MOVE_Y) - 512) / 512.0;

  if (moveY > 0.7) {
    if (selectedOption >= 1)selectedOption -= 1;
    delay(200);  // Защита от дребезга
  } else if (moveY < -0.7) {
    if (selectedOption < 3) selectedOption += 1;
    delay(200);
  }

  if (digitalRead(JOY_MOVE_BUTTON) == LOW) {
    if (selectedOption == 0) {
      // Запустить игру
      return false;
    } 
    else if ( selectedOption == 1) {
      clearScene(); 
      rapaidDoors = 0;
      applyPlayerInitialState();
      loadMapFromFlash();
      return true;
    }
    else if ( selectedOption == 2) {
      clearScene(); 
      tft.setCursor(60, 20);
      tft.println("Stetskiy Vlad");
      tft.setCursor(60, 40);
      tft.println("from PZT-42");
      delay(3000);
      clearScene(); 
      return true;
    }
    else if ( selectedOption == 3) {
      clearScene(); 
      for (int i = 0; i < (sizeof(tutorialMessages) / sizeof(tutorialMessages[0]));i++) {
        printTutorialMessage(i);
      }
      clearScene(); 
      return true;
    }
  }
}
void gameMenu() {
  delay(300);
  drawMenu();
  while (processMenuInput()) {
    drawMenu();
  }
  clearScene(); 
}


//------------------------------------------------------------------------
//2.функции и перменные для взаимодействия игрока и карты
void checkScripts() {

};

float approx_dist(float dx, float dy) {
  return max(abs(dx), abs(dy)); // Быстрая оценка вместо sqrt(dx² + dy²)
}
void movePlayer(float dx, float dy) {
  if (!gameMap[int(playerY + dy)][int(playerX + dx)]) {
    checkScripts();
    playerX += dx;
    playerY += dy;
  }
}
void isKey() {

	int objX = (int)(playerX + cos(playerAngle));
	int objY = (int)(playerY + sin(playerAngle));
  
	if (gameMap[objY][objX] % 2 == 1 && gameMap[objY][objX] != 1 && miniGames(gameMap[objY][objX]) > 0) {
    for (int i = 0; i < MAP_SIZE; i++) {
      for (int k = 0; k < MAP_SIZE; k++) {
        if (gameMap[objY][objX] - 1 == gameMap[i][k]) gameMap[i][k] = 0;
      }
    }
    gameMap[objY][objX] = 1;
    rapaidDoors++;
    drawScene();
    drawProgressBar(rapaidDoors / (float)count_Doors);
    if (rapaidDoors >= count_Doors) {

      clearScene(); 
      WindowMessage("Your WON", 4000);

      rapaidDoors = 0;
      applyPlayerInitialState();
      loadMapFromFlash();
      gameMenu();
    }
  }
}
void applyPlayerInitialState() {
  playerX = initialState[0];
  playerY = initialState[1];
  playerAngle = initialState[2];
}


//------------------------------------------------------------------------
//3.функции для вывода на дисплей (изображения, сообщеня, миниигры)

void WindowMessage(String text, int del) {
  int winW = SCREEN_WIDTH * 3.5 / 4;  // 3/4 ширины экрана
  int winH = SCREEN_HEIGHT * 3.5 / 4;  // 3/4 высоты экрана
  int xw = (SCREEN_WIDTH - winW) / 2; // Центрирование
  int yw = (SCREEN_HEIGHT - winH) / 2;
  int radius = 12;  // Радиус скругления углов

  // Рисуем белую границу
  tft.fillRoundRect(xw, yw, winW, winH, radius, ST77XX_WHITE);
  
  // Рисуем черную заливку внутри
  tft.fillRoundRect(xw+2, yw+2, winW-4, winH-4, radius, ST77XX_BLACK);
  tft.setTextSize(2);

  int x = 30;  // Смещение текста от края окна
  int y = 22;
  int maxWidth = 255; // Максимальная ширина строки (чтобы текст переносился)

  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(2);
  
  int textLength = text.length();
  int lineStart = 0;
  int charWidth = 12;  // Примерная ширина символа (зависит от шрифта)
  int charsPerLine = maxWidth / charWidth; 

  while (lineStart < textLength) {
    String line = text.substring(lineStart, lineStart + charsPerLine);
    tft.setCursor(x, y);
    tft.println(line);
    y += 20;  // Перемещение вниз для новой строки
    lineStart += charsPerLine;
  }
  delay(del);
  tft.fillRoundRect(xw, yw, winW, winH, radius, ST77XX_BLACK);
}
int processTestInput() {
  float moveY = (analogRead(JOY_MOVE_X) - 512) / -512.0;

  if (moveY > 0.5) {
    if (gameOption >= 1) gameOption -= 1;
    delay(400);  // Защита от дребезга
  } 
  else if (moveY < -0.5) {
    if (gameOption < 2) gameOption += 1;
    delay(400);
  }

  if (digitalRead(JOY_MOVE_BUTTON) == LOW) {
    if (gameOption == rightAnswer) {
      return 1;
    }
    else {
      return 2;
    }
  }
  return 0;
}

void drawOptions() {
  tft.setTextSize(2);
  if (gameOption == 0) {
      tft.setTextColor(SELECT_COLOR);
  } 
  else {
    tft.setTextColor(ST77XX_WHITE);
  }
  tft.setCursor(60, 190);
  tft.println("1");

  if (gameOption == 1) {
    tft.setTextColor(SELECT_COLOR);
  } 
  else {
    tft.setTextColor(ST77XX_WHITE);
  }
  tft.setCursor(150, 190);
  tft.println("2");

  if (gameOption == 2) {
    tft.setTextColor(SELECT_COLOR);
  } else {
    tft.setTextColor(ST77XX_WHITE);
  }
  tft.setCursor(240, 190);
  tft.println("3");
}

int WindowTest(String text, int rAnswer) {
  int winW = SCREEN_WIDTH * 3.5 / 4;  // 3/4 ширины экрана
  int winH = SCREEN_HEIGHT * 3.5 / 4;  // 3/4 высоты экрана
  int xw = (SCREEN_WIDTH - winW) / 2; // Центрирование
  int yw = (SCREEN_HEIGHT - winH) / 2;
  int radius = 12;  // Радиус скругления углов

  // Рисуем белую границу
  tft.fillRoundRect(xw, yw, winW, winH, radius, ST77XX_WHITE);
  
  // Рисуем черную заливку внутри
  tft.fillRoundRect(xw+2, yw+2, winW-4, winH-4, radius, ST77XX_BLACK);
  tft.setTextSize(1);

  int x = 30;  // Смещение текста от края окна
  int y = 22;
  int maxWidth = 255; // Максимальная ширина строки (чтобы текст переносился)

  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(2);
  
  int textLength = text.length();
  int lineStart = 0;
  int charWidth = 12;  // Примерная ширина символа (зависит от шрифта)
  int charsPerLine = maxWidth / charWidth; 

  rightAnswer = rAnswer - 1;
  while (lineStart < textLength) {
    String line = text.substring(lineStart, lineStart + charsPerLine);
    tft.setCursor(x, y);
    tft.println(line);
    y += 20;  // Перемещение вниз для новой строки
    lineStart += charsPerLine;
  }
  drawOptions();
  int result = 0;
  while(result == 0) {
    result = processTestInput();
    drawOptions();
  }
  if (result == 1) {
    tft.fillRoundRect(xw, yw, winW, winH, radius, ST77XX_BLACK);
    return 1;
  }
  else {
    tft.fillRoundRect(xw, yw, winW, winH, radius, ST77XX_BLACK);
    return 0;
  }
}

void drawScene() {
  float c = 0.005;
  int i = 0;
  for (int x = 0; x < SCREEN_WIDTH; x+=6) {
    float rayAngle = playerAngle - (FOV / 2) + x * ANGLE_STEP;
    float rayX = playerX, rayY = playerY;
    float stepX = cos(rayAngle) * c;
    float stepY = sin(rayAngle) * c;
    int rayType = 0;

    while (gameMap[(int)rayY][(int)rayX] == 0) {
      rayX += stepX;
      rayY += stepY;
      if (c < 0.1 && i > 500) {c+= 0.002;}
      if (i <= 500){ i++;}
    }
    
    rayType = gameMap[(int)rayY][(int)rayX];
    float distance = approx_dist(rayX - playerX, rayY - playerY);


    int lineHeight = SCREEN_HEIGHT / (distance + 0.1);
    int drawStart = (SCREEN_HEIGHT - lineHeight) / 2;
    // Цвет стены в зависимости от типа
  
    if (distance < 1) distance = 1;
    // Рисуем вертикальную линию
    if ((rayType % 2 == 0 && rayType != 0) || rayType == 1){ 
      tft.drawFastVLine(x, 0, 240, tft.color565(floarRoofColor[0], floarRoofColor[1], floarRoofColor[2]));
      tft.drawFastVLine(x, drawStart, lineHeight, tft.color565(wallColor[0] / distance, wallColor[1], wallColor[2]) );
    }
    else  if (rayType % 2 == 1){ 
      tft.drawFastVLine(x, 0, 240, tft.color565(floarRoofColor[0], floarRoofColor[1], floarRoofColor[2]));
      tft.drawFastVLine(x, drawStart, lineHeight, tft.color565(random(0, 256) ,random(0, 256), random(0, 256)));
    }
    else  if (rayType == 100){ 
      tft.drawFastVLine(x, 0, 240, tft.color565(floarRoofColor[0], floarRoofColor[1], floarRoofColor[2]));
      tft.drawFastVLine(x, drawStart, lineHeight, tft.color565(wallColor[1] , wallColor[0] / distance, wallColor[2]) );
    }
  }
}
int miniGames(int rayType) {

  int index = random(1, 59);;
  strcpy_P(testBuffer, (char*)pgm_read_word(&(tests[index])));
  Serial.println(testBuffer[0]);
  int rightAnswer = testBuffer[0] - '0';
  testBuffer[0] = ' ';
  return WindowTest(testBuffer, rightAnswer);
}
void clearScene() {
  for (int x = 0; x < SCREEN_WIDTH; x++) {
     tft.drawFastVLine(x, 0, 240, ST77XX_BLACK);
  }
}

//------------------------------------------------------------------------
//4.управляющая функция

void processGame() {
  float moveX = (analogRead(JOY_MOVE_X) - 512) / 512.0;
  float moveY = (analogRead(JOY_MOVE_Y) - 512) / 512.0;
  float lookX = (analogRead(JOY_LOOK_X) - 512) / 512.0;

  if (abs(moveX) < 0.5) moveX = 0;
  if (abs(moveY) < 0.5) moveY = 0;
  if (abs(lookX) < 0.5) lookX = 0;

  // Управление движением
  float moveDirX = cos(playerAngle) * moveY - sin(playerAngle) * moveX;
  float moveDirY = sin(playerAngle) * moveY + cos(playerAngle) * moveX;
  
  if (digitalRead(JOY_LOOK_BUTTON) == LOW) isKey();
  if (digitalRead(JOY_MOVE_BUTTON) == LOW) gameMenu();
  movePlayer(moveDirX * moveSpeed / 2, moveDirY * moveSpeed);
  
  // Управление поворотом
  playerAngle += lookX * rotSpeed;
  
  drawScene();
}


//------------------------------------------------------------------------
//5. функции для работы с флеш память

void loadMapFromFlash() {
  for (uint8_t y = 0; y < MAP_SIZE; y++) {
    for (uint8_t x = 0; x < MAP_SIZE; x++) {
      gameMap[y][x] = pgm_read_byte(&(gameMapFlash[y][x]));
    }
  }
}

//========================================================================
//========================================================================
//запуск

void setup() {
  Serial.begin(300);
  pinMode(JOY_LOOK_BUTTON, INPUT_PULLUP);
  pinMode(JOY_MOVE_BUTTON, INPUT_PULLUP);
  tft.init(240, 320);
  tft.setRotation(1);
  tft.invertDisplay(false); 
  applyPlayerInitialState();
  loadMapFromFlash();
  clearScene();
  countDoors();
  randomSeed(analogRead(A5));
  gameMenu();

}
void loop() {
  processGame();
}
