# 21-STM32F4_PRJ_GALVO_lib_v1.4 #
Project base: Keil uVision
<CODES>/sw4stm32_workbench_olds_and_others/uVision_mdk_28-12-2016_galvos/99-STM32F4xx_pid_lib_v1_4/

## Ac6 Plugin project creation settings
<kbd>New</kbd> > <kbd>C-Project</kbd> > <kbd>Ac6 STM32 MCU Project</kbd> > <kbd>Next</kbd> > <kbd>Next</kbd> > <kbd>Choose board</kbd> > <kbd>Standard Per... (StdPeriph)</kbd>  

- [ ] Extract all firmware in separate folder
- [X] Add low level drivers in the project
- [X] As sources in the application project
- [ ] As static external libraries

# Symbolic link #
The symbolic link inside the template folder (00-STM32F4_LIBRARY_MDB -> ../00-STM32F4_LIBRARY_MDB) is needed for compatibility with sublime-text and it's Open-Include plug-in 