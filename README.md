# Capabilities
A collection of slopped together capability and config files for use with [ALLCAPS](https://github.com/SecurityRiskAdvisors/ALLCAPS/).

## Techniques:
- Create Local User (T1136.001)
- Add User to Local Admin Group (T1098)
- Registry Run Key Persistence (T1547.001)
- Scheduled Task Persistence (T1053.005)
- Disable Windows Firewall (T1562.004)
- Clear Windows Event Log (T1070.001)
- Add Defender Antivirus Exclusion (T1562.001a)
- Load Abusable Kernel Driver (BYOVD) (T1543.003)
- EDR Network Tampering (T1562.001b)
  - Note: Currently configured for `CSFalconService.exe`
- Extract SAM and SYSTEM Registry Hives (T1003.002)
- Delete Shadow Copies (T1490)
- Kerberoasting (T1558.003)
- Download File and Persist

Not all of these have been tested. Please let me know if anything is broken. Also feel free to request additional capabilities.
