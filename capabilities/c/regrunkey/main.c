HKEY hKey = NULL;
LONG result;
LPCWSTR subKey = L"Software\\Microsoft\\Windows\\CurrentVersion\\Run";
LPCWSTR valueName = L"{{ key_name }}";
LPCWSTR valueData = L"{{ command }}";
DWORD dataSize = (DWORD)((wcslen(valueData) + 1) * sizeof(WCHAR));

result = {{ WINAPI("RegOpenKeyExW") }}(HKEY_CURRENT_USER, subKey, 0, KEY_SET_VALUE, &hKey);
if (result != ERROR_SUCCESS) {
    LPWSTR msg = L"RegOpenKeyExW failed\n";
    {{ PRINT("msg") }}
    return;
}

result = {{ WINAPI("RegSetValueExW") }}(hKey, valueName, 0, REG_SZ, (const BYTE *)valueData, dataSize);
if (result == ERROR_SUCCESS) {
    LPWSTR msg = L"Registry Run key added successfully\n";
    {{ PRINT("msg") }}
} else {
    LPWSTR msg = L"RegSetValueExW failed\n";
    {{ PRINT("msg") }}
}

{{ WINAPI("RegCloseKey") }}(hKey);
