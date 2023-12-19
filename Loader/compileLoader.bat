@ECHO off

for %%f in (*.S) do (
powerpc-eabi-as -mregnames %%f -o %%~nf.o
powerpc-eabi-ld -Ttext 0x800046F0 --oformat binary %%~nf.o -o %%~nf.bin
:: Add "REM " (yes, that space needs to be there) to the beginning of the below line to keep the Loader.o file
del /f %%~nf.o

:: Completion message to make it clear that the loader compiled
echo Successfully compiled the loader file.
echo Output file is named "%%~nf.bin"
echo.
)

pause