Compilation macros:
	APAD_DEBUGGER_ASSERTIONS  - Enable assertions breaking in a debugger
	APAD_ASSERTIONS_BACKTRACE - Enable printing of assertion call stack
	
Linkage
	- user32.lib for basic Windows operation
	- dbghelp.lib for stack back trace printing
	- gdi32.lib and opengl32.lib for Dekstop App GUI functionality