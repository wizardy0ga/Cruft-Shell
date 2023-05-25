![How I Learned To Stop Worrying And Love The Malware](/img/strangelove.jpg)

<h1 align="center">Cruft-Shell</h1>

<p align=center>
Cruft Shell is a Windows user-land malware written in C. It is designed to offer basic persistence and remote command shell operations. Cruft-Shell is my first instance of a C program that extends beyond tutorial level code. Cruft-Shell gets its name from the code that is likely to be written by a C newbie...like me:)
</p>

## No Skids
Don't use or modify this code for illicit purposes. Really, just don't. 

## General Information
Cruft-Shell operates on a basic client server architecture over TCP with no bells and whistles. Cruft-Shell's purpose was allowing me to get hands on with the Windows API in C outside of a tutorial setting. This is a great program to compile (then disassemble) and examine under the microscope for the learning reverse engineer (me). Outside of this purpose, Cruft-Shell is not very useful in its current state. Communications are in plain-text and this code is likely not secure at all. If you couldn't already tell, this code was written for learning purposes. 

## Control Flow
![ControlFlow](/img/control-flow.png)

1. Cruft-Shell will check the image path that it's executing from against a predetermined path that it has been programmed to execute from.

2. If Cruft-Shell is not running from the predetermined path, Cruft-Shell will write the CNC network information to the registry, move itself to the predetermined execution path and then establish persistence via the users logon keys in the registry.

3. If Cruft-Shell is already running from the target execution path or if the infection cycle has finished, beacon operations will begin.

4. Threat actor catches shell and gains access to malware commands.

## Commands

| Command | Description |
|-|-|
| cmd.exe | Execute a remote command on the system |
| download | Download a file to the users temp directory |
| uninstall | Remove all registry key values |
| terminate | Terminate the Cruft-Shell process |
| reconnect | Destroy socket, get cnc network info and reconnect to server |
| change port | Change the port registry value |
| change host | Change the host registry value |

## Definitions
```
#define max_command_length        The maximum length of the command from the threat actor 
#define sleepTime                 The amount of time to sleep in between failed beacons

#define encPort                   Encoded port data to write in registry
#define encHost                   Encoded host data to write in registry
#define encHostKey                Encoded host registry key name
#define encPortKey                Encoded port registry key name
#define encPersistenceData        Encoded target execution path, will be moved here
#define encConnInfoKey            Encoded connection info registry key, host/port live here
#define encPersistenceKey         Encoded HKCU:\software\microsoft\windows\currentversion\run
#define encPersistenceValue       Encoded name for the persistence key

These definitions are arbitrary. They are used to identify the commands from the controller to the malware. 

#define comReconnect        
#define comUninstall        
#define comDownload         
#define comTerminate        
#define comResetPort        
#define comResetHost        
```

## Inner-workings

Cruft-Shell attempts to obfuscate the strings within the binary by using a caeser cipher to increment and decrement the ascii values of bytes representing the code. These strings will be encoded and decoded as needed. When the malware starts, it will first check the encPersistenceData value which is the filepath that the malware needs to execute from to indicate peristence. If it's not executing from this file path, the infection cycle will begin. The infection cycle consists of writing the CnC server network information to an arbitrary registry location, moving the file to the target file path and creating a registry value at HKCU:\Software\Microsoft\Windows\CurrentVersion\Run with the target file path as the data. After the infection cycle has finished or if the machine is already infected, the malware will retrieve the CnC networking information from the registry and begin beaconing the controlling server via a TCP socket. When the malware begins it's main loop for getting user commands, the loop will loop will look for a defined command and if none is found, execute the command as a shell command.

As a side-note, this code does not provide any error checking. The API calls are made but not checked for a success status. The functions also don't check for errors. 

## Usage

No server-side code has been provided with Cruft-Shell. Cruft-Shell can be used with netcat as the server-side code where the commands that the user defines are what needs to be sent from the netcat listener to execute the corresponding function on the infected system. It is not my intention to provide code that has real-world usability in this repository. This code was simply a fun way to experiment with and learn some of the Windows API in C.

Overall, experimentation with Cruft-Shell will require source code examination and slight modification. 

## Utilities

| Utility | Description |
|-|-|
| cleanup.ps1 | Remove the registry changes and target file from the host system |
| encode_util.exe | Encode a string with the caeser cipher |
| decode_util.exe | Decode a string with the caesar cipher |

## Mitre ATT&CK TTP's

| Tactic| Technique | ID | Relation to Cruft-Shell |
|-|-|-|-|
| [Defense Evasion](https://attack.mitre.org/tactics/TA0005/)| [Modify Registry](https://attack.mitre.org/techniques/T1112/) | T1112 | Cruft-Shell will store and retrieve the CnC networking information in the registry |
| [Execution](https://attack.mitre.org/tactics/TA0002) | [Command and Scripting Interpreter: Windows Command Shell](https://attack.mitre.org/techniques/T1059/003/) | T1059.003 | Cruft-Shell defaults to a _popen call if no valid command is sent from the contoller |
| [Persistence](https://attack.mitre.org/tactics/TA0003) | [Boot or Logon Autostart Execution: Registry Run Keys / Startup Folder](https://attack.mitre.org/techniques/T1547/001/) | T1547.001 | Cruft-Shell will write its target execution file path to HKCU:\Software\Microsoft\Windows\CurrentVersion\Run for persistence |
| [Command and Control](https://attack.mitre.org/tactics/TA0011) | [Non-Application Layer Protocol](https://attack.mitre.org/techniques/T1095/) | T1095 | Cruft-Shell uses an IPv4 TCP socket as a command and control channel |
| [Command and Control](https://attack.mitre.org/tactics/TA0011) | [Ingress Tool Transfer](https://attack.mitre.org/techniques/T1105/) | T1105 |Cruft-Shell has the ability to download files on the target system via URLDownloadToFile Win32 API Call |
| [Defense Evasion](https://attack.mitre.org/tactics/TA0005/) | [Masquerading: Match Legitimate Name or Location](https://attack.mitre.org/techniques/T1036/005/) | T1036.005 | Cruft-Shell attempts to hide itself within the file system by moving to a target file path that blends in with the environment |

## Tested On

Windows 10 Pro 22H2 Build 19045

