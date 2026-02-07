# ESP8266_RetroPocket_Arduboy

> 基于 ESP8266 和 SSD1306/SSD1315 OLED 的 Arduboy 游戏机开源复刻方案，支持原生游戏运行，并预留 Wi-Fi 扩展方向。

---

## 📖 项目简介

本项目旨在将经典 Arduboy 手持游戏机移植到 ESP8266 平台。相比原版，ESP8266 具备：

- **更强性能**：最高 160MHz 主频。
- **更大存储**：常见 4MB Flash，可容纳更多游戏资源。
- **无线潜力**：支持 OTA / 网络功能扩展（部分能力处于规划与实验阶段）。

---

## ✨ 项目特性

- Arduboy 风格输入与渲染适配（ESP8266 + OLED）。
- 支持多款游戏运行与移植流程沉淀。
- 提供移植、上传排错、硬件接线等中文文档。
- 提供开发模板，便于新游戏快速起步。

---

## 🛠️ 硬件清单

### 1. 硬件材料

| 名称 | 型号/规格 |
|---|---|
| 主控板 | NodeMCU v3（ESP8266-12E，CH340） |
| 屏幕 | 0.96" OLED（SSD1315/SSD1306，I2C） |
| 按键 | 6 个微动按键（上下左右 A/B） |
| 线材 | USB 数据线、杜邦线/焊线 |

### 2. 按键引脚定义

| 功能 | GPIO | NodeMCU 标注 |
|---|---|---|
| UP | 14 | D5 |
| DOWN | 12 | D6 |
| LEFT | 13 | D7 |
| RIGHT | 0 | D3 |
| A | 2 | D4 |
| B | 3 | RX |

> 启动注意：`RIGHT` 使用 `GPIO0(D3)`，上电或复位时不要按住，否则可能进入下载模式。

### 3. OLED I2C 接线

| OLED 引脚 | NodeMCU 引脚 | 说明 |
|---|---|---|
| GND | GND | 地线 |
| VCC | 3.3V | 供电 |
| SCL | D1 (GPIO5) | I2C 时钟 |
| SDA | D2 (GPIO4) | I2C 数据 |

### 4. Arduino IDE 推荐配置

| 选项 | 推荐值 |
|---|---|
| 开发板 | NodeMCU 1.0 (ESP-12E Module) |
| CPU 频率 | 160 MHz |
| Flash Size | 4MB (FS:2MB OTA:~1019KB) |
| Upload Speed | 115200 |

### 5. 软件与依赖

- Arduino IDE：2.3.6（建议）
- ESP8266 Core：2.7.4（当前项目验证稳定）
- CH340 驱动：Windows 必装

---

## 🚀 快速开始

1. 阅读硬件与接线：`doc/相关信息.md`
2. 完成串口与上传连通：`doc/上传指南.md`
3. 打开示例游戏并编译上传（如 `games/tetris/tetris.ino`）
4. 开始移植新游戏：`doc/游戏移植指南.md`
5. 参考开发教程创建新项目：`doc/游戏开发教程.md`

---

## 🎓 移植与开发教程

- 游戏移植教程：`doc/游戏移植指南.md`
- 游戏开发教程：`doc/游戏开发教程.md`

---

## 📚 文档导航

- 复刻总览：`doc/ESPboy_Arduboy_Port.md`
- 上传排错：`doc/上传指南.md`
- 游戏移植：`doc/游戏移植指南.md`
- 游戏开发：`doc/游戏开发教程.md`
- 文档总览：`doc/README.md`
- 开源索引：`doc/OPEN_SOURCE_INDEX.md`

---

## 🎮 资源与游戏下载

- 项目内置/移植游戏：`games/`
- Arduboy 官方与社区资源：
  - https://arduboy.com/
  - https://community.arduboy.com/
  - https://github.com/topics/arduboy

---

## 📁 仓库结构

```text
ESPboy_esp8266/
├─ doc/         # 复刻、上传、移植、路线图文档
├─ games/       # 已移植游戏与模板
├─ libraries/   # 适配依赖库
├─ starter/     # 开发起步模板
└─ tools/       # 工具与脚本说明
```

---

## 🧩 3D 外壳设计

- 设计状态：**进行中**
- 后续将补充：
  - 外壳结构图
  - 按键布局建议
  - 打印参数与装配说明

---

## 🗺️ 路线图（Roadmap）

- 复刻教程完善（图示化、排错流程化）
- 游戏移植标准化（模板 + manifest + 验收清单）
- 开发体验优化（starter、工具链、自动化检查）
- Wi-Fi 扩展（OTA、联网功能实验）

详见：`doc/open-source-roadmap/ROADMAP.md`

---

## 🤝 贡献

欢迎提交 Issue / PR，特别是以下方向：

- 新游戏移植与兼容性修复
- 性能瓶颈优化（如 I2C 刷新导致掉帧）
- 文档完善（复刻、排错、开发教程）

贡献前请阅读：`CONTRIBUTING.md`

---

## ⚖️ 开源协议

- 项目许可证：`LICENSE`
- 第三方组件说明：`THIRD_PARTY.md`

---

## 📮 联系方式

- 作者：Eagle
- 邮箱：2170257193@qq.com
- 项目地址：https://github.com/EagleFandel/ESP8266_RetroPocket_Arduboy

---

## 🙏 致谢

感谢 Arduboy 社区、ESP8266 开发者社区，以及所有参与测试与贡献的朋友。

特别致谢：

- `cheungbx/esp8266_arduboy2`：https://github.com/cheungbx/esp8266_arduboy2
- 本项目在早期 ESP8266 + Arduboy2 适配思路上，参考了其 README 中关于按键映射、移植步骤与平台限制的经验总结。
