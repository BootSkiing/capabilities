HRESULT hr;
INetFwPolicy2 *pPolicy = NULL;

hr = {{ WINAPI("CoInitializeEx") }}(NULL, COINIT_MULTITHREADED);
if (FAILED(hr)) return;

hr = {{ WINAPI("CoCreateInstance") }}(&CLSID_NetFwPolicy2, NULL,
    CLSCTX_INPROC_SERVER, &IID_INetFwPolicy2, (void **)&pPolicy);
if (FAILED(hr)) goto cleanup;

hr = pPolicy->lpVtbl->put_FirewallEnabled(pPolicy, NET_FW_PROFILE2_DOMAIN, VARIANT_FALSE);
if (FAILED(hr)) goto cleanup;

hr = pPolicy->lpVtbl->put_FirewallEnabled(pPolicy, NET_FW_PROFILE2_PRIVATE, VARIANT_FALSE);
if (FAILED(hr)) goto cleanup;

hr = pPolicy->lpVtbl->put_FirewallEnabled(pPolicy, NET_FW_PROFILE2_PUBLIC, VARIANT_FALSE);

cleanup:
if (pPolicy) pPolicy->lpVtbl->Release(pPolicy);
{{ WINAPI("CoUninitialize") }}();
