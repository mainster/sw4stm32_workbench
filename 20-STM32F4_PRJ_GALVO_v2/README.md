# 01-STM32F4_MD_HELLO

# ARM math libraries #
Only some basic components of the CMSIS libraries are included when the Ac6 plugins Project wizard is used. If ARM librarie functions (e.g. arm_pid_init_f32(..)) are used, some additional configurations are necessarry to build the eclipse project.

```bash
cd <Project root>
cp -r STM32Cube_FW_F4_V1.11.0/Drivers/CMSIS/DSP_Lib/ \
        STM32Cube_FW_F4_V1.11.0/Drivers/CMSIS/Lib -t ./CMSIS 
cd CMSIS && rename 'y/[A-Z]/[a-z]/' * 
```
Under eclipse's Project Settings > C/C++ Build > Settings > MCU GCC Linker > Libraries:

- Libraries (-I):       __arm_cortexM4lf_math__
- search path (-L):     __"\${workspace_loc:/\${ProjName}/CMSIS/lib/GCC}"__

In main.c:
```C
#include "arm_math.h"
```
 
## Ac6 Plugin project creation settings
<kbd>New</kbd> > <kbd>C-Project</kbd> > <kbd>Ac6 STM32 MCU Project</kbd> > <kbd>Next</kbd> > <kbd>Next</kbd> > <kbd>Choose board</kbd> > <kbd>Standard Per... (StdPeriph)</kbd>  

- [ ] Extract all firmware in separate folder
- [X] Add low level drivers in the project
- [X] As sources in the application project
- [ ] As static external libraries

# Symbolic link #
The symbolic link inside the template folder (00-STM32F4_LIBRARY_MDB -> ../00-STM32F4_LIBRARY_MDB) is needed for compatibility with sublime-text and it's Open-Include plug-in 