Define the APAD_DEBUGGER_ASSERTIONS macro during compilration to enable __debugbreak() assertions within a debugger.

Need to disable optimisations and generate debug data (/Od and /Zi respectively when using the Microsoft C/C++ compiler), as well as use the debug version of the library, print the stack backtrace within assertions is desired.