HANDLE hEngine = NULL;
FWP_BYTE_BLOB *pAppId = NULL;
DWORD result;
UINT64 filterId = 0;

result = {{ WINAPI("FwpmEngineOpen0") }}(NULL, RPC_C_AUTHN_WINNT, NULL, NULL, &hEngine);
if (result != ERROR_SUCCESS) return;

result = {{ WINAPI("FwpmGetAppIdFromFileName0") }}(L"{{ target_exe }}", &pAppId);
if (result != ERROR_SUCCESS) goto cleanup;

FWPM_FILTER_CONDITION0 cond;
ZeroMemory(&cond, sizeof(cond));
cond.fieldKey = FWPM_CONDITION_ALE_APP_ID;
cond.matchType = FWP_MATCH_EQUAL;
cond.conditionValue.type = FWP_BYTE_BLOB_TYPE;
cond.conditionValue.byteBlob = pAppId;

FWPM_FILTER0 filter;
ZeroMemory(&filter, sizeof(filter));
filter.displayData.name = L"{{ filter_name }}";
filter.layerKey = FWPM_LAYER_ALE_AUTH_CONNECT_V4;
filter.action.type = FWP_ACTION_BLOCK;
filter.weight.type = FWP_UINT8;
filter.weight.uint8 = 15;
filter.numFilterConditions = 1;
filter.filterCondition = &cond;

result = {{ WINAPI("FwpmFilterAdd0") }}(hEngine, &filter, NULL, &filterId);

cleanup:
if (pAppId) {{ WINAPI("FwpmFreeMemory0") }}((void **)&pAppId);
if (hEngine) {{ WINAPI("FwpmEngineClose0") }}(hEngine);
