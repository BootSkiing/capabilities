HRESULT hr;
IWbemLocator *pLocator = NULL;
IWbemServices *pServices = NULL;
IEnumWbemClassObject *pEnum = NULL;
IWbemClassObject *pObj = NULL;
ULONG uReturned = 0;
BSTR bstrRoot = NULL;
BSTR bstrWQL = NULL;
BSTR bstrQuery = NULL;

hr = {{ WINAPI("CoInitializeEx") }}(NULL, COINIT_MULTITHREADED);
if (FAILED(hr)) return;

hr = {{ WINAPI("CoInitializeSecurity") }}(NULL, -1, NULL, NULL,
    RPC_C_AUTHN_LEVEL_DEFAULT, RPC_C_IMP_LEVEL_IMPERSONATE,
    NULL, EOAC_NONE, NULL);
if (FAILED(hr) && hr != RPC_E_TOO_LATE) goto cleanup;

hr = {{ WINAPI("CoCreateInstance") }}(&CLSID_WbemLocator, NULL,
    CLSCTX_INPROC_SERVER, &IID_IWbemLocator, (void **)&pLocator);
if (FAILED(hr)) goto cleanup;

bstrRoot = {{ WINAPI("SysAllocString") }}(L"ROOT\\CIMV2");
hr = pLocator->lpVtbl->ConnectServer(pLocator, bstrRoot,
    NULL, NULL, NULL, 0, NULL, NULL, &pServices);
if (FAILED(hr)) goto cleanup;

hr = {{ WINAPI("CoSetProxyBlanket") }}((IUnknown *)pServices,
    RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE, NULL,
    RPC_C_AUTHN_LEVEL_CALL, RPC_C_IMP_LEVEL_IMPERSONATE,
    NULL, EOAC_NONE);
if (FAILED(hr)) goto cleanup;

bstrWQL = {{ WINAPI("SysAllocString") }}(L"WQL");
bstrQuery = {{ WINAPI("SysAllocString") }}(L"SELECT * FROM Win32_ShadowCopy");

hr = pServices->lpVtbl->ExecQuery(pServices, bstrWQL, bstrQuery,
    WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
    NULL, &pEnum);
if (FAILED(hr)) goto cleanup;

while (pEnum->lpVtbl->Next(pEnum, WBEM_INFINITE, 1, &pObj, &uReturned) == S_OK) {
    VARIANT vPath;
    VariantInit(&vPath);
    hr = pObj->lpVtbl->Get(pObj, L"__RELPATH", 0, &vPath, NULL, NULL);
    if (SUCCEEDED(hr) && V_VT(&vPath) == VT_BSTR) {
        pServices->lpVtbl->DeleteInstance(pServices, V_BSTR(&vPath), 0, NULL, NULL);
    }
    VariantClear(&vPath);
    pObj->lpVtbl->Release(pObj);
    pObj = NULL;
}

cleanup:
if (bstrRoot) {{ WINAPI("SysFreeString") }}(bstrRoot);
if (bstrWQL) {{ WINAPI("SysFreeString") }}(bstrWQL);
if (bstrQuery) {{ WINAPI("SysFreeString") }}(bstrQuery);
if (pEnum) pEnum->lpVtbl->Release(pEnum);
if (pServices) pServices->lpVtbl->Release(pServices);
if (pLocator) pLocator->lpVtbl->Release(pLocator);
{{ WINAPI("CoUninitialize") }}();
