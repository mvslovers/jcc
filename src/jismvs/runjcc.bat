c:\jcc\jcc %3 %4 -r -z20000 -Ic:\jcc\include %1 %2
@if ERRORLEVEL 1 goto wait
@goto exit
:wait
pause
:exit
