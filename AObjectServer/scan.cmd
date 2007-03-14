@echo off
..\ALibrary\scan_ASW.py AObjectServer %1 %2 %3 %4 %5
..\ALibrary\scan_ASW.py AOSTest %1 %2 %3 %4 %5
..\ALibrary\scan_ASW.py AOSWatchDog %1 %2 %3 %4 %5
..\ALibrary\scan_ASW.py AOS_Base %1 %2 %3 %4 %5
..\ALibrary\scan_ASW.py AOS_BaseModules %1 %2 %3 %4 %5

..\ALibrary\scan_ASW.py ..\AOS_Modules\AOS_Blog %1 %2 %3 %4 %5
..\ALibrary\scan_ASW.py ..\AOS_Modules\AOS_DadaData %1 %2 %3 %4 %5
..\ALibrary\scan_ASW.py ..\AOS_Modules\AOS_Example %1 %2 %3 %4 %5
..\ALibrary\scan_ASW.py ..\AOS_Modules\AOS_Forum %1 %2 %3 %4 %5
..\ALibrary\scan_ASW.py ..\AOS_Modules\AOS_Security %1 %2 %3 %4 %5
..\ALibrary\scan_ASW.py ..\AOS_Modules\AOS_Test %1 %2 %3 %4 %5
