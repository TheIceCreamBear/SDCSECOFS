# SDCSECOFS
Senior Design Concurrency Simulator: Exploring C Options For Simulation 
Chances are, if you are looking at this repository and have no idea what it means, it isnt meant for you. This repository is public as to save me the time and enegry i would spend sharing this repositry with the people i need to share it with.

## compiling
***IF*** you know that this project is for you or related to you, to compile on any platform
 - make sure you have a compiler installed available from command line
 - have a terminal open in the directory this README.MD file is in
 - run the following command: ```<compiler> src/ourprogram.c src/userprogram.c src/concurrencylib.c -o bin/<outputFileName>```

Please note: precompiled version are included as part of the git tracking, with the formatting `<os>-[option]-<arch>.<ext>` where:
 - `os` is either: 
   - `windows`
   - `linux`
   - `macos` (comming soon)
 - `option`
   - optional, used to show evolution of the binaries over time
   - blank: when blank the compiled binary is only the function naming text
   - `threads`: shows the cross platform threads off
 - `arch` is: `x86_64`
 - `ext` is either: 
   - `exe` for windows
   - `out` for linux/macOS