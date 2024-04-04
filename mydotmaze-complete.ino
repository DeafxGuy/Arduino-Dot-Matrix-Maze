#include <LedControl.h>

//PIN LAYOUTS!
// Dot matrix:
//    VCC - 5v
//    GND - Ground
//    DIN - PWM 12
//    CS - PWM 11
//    CLK - PWM 10

// Joystick:
//    GND - Ground
//    +5V - 5v
//    VRx - A0
//    VRy - A1
//    SW - PWM 2 - NOT required but added anyway

// Pin connections to the LED matrix
int DIN = 12;
int CS = 11;
int CLK = 10;

// Create a new LedControl object
LedControl lc = LedControl(DIN, CLK, CS, 0);

// Define constants for the start and end positions
const int SRT = 2;
const int END = 3;

// The maze layouts
struct Level {
  int layout[8][8];
  int nextLevels[5]; //Next levels feature was planned to have dynamic levels instead of following one set path, you can add this if youd like by >See line 51
};

Level levels[5] = {
  // Level 0
  {
    // Layout
    {
      {1, 1, 1, 1, 1, 1, END, 1},
      {1, 1, 1, 1, 1, 1, 0, 1},
      {1, 1, 1, 1, 1, 1, 0, 1},
      {1, 0, 0, 0, 1, 0, 0, 1},
      {1, 0, 1, 0, 0, 0, 1, 1},
      {1, 0, 1, 1, 1, 1, 1, 1},
      {1, 0, 0, 1, 1, 1, 1, 1},
      {1, 1, SRT, 1, 1, 1, 1, 1}
    },
    // Next levels
    {1} // for dynamic levels, in these brackets select which levels you would like to come after the current level eg {0, 3, 8}.
  },
  // Level 1
  {
    // Layout
    {
      {1, 1, 1, 1, 1, 1, 1, 1},
      {1, 1, 1, 1, 0, 0, 0, 1},
      {1, 1, 1, 1, 0, 1, 0, 1},
      {1, 1, 1, 1, 0, 1, 0, 1},
      {1, 1, 1, 1, 0, 1, 0, 1},
      {1, 1, 1, 1, 0, 1, 0, 1},
      {END, 0, 0, 0, 0, 1, 0, 1},
      {1, 1, 1, 1, 1, 1, SRT, 1}
    },
    // Next levels
    {2}
  },
    // Level 2
  {
    // Layout
    {
      {1, 1, 1, END, 1, 1, 1, 1},
      {1, 1, 1, 0, 1, 1, 1, 1},
      {1, 0, 0, 0, 1, 1, 1, 1},
      {1, 0, 1, 1, 1, 1, 1, 1},
      {1, 0, 1, 0, 0, 0, 0, 1},
      {1, 0, 1, 0, 1, 1, 0, 1},
      {1, 0, 0, 0, 1, 1, 0, SRT},
      {1, 1, 1, 1, 1, 1, 1, 1}
    },
    // Next levels
    {3}
  },
    // Level 3
  {
    // Layout
    {
      {1, 1, 1, 1, 1, 1, 1, 1},
      {1, 1, 1, 1, 1, 1, 1, 1},
      {1, 0, 0, 0, 0, 0, 1, 1},
      {END, 0, 1, 1, 1, 0, 1, 1},
      {1, 1, 1, 1, 1, 0, 1, 1},
      {1, 1, 1, 0, 0, 0, 1, 1},
      {1, 1, 1, 0, 1, 1, 1, 1},
      {1, 1, 1, SRT, 1, 1, 1, 1}
    },
    // Next levels
    {4}
  },
    // Level 4
  {
    // Layout
    {
      {1, 1, END, 1, 1, 1, 1, 1},
      {1, 1, 0, 1, 0, 0, 0, 1},
      {1, 1, 0, 1, 0, 1, 0, 1},
      {1, 1, 0, 0, 0, 1, 0, SRT},
      {1, 1, 1, 1, 1, 1, 1, 1},
      {1, 1, 1, 1, 1, 1, 1, 1},
      {1, 1, 1, 1, 1, 1, 1, 1},
      {1, 1, 1, 1, 1, 1, 1, 1}
    },
    // Next levels
    {0}
  }
};

// The current level and player's position
int currentLevel = 0;
int playerX = 0;
int playerY = 7;

// Time limit for each level
unsigned long levelStartTime;
const unsigned long levelTimeLimit = 5000; // 5 seconds

// Game over screen
int gameOverScreen[8][8] = {
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 1, 1, 0, 0, 1, 1, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 1, 1, 1, 1, 0, 0},
  {0, 1, 0, 0, 0, 0, 1, 0},
  {0, 0, 0, 0, 0, 0, 0, 0}
};

// Number of levels completed
int levelsCompleted = 0;

void setup() {
  // Initialize the LED matrix
  lc.shutdown(0, false);
  lc.setIntensity(0, 8);
  lc.clearDisplay(0);

  // Find the player's start position
  findStartPosition();

  // Draw the initial maze
  drawMaze();

  // Record the start time of the level
  levelStartTime = millis();
}

void loop() {
  // Check if the time limit for the level has been exceeded
  if (millis() - levelStartTime > levelTimeLimit) {
    // Set the display brightness back to 8
    lc.setIntensity(0, 8);

    // Show the game over screen
    for (int y = 0; y < 8; y++) {
      for (int x = 0; x < 8; x++) {
        lc.setLed(0, y, x, gameOverScreen[y][x]);
      }
    }

    // Wait a bit before the next loop iteration
    delay(1000);

    // Clear the game over screen
    lc.clearDisplay(0);

    // Show the number of levels completed
    for (int i = 0; i < levelsCompleted; i++) {
      lc.setLed(0, i / 8, i % 8, true);
    }

    // Wait a bit before the next loop iteration
    delay(2000);

    // Reset the game
    currentLevel = 0;
    findStartPosition();
    drawMaze();
    levelStartTime = millis();
    lc.setIntensity(0, 8);

    // Reset the number of levels completed
    levelsCompleted = 0;

    return;
  }

  // Decrease the display brightness linearly over time
  int timeLeft = levelTimeLimit - (millis() - levelStartTime);
  int brightness = map(timeLeft, 0, levelTimeLimit, 2, 8);
  lc.setIntensity(0, brightness);

  // Read the joystick position
  int joyX = analogRead(A0);
  int joyY = analogRead(A1);

  // Determine the new player position
  int newX = playerX;
  int newY = playerY;
  if (joyX < 300) {
    newX--;
  } else if (joyX > 700) {
    newX++;
  }

  // Check if the new X position is inside the maze and not a wall
  if (newX >= 0 && newX < 8 && (levels[currentLevel].layout[playerY][newX] == 0 || levels[currentLevel].layout[playerY][newX] == END)) {
    // Update the player X position
    playerX = newX;
  }

  if (joyY < 300) {
    newY--;
  } else if (joyY > 700) {
    newY++;
  }

  // Check if the new Y position is inside the maze and not a wall
  if (newY >= 0 && newY < 8 && (levels[currentLevel].layout[newY][playerX] == 0 || levels[currentLevel].layout[newY][playerX] == END)) {
    // Update the player Y position
    playerY = newY;
  }

  // Check if the player has reached the end position
  if (levels[currentLevel].layout[playerY][playerX] == END) {
    // Flash the LED matrix twice
    for (int i = 0; i < 2; i++) {
      lc.clearDisplay(0);
      lc.setIntensity(0, 8);
      delay(50);
      drawMaze();
      delay(100);
    }

    // Increase the number of levels completed
    levelsCompleted++;

    // Move on to the next level
    currentLevel = levels[currentLevel].nextLevels[0];
    if (currentLevel >= 5) {
      // If there are no more levels, reset to the first level
      currentLevel = 0;
    }

    // Find the new start position
    findStartPosition();

    // Draw the maze for the new level
    drawMaze();

    // Reset the level start time
    levelStartTime = millis();
  }

  // Redraw the player
  drawPlayer();

  // Wait a bit before the next loop iteration
  delay(100);

  // Turn off the player's dot
  lc.setLed(0, playerY, playerX, false);

  // Wait a bit before turning the dot back on
  delay(100);
}

void findStartPosition() {
  for (int y = 0; y < 8; y++) {
    for (int x = 0; x < 8; x++) {
      if (levels[currentLevel].layout[y][x] == SRT) {
        playerX = x;
        playerY = y;
      }
    }
  }
}

void drawMaze() {
  for (int y = 0; y < 8; y++) {
    for (int x = 0; x < 8; x++) {
      lc.setLed(0, y, x, levels[currentLevel].layout[y][x] == 1);
    }
  }
}

void drawPlayer() {
  lc.setLed(0, playerY, playerX, true);
}
