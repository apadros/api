Compilation macros:
	
	APAD_DEBUG 
		- Will enable debug assertions useful only when using a debugger
		- Otherwise release assertions with global error setting will be enabled
		
	APAD_BUILD_DLL
		- Changes linkage of within header files from imported_function to exported_function to avoid inconsistent dll linkage compiler warning