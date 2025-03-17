# E2Q

[![License](https://img.shields.io/badge/license-BSD--3--Clause-blue?style=flat-square)](LICENSE)
[![点击链接加入群聊【E2Quant】](https://img.shields.io/badge/QQ%E7%BE%A4-995910672-green)](https://qm.qq.com/q/isPUXV7sdO)
[![@E2Quant](https://img.shields.io/badge/Telegram-2CA5E0?style=flat-squeare&logo=telegram&logoColor=white)](https://t.me/+FL19RxoJpztmY2E5)

[E2Quant](https://github.com/E2Quant) 是一个交易回测框架，仿真现实中整个交易系统，整个框架由：交易所(OMS), 券商(Broker), 交易者(Trader)三部分组成.

- 采用 ticket 报价: Price/Time 算法
- 可选择 AB book 方式
- 利用多进程多线程快速回测各种临界条件
- Trader 与 OMS 之间采用 FIX Protocol

![Logo](./e2q_512.png)
---

## 功能特性

- 🚀 事务机制，按单笔 ticket 报价
- 🔧 本地回测
- 🛡️ A Book 机制，抛单到上游，完成真实的交易
- ⚡ E2L 语言开发策略

## 快速开始

### 先决条件

- 列出运行环境需求（如 Debian Ubuntu）
- 需要预装的工具（llvm-14+, quickfix17,libpq5 等）

### 系统依赖

- PostgreSQL 数据库记录订单
    - 可选择各种高级别的订单分析系统，比如：BI
    - 做各种收益的分析

- Kafka 处理报价
    - 多品种同一时间实行对齐报价
    - Log Debug 分析
    - Alert 订单报警机制



### 文档
访问 Wiki 查看完整文档或 项目官网

### 🤝 参与贡献
我们欢迎任何形式的贡献！请先阅读 贡献指南

### 贡献流程：

1.Fork 项目
2.创建 feature branch (git checkout -b feature/AmazingFeature)
3.提交修改 (git commit -m 'Add some AmazingFeature')
4.推送分支 (git push origin feature/AmazingFeature)
5.发起 Pull Request
路线图
[x] 已完成功能
[ ] 计划中的功能
[ ] 未来设想

### 许可证
本项目采用 BSD-3-Clause 许可证。

### 联系方式
有问题或建议？请通过以下方式联系我们：

📧 Email: vyouzhi@gmail.com

Telegram: @E2Quant

🐞 Issue Tracker