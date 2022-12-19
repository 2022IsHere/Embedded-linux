### Step 5: Compilation process

The common tool for C compilation in Linux is `gcc`. The general process from C source to a running program has four preparation steps:
1. <b>Preprocessor</b> processes one C source file at a time: for each it expands macros (#define's), inserts contents of included files (#include's) and produces intermediate file also known as <b>translation unit</b>. Each translation unit is compiled separately.
2. <b>Compiler</b> translates C source from each translation unit into target machine code, resulting in <b>object file</b>. Object files contain machine instructions and debug symbols, but they have no set links to any libraries or other translation units. You can not run object files.
3. <b>Linker</b> merges multiple object files adding address bindings between them, and also to <b>static libraries</b> which are permanently addded to same output file. This output file is executable in the sense that operating system knows how to run it.
4. <b>Loader</b> does the final preparation when OS starts running the program. Loader links the code to <b>dynamic libraries</b> (Windows) / <b>shared libraries</b> (Linux) in the system.  
