# mhw-thk-logger

Requires `fmt` and `MinHook` to work. Install with vcpkg like this:
```
./vcpkg install fmt minhook nlohmann-json --triplet=x64-windows-static
```

# Commands
Commands without arguments act as toggles for boolean values. Commands are sent through ingame chat.

 * /thk filter 
Filter logging to a single monster given by monster internal id.

 * /thk segments [true|false]
Print segments instead of just nodes. 

 * /thk registers [true|false]
Print registers per segment. Requires segment print out to be active.

 * /thk pause
Pause global monster timer. Freezing them.

 * /thk unpause
Unause global monster timer. Freezing them.

 * /thk crash_pause [true|false]
Pause if an apparently invalid THK node is next in execution.

 * /thk bp add|del|lst [THK|THK Node|THK Node Segment]

Sets/Removes a breakpoint at a given segment. If the monster filter is active, the breakpoint is exclussive to the monster in question. If no value is passed it sets the breakpoint in THK00, Node 0 and Segment 0. Succesive arguments set THK, Node and Segment indices. lst lists all of the breakpoints already set.


 * /thk symbols
Import a symbols file. It opens a file dialog to provide a Leviathon Symbols file. To make Leviathon export a symbols file for your project pass the `-symbols` flag to the compiler.