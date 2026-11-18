HANDLE hEventLog = {{ WINAPI("OpenEventLogW") }}(NULL, L"{{ log_name }}");
if (hEventLog == NULL) return;

{{ WINAPI("ClearEventLogW") }}(hEventLog, NULL);
{{ WINAPI("CloseEventLog") }}(hEventLog);
