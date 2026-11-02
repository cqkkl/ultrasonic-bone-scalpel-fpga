---
name: fpga-control-audit
description: Audit FPGA and SoC control projects for feedback units, control-loop integration, source completeness, and reproducible evidence. Use for reviewing existing control code or preparing a traceable engineering submission, not for certifying hardware performance.
---

# FPGA控制工程与证据审查

## 需要保持的区分

- XSA、BIT、ELF是导出或构建产物，不能据此认定RTL/IP/XDC或控制器源码齐全。
- 函数定义、控制循环调用、固件构建成功、硬件实测通过是不同状态；分别给出证据。
- 采样率决定采样序列的FFT频点映射，处理时钟决定计算吞吐能力；除非确有重采样，不替换二者。
- 功率反馈、电流RMS与声功率不同；根据实际模型、测量边界和缩放核对定义。
- 用户设定的假设可以用于方案写作，不能改写为已观察到的实验事实。

## 执行流程

先读仓库说明与任务约束，清点HDL/HLS、PS软件、自定义IP、约束、参数、模型、构建脚本及实测数据。若需要自动清点，运行`scripts/audit_project.py <repo>`，按模式解释结果，不将提醒自动当作缺陷。

控制审查按每条环路分别记录：参考值、反馈来源、单位与缩放、帧一致性、更新周期、状态更新、执行量和限幅。追踪数据从采样入口直到实际寄存器写回，不以变量赋值代替执行确认。核对运行态是否仍被启动常量覆盖。

检查共享外设和中断对象由谁初始化、生命周期是否覆盖回调、重复初始化是否影响已注册中断。标记需要编译或硬件确认的风险，不仅依据命名作结论。

如需报告或竞赛材料，建立“主张—源码/配置—构建版本—验证记录”对应表。比较控制方法时固定被控对象、目标、输入输出定义、采样/控制周期和执行约束；分环比较后再比较联合方案，不把代码错误修复收益混作算法创新收益。

在用户仅要求审查时输出发现与下一步验证，不改源码。获准修改后，仅修正已确定范围；频率、增益和保护阈值的改变需要对象或测量依据。文件上传与硬件操作按用户授权范围执行。

## 按需读取

- 需要给另一模型复用任务：读[提示词模板](references/prompts.md)。
- 需要定位相位、PWM、PID、跨时钟或中断问题：读[问题定位清单](references/troubleshooting.md)。

输出列出已确认事实、潜在风险、未验证项和最短验证路径，给出具体文件或结果索引。保留失败记录，不将脚本通过解释为设计通过。
