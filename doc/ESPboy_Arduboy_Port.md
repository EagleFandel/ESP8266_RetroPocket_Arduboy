# ESP8266 + Arduboy2：打造自制复古掌机并移植 Arduman

> 记录我用 NodeMCU v3 + SSD1315 OLED + 微动按键实现“ESPboy”掌机，并成功移植 Arduman（吃豆人）到 ESP8266 的完整流程。

---

## 1. 硬件材料与接线

| 名称     | 型号/规格                                   |
|----------|---------------------------------------------|
| 主控板   | NodeMCU v3 Type-C (CH340, ESP8266-12E)      |
| 屏幕     | 0.96 寸 OLED，SSD1315 驱动，I2C 四针（黄蓝） |
| 按键     | 6 × 微动按钮                                |
| 其它     | 面包板 / PCB、若干杜邦线、Type-C 数据线、可选 3D 外壳 |

### 1.1 按键引脚

| 功能  | GPIO | NodeMCU 引脚 |
|-------|------|--------------|
| UP    | 14   | D5           |
| DOWN  | 12   | D6           |
| LEFT  | 13   | D7           |
| RIGHT | 0    | D3 (需注意启动模式) |
| A     | 2    | D4 |
| B     | 3    | RX (与串口 RX 复用) |

> ⚠️ D3 (GPIO0) 在上电/复位时需保持高电平，否则会进入刷机模式。上传或上电时不要按住 RIGHT 键。

### 1.2 OLED I2C 接线

| OLED 引脚 | NodeMCU 引脚 | 说明                |
|-----------|--------------|---------------------|
| GND       | GND          | 电源地              |
| VCC       | 3.3V         | 供电                |
| SCL       | D1 (GPIO5)   | I2C 时钟            |
| SDA       | D2 (GPIO4)   | I2C 数据            |

### 1.3 复刻步骤

1. **焊接/固定按键**：将 6 个微动按钮按游戏手感布置在面板上，公共端接 GND，另一端分别引出到 D5/D6/D7/D3/D4/RX。
2. **连接 OLED**：
   - VCC → 3.3V，GND → GND
   - SDA → D2，SCL → D1（建议使用杜邦线或焊接排线）
3. **供电与外壳**：USB Type-C 直接为 NodeMCU 供电；若有 3D 外壳，可在此阶段固定面板与主板。
4. **再次检查**：用万用表确认各按键未短路、OLED 通电脚位无接反。

### 1.4 初步硬件自检

1. 仅连接 USB 供电，确认 NodeMCU 能正常点亮。
2. 编译并上传一个“按键/屏幕测试”示例（例如 Arduboy2 的 Buttons Demo）：
   - 如果按键读数异常，重点检查 D3/GPIO0 与 RX/GPIO3 的焊点。
   - 若 OLED 黑屏，优先确认 SDA/SCL 是否接反或 I2C 地址是否为 `0x3C`。

---

## 2. 软件环境

- Arduino IDE 2.3.6
- ESP8266 开发板包（推荐 2.7.4（3.x版本`brzo_i2c` 会报 asm 错误，所以暂降至2.7.4 ））
- CH340 驱动

常用库（全部放在 `C:/Users/你的用户名/Documents/Arduino/libraries`）：

| 库名        | 用途                         |
|-------------|------------------------------|
| Arduboy2    | 游戏框架（我们替换到 ESP8266 版） |
| brzo_i2c    | SSD1306Brzo 所需的快速 I2C         |
| ESP8266_and_ESP32_OLED_driver_for_SSD1306_displays | OLED 驱动，基于 brzo_i2c         |

---

## 3. 移植流程（以 Arduman 为例）

### 3.1 复制源码到草图目录

1. 下载 `arduman-master` 源码。
2. 在 `.../arduman-master/` 下创建 `arduman/` 子目录，把 `.ino/.cpp/.h` 全部放进去。
3. 确保结构类似：
   ```
   arduman-master/
     arduman/
       arduman.ino
       ardu_main.cpp/h
       Player.cpp/h
       Ghost.cpp/h
       Utils.cpp/h
       Highscores.cpp/h
       Fruit.cpp/h
       level_bitmaps.h
   ```

### 3.2 头文件与类替换

1. `#include <Arduboy.h>` → `#include <Arduboy2.h>`，`Arduboy` 对象改为 `Arduboy2`。
2. `arduboy.getInput()` 改用 `arduboy.buttonsState()`。
3. 删除 `ArduboyPlaytune`，加一个空实现：
   ```cpp
   class DummyTunes {
   public:
     void initChannel(uint8_t) {}
     void playScore(const uint8_t *) {}
     void tone(uint16_t, uint16_t) {}
   };
   DummyTunes tunes;
   ```
4. 原 `AbPrinter text` 全部改用 `arduboy.setCursor/print`。

### 3.3 EEPROM / 按键 / 其它

- 在 `main_setup()` 中添加 `EEPROM.begin(1024);`，写入后 `EEPROM.commit();`
- 按键宏在 `Arduboy2Core.h` 中自定义（D5/D6/D7/D3/D4/RX）。
- 如果游戏用 `pgm_read_word` 操作函数指针，需换成 `pgm_read_dword`。

### 3.4 常见报错排查

| 报错                                        | 解决方案                     |
|---------------------------------------------|------------------------------|
| `ArduboyPlaytune.h: No such file`          | 移除该库，使用 DummyTunes    |
| `redefined PIN_*`                           | 确认 `#elif !defined(ESP8266)` 包裹 AVR 代码 |
| `RX was not declared`                       | 直接用 `#define PIN_B_BUTTON 3` |
| `Brzo_I2C asm ... impossible constraints`   | 在 `brzo_i2c.c` 顶部添加 `#pragma GCC optimize("O2")` 或降级 Core |
| `class Arduboy2 has no member getInput`     | 换用 `buttonsState()`         |
| 上传后黑屏/无法启动                         | 上电时不要按住 RIGHT(D3/GPIO0)，检查 I2C 线序 |

### 3.5 修改 Arduboy2 库（必做）

为了让 ESP8266 使用自定义按钮引脚，必须改动官方 `Arduboy2Core.h`：

1. 打开 `Documents/Arduino/libraries/Arduboy2/src/Arduboy2Core.h`
2. 找到 `SLIMBOY` 分支后紧接的 `#else`（约 150 行）
3. 把它改成 `#elif !defined(ESP8266)`，确保 AVR 引脚定义只在非 ESP8266 平台启用

修改前：

```c
#else
// AVR 引脚定义...
```

修改后：

```c
#elif !defined(ESP8266)
// AVR 引脚定义...
```

> 这样 ESP8266 才会继续往下执行自定义 NodeMCU 引脚区块，不会被 AVR 默认值覆盖。

---

## 4. 编译与上传

1. 打开 `arduman.ino`
2. 开发板选择 `NodeMCU 1.0 (ESP-12E Module)`，选择正确的 COM 口
3. 点击上传（`Ctrl+U`）
4. 上传过程中不要按任何按键；若失败，可按住板载 Flash 键后重试

---

## 5. 可选扩展

- **声音**：引入 ESPboy 定制的 `ArduboyTones`，或用 `tone()` 自行驱动蜂鸣器
- **更多游戏移植**：重复上述流程；注意 EEPROM / PROGMEM 差异
- **多合一 Launcher**：用 LittleFS 存多个固件，写菜单通过 `ESP.updateSketch()` 切换
- **分享成果**：从硬件复刻、接线、测试到移植的经验都可整理为图文/视频，欢迎上传 GitHub / B 站，帮助更多人入坑

---

> 若需追加截图、接线示意或更详细的代码 diff，可以在此文档基础上继续完善。
