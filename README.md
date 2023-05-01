# AT-Commands RN2483 using LMIC

This repository is a expansion of the LMIC library to work with the RN2483 AT commands.

Here is an overview of wich commands are currently implemented and working correctly:

## System Commands
This commands issues system level behaviour actions, gathers status information on the firmware and hardware version, or accesses the module user EEPROM memory

These commands always start with **sys**.

The nvm user EEPROM is located at the end of the EEPROM all the other data is the

- sys reset
- sys factoryRESET
- sys set nvm 'address' 'data'
    - address range: 300 - 3FF
    - data range: 00 - FF
- sys set pindig 'pinname' 'pinstate'
    - pinnames: gpio0 - gpio13
    - pinstate: 'on' or 'off'
- sys get ver
- sys get nvm 'address'
    - address range: 300 - 3FF



## MAC Commands
This commands issues LoRaWAN class A protocol network communication behaviors, actions and configurations.


These commands always start with **mac**.