# Sentinel-Red: Automated TTP Emulation Engine

## Overview

**Sentinel-Red** is a sophisticated **Automated Tactics, Techniques, and Procedures (TTP) Emulation Engine** built in C++. It is designed to assist security professionals and red teams in simulating adversary behaviors across local and remote environments. By executing YAML-defined playbooks, Sentinel-Red provides a structured way to test defensive controls and generate high-quality, actionable security reports.

## Key Features

*   **YAML-Driven Playbooks**: Easily define and customize attack scenarios using a simple YAML schema.
*   **Remote SSH Execution**: Seamlessly run campaigns against remote Linux and Windows targets using secure SSH connections.
*   **Professional HTML Reporting**: Automatically generates a dark-themed dashboard report featuring:
    *   **Campaign Statistics**: Total techniques executed, clean vs. detected counts, and overall detection rate.
    *   **MITRE ATT&CK® Mapping**: Every technique is mapped to its corresponding MITRE ATT&CK ID with direct links for further research.
    *   **Detailed Output Logs**: Capture and display the full output of each command executed during the emulation.
*   **Detection Simulation**: Built-in logic to identify potentially "detected" activities based on common malicious patterns.
*   **Cross-Platform Support**: Tailored techniques for both Windows and Linux environments.

## Dashboard Preview

The engine produces a comprehensive HTML report that provides an immediate overview of the security posture:

![Sentinel-Red Report Dashboard](https://raw.githubusercontent.com/tamimuftb/sentinel-red/main/docs/report_preview.png)
*(Note: Replace the URL above with the actual path to your screenshot in the repository)*

## Usage

Sentinel-Red supports both local and remote operation modes.

### Local Mode
Execute a playbook on the current machine:
```bash
./sentinel-red playbooks/linux_recon.yaml [report.html]
```

### Remote Mode (SSH)
Execute a playbook against a remote target:
```bash
./sentinel-red playbooks/windows_recon.yaml <host> <user> <pass> [port] [report.html]
```

## Project Structure

*   `src/`: Core engine implementation (C++).
*   `include/`: Header files and interface definitions.
*   `playbooks/`: Sample YAML playbooks for reconnaissance and detection testing.
*   `build/`: Build artifacts and compiled binaries.
*   `sentinel_report.html`: The final generated emulation report.

## Requirements

*   **C++17** compatible compiler.
*   **CMake** 3.10+.
*   **libssh2**: For remote execution capabilities.
*   **yaml-cpp**: For parsing playbook files.

---
**Disclaimer**: This tool is for authorized security testing and educational purposes only. Always obtain proper consent before running emulations against any system.
