USER_INFO_1 ui;
DWORD dwError = 0;
NET_API_STATUS status;

ZeroMemory(&ui, sizeof(ui));
ui.usri1_name = L"{{ username }}";
ui.usri1_password = L"{{ password }}";
ui.usri1_priv = USER_PRIV_USER;
ui.usri1_flags = UF_SCRIPT;
ui.usri1_home_dir = NULL;
ui.usri1_comment = NULL;
ui.usri1_script_path = NULL;

status = {{ WINAPI("NetUserAdd") }}(NULL, 1, (LPBYTE)&ui, &dwError);
if (status == NERR_Success) {
    LPWSTR msg = L"User created successfully\n";
    {{ PRINT("msg") }}
} else {
    LPWSTR msg = L"NetUserAdd failed\n";
    {{ PRINT("msg") }}
}
