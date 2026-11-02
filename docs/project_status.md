# Project status

This repository is an upload of the source snapshot provided by the team.

## Included

- Original PS application sources and headers, unchanged.
- Original custom AXI IP drivers.
- Original hardware export files and legacy executable.
- Import/build guidance and source provenance hashes.

## Not verified in this upload

- Fresh PS compilation, linking and board execution.
- Full Vivado reconstruction from RTL/IP/XDC sources.
- Reported closed-loop performance, resource utilization and timing.
- Presence of the final PSO-LADRC deployment code and optimization inputs.

## Final competition additions

Add the complete Vivado project, custom IP sources and constraints; the final
control implementation and parameter/optimization files; simulation and board
test scripts; measured datasets; resource/timing reports; reusable Skill and
the final report. Record a commit identifier for every tested build.

Some original sources use a legacy Chinese text encoding. Preserve or convert
it deliberately in a separate reviewed commit instead of mixing encoding
changes with this provenance-preserving upload.
