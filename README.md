Install Code Composer Studio

Install PRU sdk

http://www.ti.com/tool/pru-swpkg

Enter new include paths into the project

    "C:\ti\pru-software-support-package\include"
    "C:\ti\pru-software-support-package\include\am437x"

Check out this project into your workspace

Build the Release target

Copy from /{your workspace}/PRU_GpiEcap/Release/GpiECAP.out to /{sdcard}/rootfs/lib/firmware/am437x-pru1_0-fw

GpiECAP.out is an elf file, suitable for loading into the PRU. Its called 'am437x-pru1_0-fw' for the pru loader (no elf extension).