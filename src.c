// Digital Clock with Enhanced Graphics
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "RTClib.h"

// Day names with proper formatting
const char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
const char monthsOfYear[12][12] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

// Display configuration
#define SCREEN_WIDTH 128  // OLED display width, in pixels
#define SCREEN_HEIGHT 64  // OLED display height, in pixels
#define OLED_RESET    -1  // Reset pin # (or -1 if sharing reset pin)

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
RTC_DS3231 rtc;

void setup() {
  Serial.begin(9600);
  
  // Initialize RTC
  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }
  
  // Initialize display
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  
  // Set RTC to compilation time if needed
  if (rtc.lostPower()) {
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
  
  // Display startup animation
  displayStartupAnimation();
}

void displayStartupAnimation() {
  // Sweeping animation
  for(int i = 0; i < SCREEN_WIDTH; i += 4) {
    display.clearDisplay();
    display.fillRect(0, 0, i, SCREEN_HEIGHT, SSD1306_WHITE);
    display.display();
    delay(5);
  }
  
  for(int i = 0; i < SCREEN_WIDTH; i += 4) {
    display.clearDisplay();
    display.fillRect(i, 0, SCREEN_WIDTH - i, SCREEN_HEIGHT, SSD1306_WHITE);
    display.display();
    delay(5);
  }
  
  display.clearDisplay();
  
  // Show title screen
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(2);
  
  // Center "SMART CLOCK"
  display.setCursor((SCREEN_WIDTH - 11*12)/2, 20);
  display.println("SMART CLOCK");
  
  // Draw decorative box
  display.drawRect(10, 15, SCREEN_WIDTH-20, 30, SSD1306_WHITE);
  display.drawRect(8, 13, SCREEN_WIDTH-16, 34, SSD1306_WHITE);
  
  display.display();
  delay(2000);
  display.clearDisplay();
}

void displayDigitalClock() {
  DateTime now = rtc.now();
  display.clearDisplay();
  
  // Display time in large font - centered
  display.setTextSize(2);
  
  // Format time with leading zeros and proper spacing
  char timeStr[9];
  sprintf(timeStr, "%02d:%02d:%02d", now.hour(), now.minute(), now.second());
  
  // Center the time string
  int16_t timeWidth = 12 * 6 * 2; // Approx width of time string
  display.setCursor((SCREEN_WIDTH - timeWidth) / 2, 5);
  display.println(timeStr);
  
  // Draw horizontal line separator
  display.drawLine(10, 25, SCREEN_WIDTH-10, 25, SSD1306_WHITE);
  
  // Format date with month name
  display.setTextSize(1);
  char dateStr[20];
  sprintf(dateStr, "%s %d, %d", monthsOfYear[now.month()-1], now.day(), now.year());
  
  // Center the date
  int16_t dateWidth = strlen(dateStr) * 6; // Approx width
  display.setCursor((SCREEN_WIDTH - dateWidth) / 2, 30);
  display.println(dateStr);
  
  // Display day of week
  int16_t dayWidth = strlen(daysOfTheWeek[now.dayOfTheWeek()]) * 6;
  display.setCursor((SCREEN_WIDTH - dayWidth) / 2, 42);
  display.println(daysOfTheWeek[now.dayOfTheWeek()]);
  
  // Draw decorative corner brackets
  int boxMargin = 5;
  int cornerSize = 8;
  
  // Top-left corner
  display.drawLine(boxMargin, boxMargin, boxMargin + cornerSize, boxMargin, SSD1306_WHITE);
  display.drawLine(boxMargin, boxMargin, boxMargin, boxMargin + cornerSize, SSD1306_WHITE);
  
  // Top-right corner
  display.drawLine(SCREEN_WIDTH - boxMargin - cornerSize, boxMargin, SCREEN_WIDTH - boxMargin, boxMargin, SSD1306_WHITE);
  display.drawLine(SCREEN_WIDTH - boxMargin, boxMargin, SCREEN_WIDTH - boxMargin, boxMargin + cornerSize, SSD1306_WHITE);
  
  // Bottom-left corner
  display.drawLine(boxMargin, SCREEN_HEIGHT - boxMargin, boxMargin + cornerSize, SCREEN_HEIGHT - boxMargin, SSD1306_WHITE);
  display.drawLine(boxMargin, SCREEN_HEIGHT - boxMargin, boxMargin, SCREEN_HEIGHT - boxMargin - cornerSize, SSD1306_WHITE);
  
  // Bottom-right corner
  display.drawLine(SCREEN_WIDTH - boxMargin - cornerSize, SCREEN_HEIGHT - boxMargin, SCREEN_WIDTH - boxMargin, SCREEN_HEIGHT - boxMargin, SSD1306_WHITE);
  display.drawLine(SCREEN_WIDTH - boxMargin, SCREEN_HEIGHT - boxMargin, SCREEN_WIDTH - boxMargin, SCREEN_HEIGHT - boxMargin - cornerSize, SSD1306_WHITE);
  
  // Draw small decorative icons
  display.fillCircle(18, 53, 3, SSD1306_WHITE);
  display.fillCircle(SCREEN_WIDTH - 18, 53, 3, SSD1306_WHITE);
  
  display.display();
}

void displayAnalogClock() {
  DateTime now = rtc.now();
  display.clearDisplay();
  
  int centerX = display.width() / 2;
  int centerY = display.height() / 2;
  int radius = 30;
  
  // Draw clock face
  display.drawCircle(centerX, centerY, radius, SSD1306_WHITE);
  display.drawCircle(centerX, centerY, radius + 1, SSD1306_WHITE);
  display.fillCircle(centerX, centerY, 2, SSD1306_WHITE);
  
  // Draw hour markers
  for (int i = 0; i < 12; i++) {
    float angle = i * 30;
    angle = (angle / 57.29577951); // Convert to radians
    
    // Longer marks for 12, 3, 6, 9
    int markerLength = (i % 3 == 0) ? 5 : 3;
    
    int x1 = (centerX + (sin(angle) * radius));
    int y1 = (centerY - (cos(angle) * radius));
    int x2 = (centerX + (sin(angle) * (radius - markerLength)));
    int y2 = (centerY - (cos(angle) * (radius - markerLength)));
    
    display.drawLine(x1, y1, x2, y2, SSD1306_WHITE);
  }
  
  // Draw hour hand
  float hourAngle = ((now.hour() % 12) * 30) + (now.minute() * 0.5);
  hourAngle = (hourAngle / 57.29577951);
  int hourX = (centerX + (sin(hourAngle) * (radius - 12)));
  int hourY = (centerY - (cos(hourAngle) * (radius - 12)));
  display.drawLine(centerX, centerY, hourX, hourY, SSD1306_WHITE);
  
  // Draw minute hand
  float minuteAngle = now.minute() * 6;
  minuteAngle = (minuteAngle / 57.29577951);
  int minuteX = (centerX + (sin(minuteAngle) * (radius - 4)));
  int minuteY = (centerY - (cos(minuteAngle) * (radius - 4)));
  display.drawLine(centerX, centerY, minuteX, minuteY, SSD1306_WHITE);
  
  // Draw second hand
  float secondAngle = now.second() * 6;
  secondAngle = (secondAngle / 57.29577951);
  int secondX = (centerX + (sin(secondAngle) * radius));
  int secondY = (centerY - (cos(secondAngle) * radius));
  display.drawLine(centerX, centerY, secondX, secondY, SSD1306_WHITE);
  
  // Add digital time at the bottom
  display.setTextSize(1);
  char timeStr[9];
  sprintf(timeStr, "%02d:%02d:%02d", now.hour(), now.minute(), now.second());
  
  int16_t timeWidth = strlen(timeStr) * 6; // Approx width
  display.setCursor((SCREEN_WIDTH - timeWidth) / 2, SCREEN_HEIGHT - 10);
  display.println(timeStr);
  
  // Display date in small text at top
  char dateStr[16];
  sprintf(dateStr, "%s %d, %d", monthsOfYear[now.month()-1], now.day(), now.year());
  
  int16_t dateWidth = strlen(dateStr) * 6; // Approx width
  display.setCursor((SCREEN_WIDTH - dateWidth) / 2, 0);
  display.println(dateStr);
  
  display.display();
}

// Global variable to track display mode
bool showAnalogClock = false;
unsigned long lastModeChange = 0;
const unsigned long modeChangeDuration = 10000; // Change mode every 10 seconds

void loop() {
  unsigned long currentTime = millis();
  
  // Alternate between digital and analog displays
  if (currentTime - lastModeChange > modeChangeDuration) {
    lastModeChange = currentTime;
    showAnalogClock = !showAnalogClock;
    
    // Add a transition effect
    for (int i = 0; i < SCREEN_HEIGHT; i += 4) {
      display.clearDisplay();
      display.fillRect(0, 0, SCREEN_WIDTH, i, SSD1306_INVERSE);
      display.display();
      delay(5);
    }
    display.clearDisplay();
  }
  
  if (showAnalogClock) {
    displayAnalogClock();
  } else {
    displayDigitalClock();
  }
  
  delay(100);
}
