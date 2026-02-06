/*
 * ESP8266 俄罗斯方块
 * 为ESPboy/Arduboy设计
 * 
 * 操作说明：
 * 左/右 - 移动方块
 * 上 - 旋转方块
 * 下 - 快速下落
 * A - 暂停
 * B - 开始游戏
 */

#include <Arduboy2.h>
#include <EEPROM.h>

Arduboy2 arduboy;

// 游戏常量
#define BOARD_WIDTH 10
#define BOARD_HEIGHT 20
#define BLOCK_SIZE 3
#define BOARD_X 2
#define BOARD_Y 2

// EEPROM地址
#define EEPROM_HIGH_SCORE 100

// 游戏状态
enum GameState {
  STATE_MENU,
  STATE_PLAYING,
  STATE_PAUSED,
  STATE_GAME_OVER
};

GameState gameState = STATE_MENU;

// 游戏板
uint8_t board[BOARD_HEIGHT][BOARD_WIDTH];

// 方块形状定义 (7种方块)
const uint8_t SHAPES[7][4][4] PROGMEM = {
  // I 形
  {
    {0,0,0,0},
    {1,1,1,1},
    {0,0,0,0},
    {0,0,0,0}
  },
  // O 形
  {
    {0,0,0,0},
    {0,1,1,0},
    {0,1,1,0},
    {0,0,0,0}
  },
  // T 形
  {
    {0,0,0,0},
    {0,1,0,0},
    {1,1,1,0},
    {0,0,0,0}
  },
  // S 形
  {
    {0,0,0,0},
    {0,1,1,0},
    {1,1,0,0},
    {0,0,0,0}
  },
  // Z 形
  {
    {0,0,0,0},
    {1,1,0,0},
    {0,1,1,0},
    {0,0,0,0}
  },
  // J 形
  {
    {0,0,0,0},
    {1,0,0,0},
    {1,1,1,0},
    {0,0,0,0}
  },
  // L 形
  {
    {0,0,0,0},
    {0,0,1,0},
    {1,1,1,0},
    {0,0,0,0}
  }
};

// 当前方块
struct Piece {
  int8_t x, y;
  uint8_t type;
  uint8_t rotation;
  uint8_t shape[4][4];
} currentPiece, nextPiece;

// 游戏变量
uint32_t score = 0;
uint32_t highScore = 0;
uint16_t lines = 0;
uint8_t level = 1;
uint32_t lastMoveTime = 0;
uint16_t moveDelay = 500;

// 函数声明
void initGame();
void newPiece();
void loadShape(Piece* piece);
bool checkCollision(int8_t offsetX, int8_t offsetY, uint8_t rotation);
void mergePiece();
void clearLines();
void rotatePiece();
void movePiece(int8_t dx);
void dropPiece();
void drawBoard();
void drawPiece(const Piece* piece, int8_t offsetX, int8_t offsetY);
void drawUI();
void updateGame();
void handleInput();
void showMenu();
void showGameOver();
void loadHighScore();
void saveHighScore();

void setup() {
  arduboy.begin();
  arduboy.setFrameRate(60);
  arduboy.initRandomSeed();
#ifdef ESP8266
  EEPROM.begin(1024);
#endif
  loadHighScore();
}

void loop() {
  if (!arduboy.nextFrame()) return;
  
  arduboy.pollButtons();
  arduboy.clear();
  
  switch (gameState) {
    case STATE_MENU:
      showMenu();
      if (arduboy.justPressed(B_BUTTON)) {
        initGame();
        gameState = STATE_PLAYING;
      }
      break;
      
    case STATE_PLAYING:
      handleInput();
      updateGame();
      drawBoard();
      drawUI();
      break;
      
    case STATE_PAUSED:
      drawBoard();
      drawUI();
      arduboy.setCursor(30, 28);
      arduboy.print(F("PAUSED"));
      if (arduboy.justPressed(A_BUTTON)) {
        gameState = STATE_PLAYING;
      }
      break;
      
    case STATE_GAME_OVER:
      showGameOver();
      if (arduboy.justPressed(B_BUTTON)) {
        gameState = STATE_MENU;
      }
      break;
  }
  
  arduboy.display();
}

void initGame() {
  // 清空游戏板
  for (uint8_t y = 0; y < BOARD_HEIGHT; y++) {
    for (uint8_t x = 0; x < BOARD_WIDTH; x++) {
      board[y][x] = 0;
    }
  }
  
  score = 0;
  lines = 0;
  level = 1;
  moveDelay = 500;
  
  // 生成第一个方块
  nextPiece.type = random(7);
  loadShape(&nextPiece);
  newPiece();
}

void newPiece() {
  currentPiece = nextPiece;
  currentPiece.x = BOARD_WIDTH / 2 - 2;
  currentPiece.y = 0;
  currentPiece.rotation = 0;
  
  // 生成下一个方块
  nextPiece.type = random(7);
  loadShape(&nextPiece);
  
  // 检查游戏是否结束
  if (checkCollision(0, 0, 0)) {
    gameState = STATE_GAME_OVER;
    if (score > highScore) {
      highScore = score;
      saveHighScore();
    }
  }
}

void loadShape(Piece* piece) {
  for (uint8_t y = 0; y < 4; y++) {
    for (uint8_t x = 0; x < 4; x++) {
      piece->shape[y][x] = pgm_read_byte(&SHAPES[piece->type][y][x]);
    }
  }
}

bool checkCollision(int8_t offsetX, int8_t offsetY, uint8_t rotation) {
  for (uint8_t y = 0; y < 4; y++) {
    for (uint8_t x = 0; x < 4; x++) {
      if (currentPiece.shape[y][x]) {
        // 旋转坐标
        int8_t newX, newY;
        if (rotation == 1) { // 顺时针90度
          newX = 3 - y;
          newY = x;
        } else {
          newX = x;
          newY = y;
        }
        
        int8_t boardX = currentPiece.x + newX + offsetX;
        int8_t boardY = currentPiece.y + newY + offsetY;
        
        // 检查边界
        if (boardX < 0 || boardX >= BOARD_WIDTH || 
            boardY >= BOARD_HEIGHT) {
          return true;
        }
        
        // 检查与已有方块的碰撞
        if (boardY >= 0 && board[boardY][boardX]) {
          return true;
        }
      }
    }
  }
  return false;
}

void mergePiece() {
  for (uint8_t y = 0; y < 4; y++) {
    for (uint8_t x = 0; x < 4; x++) {
      if (currentPiece.shape[y][x]) {
        int8_t boardX = currentPiece.x + x;
        int8_t boardY = currentPiece.y + y;
        if (boardY >= 0 && boardY < BOARD_HEIGHT && 
            boardX >= 0 && boardX < BOARD_WIDTH) {
          board[boardY][boardX] = currentPiece.type + 1;
        }
      }
    }
  }
}

void clearLines() {
  uint8_t linesCleared = 0;
  
  for (int8_t y = BOARD_HEIGHT - 1; y >= 0; y--) {
    bool fullLine = true;
    for (uint8_t x = 0; x < BOARD_WIDTH; x++) {
      if (board[y][x] == 0) {
        fullLine = false;
        break;
      }
    }
    
    if (fullLine) {
      linesCleared++;
      // 下移上面的行
      for (int8_t yy = y; yy > 0; yy--) {
        for (uint8_t x = 0; x < BOARD_WIDTH; x++) {
          board[yy][x] = board[yy - 1][x];
        }
      }
      // 清空顶行
      for (uint8_t x = 0; x < BOARD_WIDTH; x++) {
        board[0][x] = 0;
      }
      y++; // 重新检查当前行
    }
  }
  
  if (linesCleared > 0) {
    lines += linesCleared;
    // 计分：1行=100, 2行=300, 3行=500, 4行=800
    uint16_t points[] = {0, 100, 300, 500, 800};
    score += points[linesCleared] * level;
    
    // 升级
    level = lines / 10 + 1;
    moveDelay = 500 - (level - 1) * 30;
    if (moveDelay < 100) moveDelay = 100;
  }
}

void rotatePiece() {
  if (!checkCollision(0, 0, 1)) {
    // 旋转方块
    uint8_t temp[4][4];
    for (uint8_t y = 0; y < 4; y++) {
      for (uint8_t x = 0; x < 4; x++) {
        temp[x][3-y] = currentPiece.shape[y][x];
      }
    }
    for (uint8_t y = 0; y < 4; y++) {
      for (uint8_t x = 0; x < 4; x++) {
        currentPiece.shape[y][x] = temp[y][x];
      }
    }
  }
}

void movePiece(int8_t dx) {
  if (!checkCollision(dx, 0, 0)) {
    currentPiece.x += dx;
  }
}

void dropPiece() {
  while (!checkCollision(0, 1, 0)) {
    currentPiece.y++;
    score += 2; // 快速下落奖励
  }
  mergePiece();
  clearLines();
  newPiece();
}

void drawBoard() {
  // 绘制边框
  arduboy.drawRect(BOARD_X - 1, BOARD_Y - 1, 
                   BOARD_WIDTH * BLOCK_SIZE + 2, 
                   BOARD_HEIGHT * BLOCK_SIZE + 2);
  
  // 绘制已固定的方块
  for (uint8_t y = 0; y < BOARD_HEIGHT; y++) {
    for (uint8_t x = 0; x < BOARD_WIDTH; x++) {
      if (board[y][x]) {
        arduboy.fillRect(BOARD_X + x * BLOCK_SIZE, 
                        BOARD_Y + y * BLOCK_SIZE,
                        BLOCK_SIZE - 1, BLOCK_SIZE - 1);
      }
    }
  }
  
  // 绘制当前方块
  drawPiece(&currentPiece, 0, 0);
}

void drawPiece(const Piece* piece, int8_t offsetX, int8_t offsetY) {
  for (uint8_t y = 0; y < 4; y++) {
    for (uint8_t x = 0; x < 4; x++) {
      if (piece->shape[y][x]) {
        int8_t drawX = piece->x + x + offsetX;
        int8_t drawY = piece->y + y + offsetY;
        if (drawY >= 0) {
          arduboy.fillRect(BOARD_X + drawX * BLOCK_SIZE,
                          BOARD_Y + drawY * BLOCK_SIZE,
                          BLOCK_SIZE - 1, BLOCK_SIZE - 1);
        }
      }
    }
  }
}

void drawUI() {
  // 分数
  arduboy.setCursor(35, 2);
  arduboy.print(F("SCORE"));
  arduboy.setCursor(35, 10);
  arduboy.print(score);
  
  // 行数
  arduboy.setCursor(35, 20);
  arduboy.print(F("LINES"));
  arduboy.setCursor(35, 28);
  arduboy.print(lines);
  
  // 等级
  arduboy.setCursor(35, 38);
  arduboy.print(F("LEVEL"));
  arduboy.setCursor(35, 46);
  arduboy.print(level);
  
  // 下一个方块
  arduboy.setCursor(80, 2);
  arduboy.print(F("NEXT"));
  arduboy.drawRect(78, 10, 18, 18);
  
  // 绘制下一个方块（缩小版）
  for (uint8_t y = 0; y < 4; y++) {
    for (uint8_t x = 0; x < 4; x++) {
      if (nextPiece.shape[y][x]) {
        arduboy.fillRect(80 + x * 3, 12 + y * 3, 2, 2);
      }
    }
  }
  
  // 最高分
  arduboy.setCursor(80, 32);
  arduboy.print(F("HIGH"));
  arduboy.setCursor(80, 40);
  arduboy.print(highScore);
}

void updateGame() {
  uint32_t currentTime = millis();
  
  if (currentTime - lastMoveTime >= moveDelay) {
    if (!checkCollision(0, 1, 0)) {
      currentPiece.y++;
      score += 1;
    } else {
      mergePiece();
      clearLines();
      newPiece();
    }
    lastMoveTime = currentTime;
  }
}

void handleInput() {
  if (arduboy.justPressed(LEFT_BUTTON)) {
    movePiece(-1);
  }
  
  if (arduboy.justPressed(RIGHT_BUTTON)) {
    movePiece(1);
  }
  
  if (arduboy.justPressed(UP_BUTTON)) {
    rotatePiece();
  }
  
  if (arduboy.pressed(DOWN_BUTTON)) {
    if (!checkCollision(0, 1, 0)) {
      currentPiece.y++;
      score += 1;
      lastMoveTime = millis();
    }
  }
  
  if (arduboy.justPressed(B_BUTTON)) {
    dropPiece();
  }
  
  if (arduboy.justPressed(A_BUTTON)) {
    gameState = STATE_PAUSED;
  }
}

void showMenu() {
  arduboy.setCursor(20, 10);
  arduboy.print(F("TETRIS"));
  
  arduboy.setCursor(10, 25);
  arduboy.print(F("Press B to Start"));
  
  arduboy.setCursor(10, 40);
  arduboy.print(F("High Score:"));
  arduboy.print(highScore);
  
  // 闪烁效果
  if ((millis() / 500) % 2) {
    arduboy.setCursor(25, 55);
    arduboy.print(F("ESPboy"));
  }
}

void showGameOver() {
  arduboy.setCursor(20, 15);
  arduboy.print(F("GAME OVER"));
  
  arduboy.setCursor(15, 30);
  arduboy.print(F("Score: "));
  arduboy.print(score);
  
  arduboy.setCursor(15, 40);
  arduboy.print(F("Lines: "));
  arduboy.print(lines);
  
  if (score > highScore) {
    arduboy.setCursor(10, 50);
    arduboy.print(F("NEW HIGH SCORE!"));
  }
}

void loadHighScore() {
  highScore = EEPROM.read(EEPROM_HIGH_SCORE) | 
              (EEPROM.read(EEPROM_HIGH_SCORE + 1) << 8) |
              (EEPROM.read(EEPROM_HIGH_SCORE + 2) << 16) |
              (EEPROM.read(EEPROM_HIGH_SCORE + 3) << 24);
  
  // 验证数据
  if (highScore > 999999) {
    highScore = 0;
  }
}

void saveHighScore() {
  EEPROM.write(EEPROM_HIGH_SCORE, highScore & 0xFF);
  EEPROM.write(EEPROM_HIGH_SCORE + 1, (highScore >> 8) & 0xFF);
  EEPROM.write(EEPROM_HIGH_SCORE + 2, (highScore >> 16) & 0xFF);
  EEPROM.write(EEPROM_HIGH_SCORE + 3, (highScore >> 24) & 0xFF);
  
  #ifdef ESP8266
  EEPROM.commit();
  #endif
}
