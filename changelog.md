## grblHAL changelog

Build 20210726:

Core:
* Added encapsulation in stream handlers for realtime processing. This allows more flexible use of additional streams.
* Added optional debug stream to the HAL. Currently only used by the iMXRT1062 driver.
* Position will now be recalulated on steps/mm setting changes.
* Limited final `$TPW` retract on tool changes to Z home to avoid triggering Z-limit.
* Moved some common driver code snippets to the core to avoid duplication.

Drivers & plugins:
* Updated all for new stream encapsulation.
* Added option to use UART input for I2C keypad plugin. Added fan toggle support.
* Fixed some bugs and regressions.
* Added support for ganged/autosquared axes to RP2040 driver.

Build 20210707:

Core:
* Added realtime command `0x8A` for toggling fan 0 on/off. Requires the Fans plugin.
* Moved a bit of code to aid documentation structure.
* Added option for plugins to add pin description for claimed pins to `$pins` report.
* Simplified API for user added M-codes, backwards compatibility retained for now.

Drivers:
* Updated a number of drivers for new style pin handling (allows `$pins` report and simpler pin mapping files).
* Updated and verified driver for RP2040, mainly to support the PicoCNC board.
* Added/improved interrupt support for auxillary inputs for both plugin code (via API) and for `M66`.
* Bug fixes.

Build 20210626:
* Standardized handling of motors for ABC- and ganged/squared axes, configuration moved to _my_machine.h_.  
First number of motors required is calculated, then ABC axes are added from bottom up and then ganged/squared axes from top down.  
E.g if the board map supports six motors then the following allocations will be made:  
A-axis and auto-squared Y axis: A-axis -> motor 4 and second Y-axis -> motor 5. Motor 6 pins may then be assigned to auxillary I/O.  
A-axis, B-axis and ganged X-axis: A-axis -> motor 4 and, B-axis -> motor 5 and second X-axis -> motor 6. Motor 6 limit pin\(s\) if available may be assigned to auxillary I/O.  
Auto-squared X and Y-axis: second X-axis -> motor 4 and second Y-axis -> motor 5. Motor 6 pins may then be assigned to auxillary I/O.  
etc...  
__IMPORTANT:__ For those who have used auto-squared/ganged axes with previous builds be sure to check that the motors allocated matches the current wiring.
Tip: use the `$pins` system command to list the pin allocations when checking. Rewire as neccesary. 
* Added ganged axis/auto-squaring support to some board maps for the [LPC176x](https://github.com/grblHAL/LPC176x) driver.
* Expanded on HAL entry points for stream communication and added [initial documentation](http://svn.io-engineering.com/grblHAL/html/hal_8h.html) for the HAL and parts of the core.
* Encapsulated UART/USB CDC code for many drivers, for most only the init function is now available for direct acccess from the outside. Simplified main driver code and plugins using streams.  
__NOTE:__ For driver developers: `hal.stream.get_rx_buffer_available` has been renamed to `hal.stream.get_rx_buffer_free` as it was ambiguous \(free space vs. available characters\).
* Added preview version of [Bluetooth plugin](https://github.com/grblHAL/Plugins_Bluetooth/) - allows auto configuration and auto stream switching for drivers/boards that supports it.
* Added number of auxillary I/O ports available to `$I` command response.
* Added value read from last `M66` to the first real-time report following the read. The element `|In:<value>` is used for the report. If the value reported is `-1` the read failed.

Build 20210608:
* Fixes for `$70` setting handling, networking services. Only compile time enabled services \(or protocols\) can now be configured.

Build 20210604:
* Added some HAL entry points and properties, shared file for mapped step and dir output.
* Added `$pins` system command, for listing current pin assignments. Work in progress, only supported by a couple of drivers. For now only plain GPIO pins are listed.
* Some minor bugs fixed. 

Build 20210515:

* Fixed G43 "bug" by changing to LinuxCNC specification. NIST specification is ambiguous.
* Added acceleration override handling, to be used by OpenPNP plugin M-code.

Build 20210505:

* Some bug fixes: relative canned cycles repeat error, comment handling...
* Check mode changes to allow use with SD card streaming.
* Internal buffer changes for reducing RAM footprint. Some HAL extensions.

Build 20210314:

* Added support for tool change protocol to networking protocols using shared stream buffer.
* Added `$S` system command for toggling single step mode, when enabled cycle start has to be issued after each block.
* Some bug fixes.

Build 20210207:

* Added `#define BOARD_MY_MACHINE` option in _my_machine.h_ for building using _my_machine-map.c_, this for simpler handling of user defined pin mappings.  
_my_machine-map.c_ is __*not*__ part of the distributed source and must by added to the project by the user before enabled, typically by copying an existing map file.
* Added HAL layer on top of Trinamic driver low-level code. Unified Triniamic plugins into single plugin.  
This is still work in progress, testers wanted.
* Added core support for up to four limit switches per axis.  
Added `$LEV` command for outputting report containing which limit or control switch(es) caused the last event.  
Report format:  
`[LASTEVENTS:<control signals>,<min>,<max>,<min2>,<max2)]`  
Where `<control signals>` field may contain controls signal letters (`H`, `S` etc.) and the rest axis letters for the corresponding limit switches inputs.
* More settings susbsystem changes and refactored $-system commands parser.  
There are some API changes related to this that may affect user defined plugins.
* Added $7 setting for option "Spindle off with zero speed".
* Added `|FW:grblHAL` element to full real-time report requested by sending `0x87`. 
* Simplified settings handling.
* No longer sends any messages to networking streams on connect.
* Added [driver](drivers/STM32F3xx) for STM32F303 based Blackpills.
* Updated support for Trinamic TMC5160 drivers, currently for the SKR 1.x boards \(LPC176x driver\) - testing i progress.
* Initial support for Trinamic TMC5160 drivers added, currently for the SKR 1.x boards \(LPC176x driver\) - not yet tested.
* Added handling for motor fault signal to the core, similar to E-stop handling. Added new alarm and error code for this.
* Changed to clear homed status on a soft reset only if machine was in motion. Added setting flag for always keeping homed status on soft reset to `$22` setting.  
__NOTE:__ This change is experimental and might be changed or reverted. Please report any problems related to this.
* Updated gcode parser \(grbl/gcode.c\) to use bitfields structs instead of bitfield variables.  
Done to improve readability and for easier debugging.  
Note that this is a major change and there is a non-zero risk that mistakes has been made.
* Changed signature of user mcode validation function to use a bitfield union for value words available.  
Removed the need for user mcode parameter words to have an associated value. This means that [user mcode](https://github.com/terjeio/grblHAL/tree/test/templates) implementations now must check this locally.  
If no associated value is provided the corresponding value in the value struct is set to `NAN` (Not A Number) for floats and all bits set to 1 for integers.
* Refactored Trinamic driver code, added initial support for TMC2209. Work in progress.
* Removed the need to copy the core grbl and plugin code to the driver chosen, this is now kept in sync with the master Subversion repository automatically.
* Added initial support for RADDS 1.6 board to SAM3X8E driver \(Arduino Due\). Untested!
* Added C-axis support to iMXRT1062 driver \(Teensy 4.x\).  
Untested and none of the current board maps has the needed pins defined.
* Added alarm and error message for power on self-test \(POS\) failure.  
If POS fails only $-commands are accepted.
* Work in good progress for Trinamic TMC2209 driver support \(UART mode\).  
Processor/board specific driver code has to be added for this, currently testing with STM32F446 and Nucleo-64 breakout board.
* Renumbered setting groups for more logical sorting (by id).
* Harmonized probing code across drivers for planned future extensions.
* Added additional I/O support for the [Teensy 4.1 T41U5XBB board](https://github.com/phil-barrett/grbl-teensy-4) \(iMXRT1062 driver\), 3 outputs and 4 inputs available via `M62` - `M66`.  
Not that the result from reading inputs with `M66` cannot be used in a gcode program in any meaningful way.
* Fixed excessive step pulse jitter in STM32F4xx driver.
* Added [more options](https://github.com/terjeio/grblHAL/wiki/Report-extensions#controller-information-extensions) to the `NEWOPT` tag in the extended `$I` report.
* Error 7 is no longer issued on startup if non-volatile storage \(Flash/EEPROM/FRAM\) for settings is not available.
* [Alarm substate](https://github.com/terjeio/grblHAL/wiki/Report-extensions#realtime-report) \(if available\) is always added to the real-time report if a [complete report](https://github.com/terjeio/grblHAL/wiki/For-sender-developers#single-character-real-time-commands) is requested by sending `0x87`.
* Added input signal and handling for limit switches override.  
The pin is pulled high and requires a normally open \(NO\) push switch for operation. When closed limit pins are excluded from the status report and alarm 12 will not be raised if a limit switch is asserted _on a soft reset_ when "Hard limits" and "Strict mode" is enabled with `$21`.
This allows normal operation so that a manual pull-off can be done before e.g. homing the machine.  
Currently only the iMXRT1062 \(Teensy 4.x\) driver has support for this, for now by reassigning the safety door input when this is not used for its intended purpose.  
__NOTE:__ A override will _not_ affect handling of homing and limit switch events elsewhere.
* Now adds `ODO` to `NEWOPT` tag values if odometer data is available.
* Updated _[my_plugin.c](templates/my_plugin.c)_ [template](templates/README.md) with settings details for `$HELP` and `$ES`/`$EG` enumerations.
* Settings/setting groups handling enhanced, moved some to plugins and added sorting (requres enough heap).
* Removed external dependecies by adding driver source/USB blob to LPC176x driver.
* Enhanced and improved ModBus support code for VFD spindle, added settings for baud rate and receive timeout.
* Added support for enumeration of and help for driver and plugin provided settings and setting groups.
* Moved board selection etc. to [CMakeLists.txt](drivers/ESP32/CMakeLists.txt) for [ESP32 driver](drivers/ESP32/README.md) for simpler configuration.
* Fixed regression for VFD spindle code, should now be able to run tests.
* Added build configurations for processor variants and Nucleo-64 boards for the [STM32F4xx driver](drivers/STM32F4xx/README.md).
* Added initial board map file for [BTT SKR 1.4 Turbo board](https://www.bigtree-tech.com/products/btt-skr-v1-4-skr-v1-4-turbo-32-bit-control-board.html) to the [LPC176x driver](drivers/LPC1769/README.md) including build configuration for bootloader compatible executable.
* Added polling of limit switches to the LPC176x driver, enabling hard limits is now possible.
* Added `$` commands for getting details about alarm codes, error codes, settings and settings groups.  
`$EA` - enumerate alarm codes.  
`$EE` - enumerate error codes.  
`$ES` - enumerate settings.  
`$EG` - enumerate setting groups.  
`$E*` - enumerate all above.  
The output from these is intended for sender developers and can be used instead of loading this information from .csv files.  
The settings enumeration contains additional information such as group assignment, datatype and format, value list for bitfields, min allowed value and max allowed value. 
For now descriptions of the settings are not included.  
__NOTE:__ This is a preview version, format and group codes may change for settings and setting groups.
* Added `$HELP` command and `$$<n>` command for listing information about a specific setting.  
`$HELP` on its own prints arguments that can be used with `$HELP`.  
`$HELP Commands` - print `$` commands available with a short description.  
`$HELP Settings` - print information about all available settings.  
`$HELP <argument>` print information about settings from the setting group provided in `<argument>`. E.g. `$HELP Spindle` will print information about spindle settings.  
__NOTE:__ do _NOT_ issue these commands from a sender MDI as the output may crash it, output is in plaintext and thus intended for use from a terminal only.
__NOTE:__ Settings data format has been changed and settings will be reset to default on update. Backup and restore.
* Moved `#define` values to settings for auto square failure distances:  
`$347` - default value from `DUAL_AXIS_HOMING_FAIL_AXIS_LENGTH_PERCENT` \(5%\) in grbl/config.h.  
`$348` - default value from `DUAL_AXIS_HOMING_FAIL_DISTANCE_MIN` \(2.5mm\) in grbl/config.h.  
`$349` - default value from `DUAL_AXIS_HOMING_FAIL_DISTANCE_MAX` \(25mm\) in grbl/config.h.
* Added settings and functionality for moving the second axis up to &plusmn;2mm after successful auto squaring to compensate for any switch alignment error.  
The settings is per axis, `$17n` where `n` is the axis index: `n` = `0` -> X axis, `1` -> Y axis, ...  
Note that settings values will only be reported for axes with dual motors installed and configured for auto squaring. 
* Added setting `$345` for pull-off rate from tool length sensor used for tool change. Default value is 100mm/min.
* Added setting flag to `$10` for enabling override of _Homing on startup required_ \(if enabled\) by a soft reset.
* Blocked loophole where machine could be unlocked by issuing a single axis homing command when _Homing on startup required_ is enabled.  
Alarm 11 will now be reissued until all axes configured for homing are homed.
* Added software debounce for the safety door switch to STM32 drivers.
* "Hardened" parking functionality. It should now tolerate a bouncy door switch and multiple closing/reopenings of the door during retract/restore.  
__NOTE:__ Not extensively tested. Use with care!
* Added `$I+` system command. This can be used when [compatibility level](https://github.com/terjeio/grblHAL/wiki/Compatibility-level) is > 0 to get the extended version including the current compatibility setting.
* Added basic support for separating limit switches from homing switches in the core. If a driver does not handle separate inputs for these the core "connects" the homing switches to the limit switches in code.
* Improved auto squaring. If a limit switch is engaged when homing starts the axis will be moved pull-off distance * 5 away from them. If still engaged homing will fail.  
__NOTE:__ Auto squaring is currently only tested with a simulator. Use with care!

Build 20201103:

* Added data structures for spindle encoder/spindle sync to the core. Used by drivers supporting spindle sync.
* Updated spindle sync code for MSP432 and added spindle sync capability to iMXRT1060 and STM32F4xx drivers.  
__NOTE:__ Spindle sync support is still in alpha stage! The current code has only been tested with a simulator.
* Fixed bug that could lead to settings storage area fail to reinitialize properly when corrupted.
* Moved some symbols in preparation for adding $-settings for them.
* Improved rewind capability (on `M2`) of SD card plugin. Added `$` command for dumping SD card file contents to output stream.

Build 20201020:

__NOTE:__ Settings data format has been changed and settings will be reset to default on update. Backup and restore.

* Added support for STM32F446 based Nucleo-64 boards to STM32F4xx driver.
* Fix for regression that set laser mode as default - `$32=1`. Check that this is correct after restoring settings from backup. 
* Added new settings option `$341=4` for ignoring `M6` tool change command.
* Bug fix for `M61Q0` - returned error previously.
* Changed clearing of tool length offset reference on homing to be done only if relevant axis is/axes are homed.
* Fixed check for running startup scripts on homing to check that all axes configured for homing are actually homed.
* Changes to message display from `(MSG,..)` comments in gcode.  
`(MSG,..)` strings will be sent back to the sender in sync with gcode execution.
* Added `*` prefix to NVS storage type in `$I` report if buffered, e.g: `[NEWOPT:*EEPROM,ES,TC]`
* Increased heap allocation for nearly all drivers.  
Memory from heap is used for the NVS buffer and for temporary storage of `(MSG,..)` strings.  
* Added option for adding substates to the `Run` state in the real time report to `$11` setting \(bit 11\).  
If enabled `Run:2` will be reported when a probing motion is ongoing, this can be used by senders to provide a simple probe protection scheme.  
* Another refactoring of the settings subsystem, this time for handling plugin settings.
Plugins settings storage space is now dynamically allocated and handled locally by the plugin code, this allows user defined plugins to add settings too!
Ten setting codes are reserved for user defined plugins.
* Added template for [basic plugin with two settings](templates/README.md).
* HAL pointers refactored for code readability, many moved to separate structures for each subsystem. Added typedefs.
* Some minor bug fixes and other code readability improvements. Prepared for switch to 16-bit CRC for settings data validation.

Build 20200923:

* Added support for STM32F411 based Blackpill boards to STM32F4xx driver.
* Initial changes to ESP32 driver to allow compilation with PlatformIO, added my_machine.h for this. Note that my_machine.h is not used if compiling with idf.py.
* Added home position to `$#` ngc report, e.g. `[HOME,0.000,0.000,0.000:7]` - means all axes are homed. Position is reported in machine coordinates. `:7` in the example is an axis bitfield, the reported value is for which axes are homed: bit 0 is Z, 1 is X etc. `:0` = no axes homed.
* "Hardened" the new tool change functionality even more. Initial changes for multi-axis tool reference offset made.  
An empy message will now be sent when tool change is complete, this to clear any tool change related message in the sender.
* Added call to [weak](https://en.wikipedia.org/wiki/Weak_symbol) `my_plugin_init()` function at startup, name your [plugin](https://github.com/terjeio/grblHAL/tree/master/plugins) init function `void my_plugin_init (void)` and there is no need to change any grblHAL source files to bring it alive.  
Use this feature for your private plugin only, multiple public plugins using this name cannot coexist!
* Some changes to improve code readability and added strict check for `G59.x` gcodes.

Build 20200911:

* Core refactored for better support for non-volatile storage. Some HAL entry points renamed for readability and moved to a new data structure.
* Added plugin for axis odometers. This logs total distance traveled and machining time to EEPROM/FRAM.  
[FRAM](https://www.electronics-notes.com/articles/electronic_components/semiconductor-ic-memory/fram-ferroelectric-ram-memory.php) is recommended for storage as it is faster and can sustain a larger number of write cycles. FRAM chips are sold in packages that is pin compatible with EEPROM.  
__NOTE:__ Currently for review and for now only for the iMRXT1061 \(Teensy 4.x\) driver. It will _not_ be available in configurations that stores non-volatile data to flash.
* "Hardening" of new [manual tool change](https://github.com/terjeio/grblHAL/wiki/Manual,-semi-automatic-and-automatic-tool-change) functionality. 
* Improved auto squaring algorithm in core.
* Enhanced some plugins so they can coexist.

--- 

Build 20200830:

* Improved tool change functionality, a status report is forced on end of tool change to inform change is completed.
* Fix for iMRXT1062 homing failure when serial over USB is used.
* Standardized serial over USB `#define` macro across drivers to `USB_SERIAL_CDC`.

---

Build 20200818:

* Part two of large overhaul of configuration system.

Driver configuration has been moved from _driver.h_ to _my_machine.h_ and options has been made available to be defined as compiler defined symbols/macros with the `-D` compiler command line option. In order to allow compiler defined symbols to override _my_machine.h_ settings the symbol `OVERRIDE_MY_MACHINE` should always be added to the command line. See [compiling grblHAL](https://github.com/terjeio/grblHAL/wiki/Compiling-GrblHAL) for more information. Available driver options in _my_machine.h_ can now be listed from the driver ReadMe page.

---

Build 20200813:

* "Hardened" step pulse generation when delay is enabled. The delay will now only be added on direction changes.
The minimum possible delay is dependent on many factors and will be adjusted to match what the processor is capable of.
By default driver code is calibrated for a 2.5 &micro;s delay and may deviate a bit for other settings and configurations. The actual delay should be checked with an oscilloscope when high step rates are used.
__NOTE:__ A delay is only added if AMASS is disabled, when AMASS is enabled \(it is by default\) there is always a implicit delay on direction changes.

---

Build 20200811:

* Part one of large overhaul of configuration system.
This has been done to allow a global configuration file and/or setting compile time options as compiler symbols/macros with the `-D` compiler command line option.
* Networking \(cabled ethernet\), SD Card and I2C keypad plugin support added to [Teensy 4.x driver.](drivers/IMXRT1062) A Teensy 4.1 is required for networking.
* Tool number range changed from 8-bit \(0-255\) to 32-bit \(0-4294967294\). Note that if the optional tool table is enabled the max tool number is limited by number of entries in the tool table.
* M60 and corresponding pallet shuttle HAL entry point added. No driver support for this yet.

_Configuration system changes:_

Symbols \(#define macros\) that are unlikely to be changed or is used for conditional compilation has been moved from [grbl/config.h](grbl/config.h) to [grbl/grbl.h](grbl/grbl.h).
Symbols used for conditional compilation can be overridden by `-D` compiler command line options or by editing [grbl/config.h](grbl/config.h).

[grbl/defaults.h](grbl/defaults.h) is now used for default values for settings that can be changed at run-time with the `$$` command. __Important:__ grbl/defaults.h should only be changed when adding new settings.

[grbl/config.h](grbl/config.h) is now used for overriding default values in grbl/grbl.h or grbl/defaults.h. Out of the box all overridable symbols are commented out, uncomment and change as needed to override default definitions in grbl/grbl.h or grbl/defaults.h.

These changes are part of a long term plan to create a user friendly front end for configuring and building grblHAL.

---

Build 20200805:
* **Important:** settings has been changed again and settings will be restored to defaults after updating. Backup & restore!
* Added tool change handler for [manual tool change](https://github.com/terjeio/grblHAL/wiki/Manual,-semi-automatic-and-automatic-tool-change) on M6. Four different modes available, selectable with [`$341` setting](https://github.com/terjeio/grblHAL/wiki/Additional-or-extended-settings#manual-tool-change-settings).
* STMF4xx driver updated for optional I2C EEPROM plugin support.

---

Build 20200722:
* **Important:** settings version has been changed again and settings will be restored to defaults after updating. Backup & restore! 
* Changed step pulse width and delay settings from int to float and reduced minimum allowed value to 2 microseconds<sup>1</sup>. Useful for very high step rates.
* New plugin for [quadrature encoder input](https://github.com/terjeio/grblHAL/issues/73#issuecomment-659222664) for up to 5 encoders \(driver dependent\). Can be used to adjust overrides and has rudimentary support for MPG functionality. Work in progress and the iMXRT1062 \(Teensy 4\) driver is currently the only driver with low-level support for this (one encoder).
* New plugin for [ModBus VFD](https://github.com/terjeio/grblHAL/issues/68) spindle controllers. Untested and with limited driver support in this build.
* Added setting, `$340` for spindle at speed tolerance \(percent\). If spindle fails to reach speed within limits in 4 seconds alarm 14 will be raised. Set to 0 to disable. Availability driver dependent.
* All [new settings](https://github.com/terjeio/grblHAL/wiki/Additional-or-extended-settings) are now possibly to set independent of the [compatibility level](https://github.com/terjeio/grblHAL/wiki/Changes-from-grbl-1.1#workaround) except some settings that has flags added to them. The added flags will not be available at all compatibilty levels. A new command, `$+`, can be used to list all settings independent of compatibilty level.
* Internal changes to settings data in order to simplify automatic migration on changes. Automatic migration is on the roadmap.

<sup>1</sup> Note that several factors may affect the accuracy of these settings such as step output mode, number of axes defined and compiler optimization settings.
A new #define, `STEP_PULSE_LATENCY`, has been added to driver.h for those drivers that requires it so that fine tuning can be done. I may move this to a setting later.
In setups where very high step rates are used the actual step pulse width should be confirmed with an oscilloscope.

The step pulse delay has not been fine tuned yet as setting this to a value > 0 is not normally needed as there seems to be a implicit delay on direction changes when AMASS is enabled.

__Note:__ high step rates \(e.g. above 80 kHz\) cannot be achieved with the step pulse setting set to the default 10 microseconds. This has to be reduced so that both the on and off time is within specifications of the stepper driver. At 100 kHz the time available for a pulse \(on + off\) is 10 microseconds.

__Note:__ The SAMD21 \(MKRZERO\) driver needs updating in for it to allow short step pulses. My mistake was to use some Arduino code so that the Arduino pin numbers could be used for pin mappings. This adds around 500 ns per axis of overhead... To be fixed later.

For the curious: I have managed to achieve a 400 kHz step rate with the iMXRT1062 before everything breaks down, this with a command entered from MDI. I have not tested this with a running program, but I am pretty sure that a step rate at or above 200 kHz is sustainable.

---

2020/06/18: Added driver for STM32F4xx [Black Pill](https://www.cnx-software.com/2019/12/24/stm32-black-pill-board-features-stm32f4-cortex-m4-mcu-optional-spi-flash/), code modified by @shaise from the STM32F1xx driver. This is the first driver provided by someone else than me, thanks for that.

This driver is a candidate along with the IMXRT1062 \(Teensy 4.x\) driver to get spindle sync support. I have a [NucleoF411RE development board](https://www.st.com/en/evaluation-tools/nucleo-f411re.html) on order and will look into adding a pin mapping for that when it arrives. 

---


Build 20200603:
* **Important:** settings version has been changed and settings will be restored to defaults after updating. Backup & restore! 
* Optimizations for ring buffer handling in planner and step generator.
* New optional input signal for probe connected status, driver support will be added later to selected drivers.
* Automatic reporting of tool length offset \(`[TLO:...]`\) when changed.
* Support for [G5](http://www.linuxcnc.org/docs/2.5/html/gcode/gcode.html#sec:G5-Cubic-Spline) \(cubic spline\) added.
* `G43.x`, `G49` and `G92` added to parser state report.
* `G76` [threading cycle](https://hackaday.io/project/165248-mini-lathe-emco-compact-5-cnc-conversion) refactored.
* \(Re\)added `REPORT_PROBE_COORDINATES` and `TOOL_LENGTH_OFFSET_AXIS` [configuration](grbl/config.h) options, the latter available when `COMPATIBILITY_LEVEL` > 2.
* Improved backwards compatibility with vanilla grbl, e.g. G92 and tool offset\(s\) will be lost on a soft reset. Dependent on `COMPATIBILITY_LEVEL` setting.
* Board name added to `$I` report if provided by driver.
* [Grbl-Sim](https://github.com/grbl/grbl-sim) ported to grblHAL as a [driver](drivers/Simulator). Added telnet support++. Can be used to test senders. Note: currently only compiled/tested for Linux.
* Some minor bug fixes.

---

Build 20200503: Added configuration flag for manual homing. \(Re\)added compile time option `ENABLE_SAFETY_DOOR_INPUT_PIN` for [safety door switch](https://github.com/terjeio/grblHAL/blob/master/grbl/config.h), default is now disabled. Some bug fixes and "hardening" of code.

---

Added some [template code](./templates/README.md) to aid customizations such as driver support for M62 - M68 M-codes mentioned below.

---

Build 20191222: Added digital and analog output support to the core \(and HAL\) as per [linuxcnc specifications for M62 - M68](http://linuxcnc.org/docs/html/gcode/m-code.html#mcode:m62-m65), number of outputs available \(if any\) is driver dependent. Adding support for these M-commands makes it fairly easy to add driver code \(for up to 256 outputs\) as parsing and synchronization is taken care of by the core.

---

Build 20191215: Moved spindle RPM linearization to $-settings, option needs to be enabled in config.h - driver support required. Optimized EEPROM allocation handling. WebUI support for ESP32 driver improved.  

MSP432 driver enhanced for spindle linearization app in the pipeline \(for Windows only - needs input from spindle encoder\), more work done on closed loop spindle RPM control and spindle synchronized motion - still at experimental stage.

__NOTE:__ settings version number has been increased so settings will be reset to default after update, make a backup first!

---

Added [#define COMPATIBILITY_LEVEL to config.h](grbl/config.h) for backwards compatibility with Grbl v1.1 protocol definition, this for enabling the use of more GCode senders. Please raise an issue if your sender still does not behave well after setting this as the current implementation does not yet disable all extensions, notably [new $xx settings](doc/markdown/grblHAL%20extensions.md#settings).

G76 threading support added to grblHAL in combination with the [MSP432 driver](drivers/MSP432/README.md). Extensive testing is required before it can be regarded as safe.

**WARNING!** This is a potentially dangerous addition. Do NOT use if you do not understand the risks. A proper E-Stop is a must, it should cut power to the steppers and if possible engage any spindle brake. The implementation is based on the [linuxcnc specification](http://linuxcnc.org/docs/2.6/html/gcode/gcode.html#sec:G76-Threading-Canned). Please note that I am not a machinist so my interpretation and implementation may be wrong!

G76 availablity requires a spindle encoder with index pulse, grblHAL configured to [lathe mode](doc/markdown/settings.md#opmode) and tuning of the spindle sync PID loop.  
__NOTE:__ Feed hold is delayed until spindle synced cut is complete, spindle RPM overrides and CSS mode disabled through the whole cycle. 
