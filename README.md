# Shieldbuddy + FreeEntryToolchain + ILLD

This repo contains projects to program a **Shieldbuddy (Infineon AURIX Tricore TC275)**.
Each project uses the **Hightec Free Entry Tricore Toolchain** and the **Infineon Low Level Drivers (ILLD)**.
The following sections will explain how to obtain and install the Hightec Free Entry Toolchain
and how this repo is structured. The instructions are only valid for a Windows machine.


## Structure of this repo - overview

```
Shieldbuddy-FreeEntryToolchain-ILLD
    AurixCodeExamples/PROJ_NAME_1a
    AurixCodeExamples/PROJ_NAME_2a
    ...
    BifacesBaseFramework/PROJ_NAME_1b
    BifacesBaseFramework/PROJ_NAME_2b
    ...
```


## Structure of this repo

This repo contains branches named `AurixCodeExamples/PROJ_NAME` and branches named `BifacesBaseFramework/PROJ_NAME`.
These two names for branches are used, as we show how to set up 2 kind of projects to be used as starting point to program the Shieldbuddy with.
We will always use the freely available `Free Entry Toolchain` and the `Hightec IDE` which is part of the it,
and also Infineon's `ILLD` library which is freely available as part of the open source [aurix-code-examples](https://github.com/Infineon/AURIX_code_examples/tree/master/code_examples).

1) is based on the open source [aurix-code-examples](https://github.com/Infineon/AURIX_code_examples/tree/master/code_examples)
    - branches `AurixCodeExamples/PROJ_NAME`
    - `Shieldbuddy (TC275) + Free Entry Toolchain + Hightec IDE + Aurix Code Example`
    - to see how the project was created, see [here-1](#projects-1-aurix-code-examples)
        - see `README-aurix-code-examples.md`
2) is based on the Bifaces BaseFramework
    - branches `BifacesBaseFramework/PROJ_NAME`
    - `Shieldbuddy (TC275) + Free Entry Toolchain + Hightec IDE + Bifaces BaseFramework`
    - to see how the project was created, see [here-2](#projects-2-bifaces-baseframework)
        - see `README-bifaces-baseframework.md`


## How to set up FreeEntryToolchain for Shieldbuddy

To check the MAC-address of the LAN-/Wifi-Adapter, use the commands `ipconfig /all` and `getmac /v`.
Another way to check the MAC-addresses is by `Control Panel -> Network and Internet -> Network Connections -> right-click on adapter -> Status -> Details... -> Physical Address`.

Go to `https://free-entry-toolchain.hightec-rt.com/`, fill out the form on the right and click `Generate License File & Download` to download the toolchain and the generated license file.

Follow the install instructions shown on the website:
- unpack
- run `Setup.exe`
- copy license file to `C:\Hightec\licenses`
- check if environment variable `RLM_LICENSE` is set to `C:\HighTec\licenses` and create it in case it doesn't exist
- check if `C:\HighTec\toolchains\tricore\v4.9.3.0-infineon-1.0\bin` was added to `PATH`. if not, do so. confirm by checking `tricore-gcc --version`.


## Projects (1): Aurix Code Examples

The following shows how to program and debug the Shieldbuddy (TC275)
using the freely available `Hightec Free Entry Tricore Toolchain` + the open source [aurix-code-examples](https://github.com/Infineon/AURIX_code_examples/tree/master/code_examples/) provided by Infineon.

This project will be a branch following the naming convention `AurixCodeExamples/PROJ_NAME`.

To add a project from [aurix-code-examples](https://github.com/Infineon/AURIX_code_examples/tree/master/code_examples), .e.g. [Blinky](https://github.com/Infineon/AURIX_code_examples/tree/master/code_examples/Blinky_LED_1_KIT_TC275_SB) in Hightec IDE, follow these steps:

0. start Hightec IDE and select a workspace
1. create a new empty project using `File -> New -> HighTec Project -> Create empty project -> ...`
2. copy files/folders from the github project-folder into the Hightec project-folder, except

   - `.ads/`, `.exportedSettings`, `*.lsl`
   
   i.e. only copy the following:
    
    - `Configurations/`, `Libraries/`, `Cpu*_Main.c`, `*.c`, `*.h`

3. add include paths (follow either one of the 2 following suggestions below):
    1. make list of include paths, e.g. named `illd_includes.opt`, and add it to compiler- and assembler-includes:
        - add `absolute_path_to_file\illd_includes.opt` in
            - `Properties -> C/C++ Build -> Settings -> TriCore C Compiler -> General -> Option file` 
            - `Properties -> C/C++ Build -> Settings -> TriCore Assembler -> General -> Option file`
    2. in `Properties -> C/C++ Build -> Settings -> Tool Settings -> TriCore C Compiler -> General`
        - Add... -> Workspace... -> expand all by `SHIFT + NUMPAD::*` -> `STRG + A` -> OK
4. copy desired linker-script into project-folder: download this [linker-file](https://github.com/ProtossDragoon/TC275D-high-level-library/blob/master/1_ToolEnv/0_Build/1_Config/Config_Tricore_Gnuc/Lcf_Gnuc_Tricore_Tc.lsl), for instance, and copy it in the project-folder
5. change settings as desired, e.g.:
    - `Properties -> C/C++ Build -> Settings -> Tool Settings -> TriCore C Compiler -> Global options`
        - CPU type: TC27xx
    - `Properties -> C/C++ Build -> Settings -> Tool Settings -> TriCore C Compiler -> Language dependent`
        - C Standard: C 99
    - `Properties -> C/C++ Build -> Settings -> Tool Settings -> TriCore C Linker -> General -> Browse Linker Description File`
        - add path of previously added linker-file here
    - ...
6. optionally: add pre-build/post-build steps in `Properties -> C/C++ Build -> Advanced Build Steps -> Post-Build Steps -> Library`
7. add debug configuration (new UDE configuration) for debugging and flashing
    - create new debug configuration `Debug symbol -> Debug Configurations... -> right click Universal Debug Engine -> New Configuration`, then add a new target configuration file with `UDE Startup -> Create Configuration -> Use a default target configuration -> TriCore -> Hitex -> Shieldbuddy -> Hitex Shieldbuddy with TC275T D-Step (Multicore Configuration) -> Finish -> Save file (e.g. in project folder)`
    - to start flashing/debugging, click Debug. to then flash press `Program all`, wait for completion and press `Exit`. you can now debug/run the program.
    - another way to flash the Shieldbuddy once the program was compiled and the hex-file was generated, is using the free [Infineon MemTool](https://www.infineon.com/cms/en/tools/aurix-tools/free-tools/infineon/)

Now everything should be set up to build, flash and debug.


## Projects (2): Bifaces BaseFramework

The following shows how to program and debug the Shieldbuddy (TC275)
using the freely available `Hightec Free Entry Tricore Toolchain` + a Bifaces BaseFramework project.

This project will be a branch following the naming convention `BifacesBaseFramework/PROJ_NAME`.

1. create new empty project using `File -> New -> HighTec Project -> Create empty project -> ...`
2. optionally: create new build configuration `iROM` and set it as active
3. create new folder `src` in the project-folder
4. copy `AppSw` and `BaseSw` from the Bifaces BaseFramework into `src/`
5. copy desired linker-script into project-folder
6. add include paths (follow either one of the 2 following suggestions below):
    1. copy the `Tricore_IncludePathList.opt` ILLD header files search list into project-folder
    2. add include paths in Properties as described in (7) under `Properties -> C/C++ Build -> Settings -> Tool Settings -> Tricore C Compiler -> General -> Add... -> Workspace... -> add all under src/`
7. check settings in
    - `Properties -> C/C++ Build -> Settings -> Tool Settings -> Global options`
            CPU type: TC27xx
    - `Properties -> C/C++ Build -> Settings -> Tool Settings -> Tricore C Compiler`
        - `-> General -> Add... -> Workspace... -> add all under src/`
        - `-> Optimization: Speed`
        - `-> Language dependent -> C 99`
        - `-> Miscellaneous: add '-fstrict-volatile-bitfields'`
        - `-> Code Generation -> enable '-fdata-sections', '-ffunction-settings'`
    - `Properties -> C/C++ Build -> Settings -> Tool Settings -> Tricore C Linker`
        - `-> General -> enable '-nostartfiles'`
        - `-> General -> Browse Linker Description File: "${ProjDirPath}/Lcf_Gnuc_Tricore_Tc.x"`
8. optionally: add pre-build/post-build steps in `Properties -> C/C++ Build -> Advanced Build Steps -> Post-Build Steps -> Library`
9. add debug configuration (new UDE configuration) for debugging and flashing
    - see [here-1](#projects-1-aurix-code-examples) under `7.`

Now everything should be set up to build, flash and debug.


## Tested code

The following code was tested on a Shieldbuddy with Tricore processor `SAK-TC275TP-64 F200N DC`:
- AurixCodeExamples/BaseProj
- BifacesBaseFramework/BaseProj


# Notes:
- license-related settings in Hightec IDE can be found via `Window -> Preferences -> HighTec Preferences -> License Manager`
- download illd-user-manual from `https://www.infineon.com/aurix-expert-training/TC27D_iLLD_UM_1_0_1_12_0.chm`. this link is in `https://github.com/Infineon/AURIX_code_examples/blob/master/code_examples/Blinky_LED_1_KIT_TC275_SB/Cpu0_Main.c`.
- this [migration guide](https://free-entry-toolchain.hightec-rt.com/HighTec_Free_TriCore_Entry_Toolchain_Migration_Guide.pdf) shows how to migrate Bifaces ILLD demo projects into HighTec projects
- predefined post-build steps in `C/C++ Build -> Advanced Build Steps -> Post-Build Steps -> Library`

    ```bash
    tricore-objcopy.exe -O ihex ELF_FILE HEX_FILE > hex.txt      # generate hex-file
    tricore-readelf.exe -S ELF_FILE > section_headers.txt        # generate section headers
    tricore-size.exe -A ELF_FILE > size.txt                      # generate size summary
    tricore-size.objdump -d ELF_FILE > disassemble.txt           # generate assembly code
    ```

- build-log is in `<hightec_workspace>\.metadata\.plugins\org.eclipse.cdt.ui` (set in `C/C++ Build -> Logging`)
- check used illd-version
    - in Aurix code-examples: `Libraries/iLLD/TC27D/Tricore/IfxLldVersion.h`
    - in Bifaces BaseFramework: `src/BaseSw/iLLD/TC27D/Tricore/IfxLldVersion.h`
- sth