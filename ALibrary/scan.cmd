@echo off

scan_ASW.py ABase %1 %2 %3 %4 %5
scan_ASW.py UnitTests %1 %2 %3 %4 %5

scan_ASW.py ../ALibraryImpl/ACrypto %1 %2 %3 %4 %5
scan_ASW.py ../ALibraryImpl/ADatabase_MySQL %1 %2 %3 %4 %5
scan_ASW.py ../ALibraryImpl/ADatabase_ODBC %1 %2 %3 %4 %5
scan_ASW.py ../ALibraryImpl/ADatabase_SQLite %1 %2 %3 %4 %5
scan_ASW.py ../ALibraryImpl/AGdLib %1 %2 %3 %4 %5
scan_ASW.py ../ALibraryImpl/APythonEmbed %1 %2 %3 %4 %5
scan_ASW.py ../ALibraryImpl/AXsl %1 %2 %3 %4 %5
scan_ASW.py ../ALibraryImpl/AZlib %1 %2 %3 %4 %5
scan_ASW.py ../ALibraryImpl/ConsoleImpl %1 %2 %3 %4 %5
