# The malware sitting in its target execution path
Remove-Item -Path ""

# The name given to the value of the peristence registry key
Remove-ItemProperty -Path "HKCU:\software\microsoft\windows\currentversion\run" -Name ""

# Remove the host and port values from from the registry keys that were chosen
# to hold the connection information. 
Remove-ItemProperty -Path "" -Name ""
Remove-ItemProperty -Path "" -Name ""