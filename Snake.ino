#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Fonts/FreeSans9pt7b.h> 
#include <vector>

using namespace std;

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define BUTTON_PIN 15
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

vector<int> snakeX;
vector<int> snakeY;

int dx = 4, dy = 0;
/*
Difficulties:
  EZ: dx = 1
  Medium: dx = 4
  A bit hard: dx = 6
  Hard: dx = 8
  Very hard: dx = 12
  Sigma: dx = 20
*/
int foodX = 40, foodY = 30;
bool playing = false;

// Button variables
int buttonState = HIGH;
int lastReading = HIGH;
unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 50; // Default 

void setup() {
  Serial.begin(9600);
  pinMode(BUTTON_PIN, INPUT_PULLUP); // INPUT_PULLUP for no short-circuit

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();

  //! Snake body
  snakeX.push_back(10); // (10, 10)
  snakeY.push_back(10);
  snakeX.push_back(6); // (6, 10)
  snakeY.push_back(10);
  snakeX.push_back(2); // (2, 10)
  snakeY.push_back(10);
}

void loop() {
  int reading = digitalRead(BUTTON_PIN);

  // debounce
  if (reading != lastReading) {
    lastDebounceTime = millis();
  }
  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (reading != buttonState) {
      buttonState = reading;
      if (buttonState == LOW) {
        if (!playing) {
          playing = true; // Albert einstein right here
        } else {
          // rotate direction per press (had to use ai for ts part)
          int temp = dx;
          dx = -dy;
          dy = temp;
        }
      }
    }
  }
  lastReading = reading;

  display.clearDisplay();

  if (playing) {
    // Follow head
    for (int i = snakeX.size() - 1; i > 0; i--) {
      snakeX[i] = snakeX[i - 1];
      snakeY[i] = snakeY[i - 1];
    }

    // move head
    snakeX[0] += dx;
    snakeY[0] += dy;

    // wrap around screen
    if (snakeX[0] < 0) snakeX[0] = SCREEN_WIDTH - 4;
    if (snakeX[0] >= SCREEN_WIDTH) snakeX[0] = 0;
    if (snakeY[0] < 0) snakeY[0] = SCREEN_HEIGHT - 4;
    if (snakeY[0] >= SCREEN_HEIGHT) snakeY[0] = 0;

    // draw snake
    for (int i = 0; i < snakeX.size(); i++) {
      display.fillRect(snakeX[i], snakeY[i], 4, 4, SSD1306_WHITE);
    }

    // draw food
    display.fillRect(foodX, foodY, 4, 4, SSD1306_WHITE);

    // check if tourching food
    if (abs(snakeX[0] - foodX) < 4 && abs(snakeY[0] - foodY) < 6 ) { // < 6 can be changed, gehee
      foodX = random(0, SCREEN_WIDTH - 4);
      foodY = random(0, SCREEN_HEIGHT - 4);

      // grow snake from tail
      snakeX.push_back(snakeX.back());
      snakeY.push_back(snakeY.back());
    }
  } else {
    display.setFont(&FreeSans9pt7b);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 20);
    display.print("Press to Start !\n(Calculator)");
  }

  display.display();
  delay(100); // You can change if you want
}
