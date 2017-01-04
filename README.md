# sw4stm32_workbench #


## Setup "System Workbench ac6" ##

- Install the eclipse based "System Workbench ac6 for STM32"
- Invoke an initial start of sw4stm32_workbench
- To avoid eclipse C project setup error Target _firmware has not been found, please download it_
__C Project__
__Project Firmware configuration__
__Select the project structure and firmware__

```bash
0:mainster@XPS:/media/data/CODES/sw4stm32_workbench$ ll
total 422M
drwxr-xr-x  18 mainster 4,0K 2017-01-04 03:07 ./
drwxr--r-- 115 mainster  12K 2016-12-30 11:58 ../
drwxr-xr-x  10 mainster 4,0K 2017-01-04 01:30 00-STM32F429_LIBRARIES/                   # Complete lib of Tilen Majerle
drwxr-xr-x   2 mainster 4,0K 2017-01-04 02:44 00-STM32F429_LIBRARIES_MDB_MINIMAL/       # Minimal part of Tilen Majerles lib
drwxr-xr-x   4 mainster 4,0K 2016-12-30 19:01 00-STM32F4xx_STANDARD_PERIPHERAL_DRIVERS/ # StdPeriph_Lib included in Tilen Majerles lib 
drwxr-xr-x   9 mainster 4,0K 2017-01-04 02:56 02-STM32F429_MD_LED_BUTTON/
drwxr-xr-x   9 mainster 4,0K 2016-12-30 10:48 05-STM32F4_USART_PRINTF/
-rw-r--r--   1 mainster 311M 2016-12-30 10:48 stm32cube_fw_f4_v1110.zip             # Download 1. via IDE  
-rw-r--r--   1 mainster 111M 2016-12-30 10:48 stm32f4_dsp_stdperiph_lib_v161.zip    # Download 2. via IDE
drwxr-xr-x   8 mainster 4,0K 2016-12-30 11:45 STM32Cube_FW_F4_V1.11.0/              # Unzipped "Download 1."
drwxr-xr-x   6 mainster 4,0K 2016-12-30 12:00 STM32F4xx_DSP_StdPeriph_Lib_V1.6.1/   # Unzipped "Download 2."
drwxr-xr-x   6 mainster 4,0K 2016-12-30 16:54 stm32f429i-disc1_stdperiph_lib/       # ???
-rw-r--r--   1 mainster  880 2016-12-29 16:50 README.md
drwxr-xr-x   2 mainster 4,0K 2016-12-30 10:50 RemoteSystemsTempFiles/
drwxr-xr-x   4 mainster 4,0K 2017-01-04 00:18 .metadata/
drwxr-xr-x   5 mainster 4,0K 2016-12-30 11:38 .trash/
```

The StdPeriph_Lib components should be included via project settings / includes.
A good choice to include 00-STM32F429_LIBRARIES_MDB_MINIMAL or other library sources is to create a "resources link" inside the current project:

- Open context menu of actual project and
- select "Import" - "General" - "File System"
- select the resources directory (e.g. 00-STM32F429_LIBRARIES_MDB_MINIMAL)
- and check options "Create top-level folder" and "Create links in workspace"