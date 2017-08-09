::========================Mediasender-win32-version===================
::========================visual studio 2010 version=====================
@echo on
set son=mediasender

if not exist "%son%" (
    mkdir %son%
)

for /f %%i in ('dir/b') do (
    if not "compile.bat"=="%%i" (
        move %%i %son%
    )
)

cd %son%

echo "===========copy lib start=============="

copy ..\..\..\..\..\10-common\lib\debug\win32_2010\KdvMediaNetDll.dll .\  /y
copy ..\..\..\..\..\10-common\lib\debug\win32_2010\libkdvsrtp.dll .\  /y
copy ..\..\..\..\..\10-common\lib\debug\win32_2010\libkdcrypto-1_1.dll .\  /y

echo "===========copy lib finish=============="

echo "===========build MediaSender vs2010 start=============="

call "%VS100COMNTOOLS%\vsvars32.bat"
devenv /Rebuild debug "senderTest.vcxproj" /Out senderTest_d.txt
devenv /Rebuild release "senderTest.vcxproj" /Out senderTest_r.txt

echo "===========build MediaSender vs2010 finish=============="

echo "===========copy lib start=============="

copy ..\..\..\..\..\10-common\lib\debug\win32_2010\KdvMediaNetDll.dll .\Debug  /y
copy ..\..\..\..\..\10-common\lib\debug\win32_2010\libkdvsrtp.dll .\Debug  /y
copy ..\..\..\..\..\10-common\lib\debug\win32_2010\libkdcrypto-1_1.dll .\Debug  /y

echo "===========copy lib finish=============="


@exit
