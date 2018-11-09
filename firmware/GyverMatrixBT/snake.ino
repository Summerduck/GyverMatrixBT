// игра змейка!

#define START_LENGTH 4    // начальная длина змейки
#define MAX_LENGTH 30     // максимальная длина змейки
#define DEMO_SNAKE 0      // демо режим

int8_t vectorX, vectorY;
int8_t headX, headY, buttX, buttY;
int8_t appleX, appleY;
boolean apple_flag, missDelete = false;

int8_t buttVector[MAX_LENGTH];
int snakeLength;
boolean butt_flag, pizdetc;

void snakeRoutine() {
  buttonsTick();

  if (DEMO_SNAKE) snakeDemo();

  if (gameTimer.isReady()) {

    // БЛОК ГЕНЕРАЦИИ ЯБЛОКА
    while (!apple_flag) {                         // пока яблоко не создано
      appleX = random(0, WIDTH);                  // взять случайные координаты
      appleY = random(0, HEIGHT);

      // проверить, не совпадает ли координата с телом змеи
      if ((long)getPixColorXY(appleX, appleY) == 0) {
        apple_flag = true;                        // если не совпадает, считаем что яблоко создано
        drawPixelXY(appleX, appleY, GLOBAL_COLOR_2);        // и рисуем
        FastLED.show();
      }
    }

    // запоминаем, куда повернули голову
    // 0 - право, 1 - лево, 2 - вверх, 3 - вниз
    if (vectorX > 0) buttVector[snakeLength] = 0;
    else if (vectorX < 0) buttVector[snakeLength] = 1;
    if (vectorY > 0) buttVector[snakeLength] = 2;
    else if (vectorY < 0) buttVector[snakeLength] = 3;

    // смещение головы змеи
    headX += vectorX;
    headY += vectorY;

    // проверка на pizdetc
    if ((getPixColorXY(headX, headY) != 0 && getPixColorXY(headX, headY) != GLOBAL_COLOR_2)   // если змея врезалась во что то, но не в яблоко
        || headX < 0 || headX > WIDTH - 1       // если вышла за границы поля
        || headY < 0 || headY > HEIGHT - 1) {
      pizdetc = true;                           // флаг на отработку
    }

    // БЛОК ОТРАБОТКИ ПОЕДАНИЯ ЯБЛОКА
    if ((long)getPixColorXY(headX, headY) == (long)GLOBAL_COLOR_2) { // если попали головой в яблоко
      apple_flag = false;                       // флаг что яблока больше нет
      snakeLength++;                            // увеличить длину змеи
      buttVector[snakeLength] = 4;              // запоминаем, что надо будет не стирать хвост
    }

    // вычисляем координату хвоста (чтобы стереть) по массиву вектора
    switch (buttVector[0]) {
      case 0: buttX += 1;
        break;
      case 1: buttX -= 1;
        break;
      case 2: buttY += 1;
        break;
      case 3: buttY -= 1;
        break;
      case 4: missDelete = true;  // 4 значит не стирать!
        break;
    }

    // смещаем весь массив векторов хвоста ВЛЕВО
    for (byte i = 0; i < snakeLength; i++) {
      buttVector[i] = buttVector[i + 1];
    }

    // если змея не в процессе роста, закрасить бывший хвост чёрным
    if (!missDelete) {
      drawPixelXY(buttX, buttY, 0x000000);
      FastLED.show();
    }
    else missDelete = false;

    // рисуем голову змеи в новом положении
    drawPixelXY(headX, headY, GLOBAL_COLOR_1);
    FastLED.show();
  }

  // если он настал
  if (pizdetc) {
    pizdetc = false;

    // ну в общем плавно моргнуть, типо змейке "больно"
    for (byte bright = 0; bright < 15; bright++) {
      FastLED.setBrightness(bright);
      for (int i = 0; i < NUM_LEDS; i++) {
        leds[i] = CRGB::Red;
      }
      FastLED.show();
      delay(10);
    }
    delay(100);
    FastLED.clear();
    FastLED.show();
    FastLED.setBrightness(BRIGHTNESS);
    displayScore(snakeLength - START_LENGTH);
    delay(1000);
    FastLED.clear();
    FastLED.show();
    newGameSnake();                          // миша, всё ху.я, давай по новой
  }
}

void snakeDemo() {

}

void buttonsTick() {
  if (checkButtons()) {
    if (buttons[3]) {   // кнопка нажата
      vectorX = -1;
      vectorY = 0;
      buttons[3] = 0;
    }
    if (buttons[1]) {   // кнопка нажата
      vectorX = 1;
      vectorY = 0;
      buttons[1] = 0;
    }
    if (buttons[0]) {   // кнопка нажата
      vectorY = 1;
      vectorX = 0;
      buttons[0] = 0;
    }
    if (buttons[2]) {   // кнопка нажата
      vectorY = -1;
      vectorX = 0;
      buttons[2] = 0;
    }
  }
}

void newGameSnake() {
  FastLED.clear();
  // свежее зерно для генератора случайных чисел
  randomSeed(analogRead(2) * analogRead(3));

  // длина из настроек, начинаем в середине экрана, бла-бла-бла
  snakeLength = START_LENGTH;
  headX = WIDTH / 2;
  headY = HEIGHT / 2;
  buttY = headY;

  vectorX = 1;  // начальный вектор движения задаётся вот здесь
  vectorY = 0;

  // первоначальная отрисовка змейки и забивка массива векторов для хвоста
  for (byte i = 0; i < snakeLength; i++) {
    drawPixelXY(headX - i, headY, GLOBAL_COLOR_1);
    buttVector[i] = 0;
  }
  FastLED.show();
  buttX = headX - snakeLength;   // координата хвоста как голова - длина
  missDelete = false;
  apple_flag = false;
}
