# ESPL Opponents

You will find pre-compiled executables to game opponents in this directory.

Detailed documentation can be found here: https://github.com/alxhoff/FreeRTOS-Emulator/wiki/Opponents

## Manual

General Usage:

```
./example_opponent [-v] [--lock] [--host HOSTNAME] [--port PORT] [--difficulty 1|2|3]
```

Parameters:

- `--verbose`, `-v`
Shows detailed logging output if toggled on
*Default:* false

- `--lock`, `-l`
Disable ability change the difficulty via the sockets.
*Not implemented!*

- `--host`, `-h`
Hostname of the system running the Game/Emulator
*Default:* localhost

- `--port`, `-p`
Port the transmission (TX) socket. (RX will use port+1)
*Default:* 1234

- `--difficulty`, `-d`
Hardness of the opponent. (1 is lowest, 2 medium and 3 highest)
*Default:* 2 (medium)
