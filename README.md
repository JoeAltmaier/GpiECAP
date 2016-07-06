Install Code Composer Studio

Install PRU sdk

http://www.ti.com/tool/pru-swpkg

Enter new include paths into the project

    "C:\ti\pru-software-support-package\include"
    "C:\ti\pru-software-support-package\include\am437x"

Build Release target

Copy from /{your workspace}/GpiEcap/Release/GpiECAP.out to /{sdcard}/rootfs/lib/firmware/am437x-pru1_0-fw

GpiECAP.out is an elf file, suitable for loading into the PRU.
