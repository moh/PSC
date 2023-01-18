cd /d "%~dp0"

set OSUTILS_PATH=%~d0\Personal data\Software\Other\Software development\C++\Robotics\Current\OSUtils

recycle -f .\OSComputerRS232Port.h
copy /B /Y /Z "%OSUTILS_PATH%\OSComputerRS232Port.h" .\OSComputerRS232Port.h
recycle -f .\OSCore.c
copy /B /Y /Z "%OSUTILS_PATH%\OSCore.c" .\OSCore.c
recycle -f .\OSCore.h
copy /B /Y /Z "%OSUTILS_PATH%\OSCore.h" .\OSCore.h
recycle -f .\OSCriticalSection.c
copy /B /Y /Z "%OSUTILS_PATH%\OSCriticalSection.c" .\OSCriticalSection.c
recycle -f .\OSCriticalSection.h
copy /B /Y /Z "%OSUTILS_PATH%\OSCriticalSection.h" .\OSCriticalSection.h
recycle -f .\OSMisc.c
copy /B /Y /Z "%OSUTILS_PATH%\OSMisc.c" .\OSMisc.c
recycle -f .\OSMisc.h
copy /B /Y /Z "%OSUTILS_PATH%\OSMisc.h" .\OSMisc.h
recycle -f .\OSNet.c
copy /B /Y /Z "%OSUTILS_PATH%\OSNet.c" .\OSNet.c
recycle -f .\OSNet.h
copy /B /Y /Z "%OSUTILS_PATH%\OSNet.h" .\OSNet.h
recycle -f .\OSThread.c
copy /B /Y /Z "%OSUTILS_PATH%\OSThread.c" .\OSThread.c
recycle -f .\OSThread.h
copy /B /Y /Z "%OSUTILS_PATH%\OSThread.h" .\OSThread.h
recycle -f .\OSTime.c
copy /B /Y /Z "%OSUTILS_PATH%\OSTime.c" .\OSTime.c
recycle -f .\OSTime.h
copy /B /Y /Z "%OSUTILS_PATH%\OSTime.h" .\OSTime.h

pause
