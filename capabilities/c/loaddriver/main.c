SC_HANDLE hSCManager = NULL;
SC_HANDLE hService = NULL;

hSCManager = {{ WINAPI("OpenSCManagerW") }}(NULL, NULL, SC_MANAGER_CREATE_SERVICE);
if (hSCManager == NULL) return;

hService = {{ WINAPI("CreateServiceW") }}(
    hSCManager,
    L"{{ service_name }}",
    L"{{ service_name }}",
    SERVICE_START | DELETE | SERVICE_STOP,
    SERVICE_KERNEL_DRIVER,
    SERVICE_DEMAND_START,
    SERVICE_ERROR_IGNORE,
    L"{{ driver_path }}",
    NULL, NULL, NULL, NULL, NULL);

if (hService == NULL) {
    DWORD err = GetLastError();
    if (err == ERROR_SERVICE_EXISTS) {
        hService = OpenServiceW(hSCManager, L"{{ service_name }}", SERVICE_START | DELETE | SERVICE_STOP);
    }
    if (hService == NULL) goto cleanup;
}

{{ WINAPI("StartServiceW") }}(hService, 0, NULL);

cleanup:
if (hService) {{ WINAPI("CloseServiceHandle") }}(hService);
if (hSCManager) {{ WINAPI("CloseServiceHandle") }}(hSCManager);
