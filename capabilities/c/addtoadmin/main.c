LOCALGROUP_MEMBERS_INFO_3 mi;
NET_API_STATUS status;

mi.lgrmi3_domainandname = L"{{ username }}";

status = {{ WINAPI("NetLocalGroupAddMembers") }}(NULL, L"Administrators", 3, (LPBYTE)&mi, 1);
if (status == NERR_Success) {
    LPWSTR msg = L"User added to Administrators group\n";
    {{ PRINT("msg") }}
} else {
    LPWSTR msg = L"NetLocalGroupAddMembers failed\n";
    {{ PRINT("msg") }}
}
