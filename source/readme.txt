Use following flags for compilation:
	- APAD_DEBUG
		- If not used, compiler will assume a release build using /O2''
	- APAD_DLL
		- For when compiling dlls, to avoid the compiler from throwing a fit about linkage
			when including .h files which use the imported_function macro