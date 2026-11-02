# Ultrasonic Bone Scalpel FPGA Control System

超声骨刀声功智控系统的 AMD Zynq-7020 / Vitis 工程代码与硬件导出文件。

本仓库上传自团队提供的 Vitis workspace。`src/ps/` 与 `src/drivers/` 的文件保留原始内容和编码；没有将其他修订版本的源码替换进本次上传。

## Directory layout

```text
src/ps/src/       PS application C sources, linker script and compiler spec
src/ps/include/   Application headers
src/drivers/      Custom AXI IP drivers from the original workspace
board/           XSA, BIT, MMI, PS initialization files and legacy ELF
docs/            Project status and import instructions
skill/           Reusable project-audit Skill, prompts and read-only checker
report/          AI collaboration record and validation status
```

## Platform

- Target device: AMD Zynq-7000 `xc7z020clg400-2`.
- Original toolchain: Vivado / Vitis 2019.2.
- PS: ARM Cortex-A9, standalone application.
- PL interfaces: DDS frequency control, AXI PWM and BRAM-based feedback.
- Application functions include frequency scanning, PI/PID control, fuzzy parameter calculation, UART, GPIO, timer and measurement decoding. See source files for the exact implementation in this snapshot.

## Import and build

1. Install a compatible Vivado/Vitis release. Start with 2019.2 for this snapshot.
2. Create a Vitis workspace and a hardware platform from `board/top_s2_piezosurgery_shared.xsa`, selecting `ps7_cortexa9_0` and a standalone domain.
3. Add `src/drivers/` as the local driver repository if the custom AXI drivers are not resolved from the XSA, then regenerate the BSP.
4. Create an empty standalone C application. Import `src/ps/src/` and `src/ps/include/`, preserving the sibling `src` and `include` directories used by relative includes.
5. Select `src/ps/src/lscript.ld` as the linker script. Reproduce the original Cortex-A9 / VFPv3 hard-float configuration and supply the generated BSP include and library paths. If required, configure the compiler spec from `src/ps/src/Xilinx.spec`.
6. Build the application and verify compiler/linker diagnostics before downloading it to the board.

These are import instructions for the supplied snapshot. This upload session did not rebuild or test the firmware. The prebuilt `board/firmware_legacy.elf` belongs to the original workspace and is not a new build.

## Skill and collaboration records

- [Reusable FPGA control-project audit Skill](skill/README.md): usage, prompt templates, troubleshooting and a standard-library read-only audit script.
- [AI collaboration log](report/ai_collaboration_log.md): actual tasks, analysis, corrections, local-only changes, upload history and evidence boundaries.
- [Validation status](report/validation_status.md): checks executed for this package and outstanding engineering verification.

The collaboration log separates completed file operations from deployment suggestions and hypothetical report scenarios. Adding this package does not add or validate the final PSO-LADRC implementation.

## License

Project-owned code is provided under the MIT License. Third-party files, including AMD/Xilinx-generated drivers and initialization code, retain their own copyright and license notices. The root license does not replace those notices.
