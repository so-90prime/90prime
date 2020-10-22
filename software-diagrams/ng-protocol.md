# NG Protocol COMMANDS/REQUESTS - 90Prime
All available commands and requests to control 90prime filter box. 

** This documentation will be evolving over time and is not finalized. **

## Quickstart
To get the current LVDT values from 90Prime.
```console
dan@lump: ~
$ echo "BOK 90PRIME 123 REQUEST LVDT" | nc localhost 5750
BOK 90PRIME 123 -700 -900 -500
```
Tell 90Prime to change filters to filter in position 0
```console
dan@lump: ~
$ echo "BOK 90PRIME 123 COMMAND FILTER CHANGE 0" | nc localhost 5750
BOK 90PRIME 123 OK
```

## Commands
```
LVDT

    POS         Change wanted LVDT values
                Args: AAAA BBBB CCCC
                    AAAA : LVDT A in microns
                    BBBB : LVDT B in microns
                    CCCC : LVDT C in microns
                Returns: OK, FAILED

    TOL         Set the tolerance to use for LVDTs
                Args: X
                    X : Integer value of +- tolerance for LVDTs in microns
                Returns: OK, FAILED

    LOCK        Sets the lock for LVDTs
                Args: ON, OFF
                Returns: OK, FAILED
```
```
FOCUS

    POS         ??
                Args: ??
                Returns: OK, FAILED

    MAP         Toggles applying focus map
                Args: ON, OFF
                Returns: OK, FAILED
 ```
 ```
REFERENCE

    SET         Sets the reference state as current values
                Args: N/A
                Returns: OK, FAILED
                
    RESTORE     Sets the current state from reference state
                Args: N/A
                Returns: OK, FAILED

    NOMPLANE    Sets the current state to the nominal plane
                Args: N/A
                Returns: OK, FAILED
```  
```
FILTER

    READ        Reads the filter wheel
                Args: N/A
                Returns: OK, FAILED
                
    LOAD        Loads the supplied filter position
                Args: X
                    X : Integer between 0-5, 0 being filter position 1 and 5 being filter position 6
                Returns: OK, FAILED
                
    UNLOAD      Unloads the current filter
                Args: N/A
                Returns: OK, FAILED
                
    CHANGE      Shortcut for unloading and loading specified filter
                Args: X
                    X : Integer between 0-5, 0 being filter position 1 and 5 being filter position 6
                Returns: OK, FAILED        
```      
## Requests
```
ALL             Informations users would want frequently
                Args: N/A
                Returns: payload made up of floats, bytes, and strings 
                Format: [lvdt values] [encoder values] [limits] [filter position] [filter name] [filter state]
                    lvdt values		: AAAA.AA BBBB.BB CCCC.CC
                    encoder values	: AAAA BBBB CCCC
                    limits          : 
                    filter position	: 
                    filter name		:
                    filter state	:
```
```
LVDT            Values of LVDTs in microns
                Args: N/A
                Returns: Three integer values in A, B, C LVDT order
                Format: AAAA BBBB CCCC
                    AAAA : LVDT A value in microns
                    BBBB : LVDT B value in microns
                    CCCC : LVDT C value in microns
```
```
REFERENCE       ??
                Args: N/A
                Returns: Three integer values for reference LVDT values in A, B, C order
                Format: AAAA BBBB CCCC
                    AAAA : LVDT A reference value in microns
                    BBBB : LVDT B reference value in microns
                    CCCC : LVDT C reference value in microns
```
```
ENCODER         Encoder values unitless
                Args: N/A
                Returns: Three integer values for encoders unitless
                Format: AAAA BBBB CCCC
                    AAAA : Encoder A value
                    BBBB : Encoder B value
                    CCCC : Encoder C value
```
```
STATUS          Status of 90prime box
                Args: N/A
                Returns: BUSY, IDLE, ERROR
                    BUSY : There is motion in the 90prime box, commands will be ignored
                    IDLE : Box is ready for next command
                    ERROR : There is an error
```    
       
    
