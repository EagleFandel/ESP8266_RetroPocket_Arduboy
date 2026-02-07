# 工具目录

本目录用于存放资源转换、地图处理、字符串生成等辅助脚本及说明。

## 原则

- 工具流程应可复现、可脚本化。
- 不提交生成型依赖（如 `node_modules`）。
- 每个工具明确运行环境（`node`、`python` 等）与版本要求。

## 建议结构

```text
tools/
  <tool_name>/
    README.md
    package.json / requirements.txt
    src/
```
