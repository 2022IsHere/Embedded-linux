### Step 5: Compilation and execution process

The general process from C source to a running program has four preparation steps:
1. <b>Preprocessor</b> processes one C source file at a time: for each it expands macros (#define's), inserts contents of included files (#include's) and produces intermediate file also known as <b>translation unit</b>. Each translation unit is compiled separately.
2. <b>Compiler</b> translates C source from each translation unit into target machine code, resulting in <b>object file</b>. Object files contain machine instructions and debug symbols, but they have no set links to any libraries or other translation units. You can not run object files.
3. <b>Linker</b> merges multiple object files adding address bindings between them, and also to <b>static libraries</b> which are permanently added to same output file. This output file is <b>executable binary</b> or <b>executable</b>, and it typically contains debug symbols in ELF format.
4. <b>Loader</b> loads the executable from mass memory (HDD or SSD) into main memory (DRAM), and links the code to <b>dynamic libraries</b> (Windows DLL) / <b>shared libraries</b> (Linux .so) in the system. Dynamic libraries aim to minimise the main memory usage for the complete system: If all running programs use printf for output, it is not necessary to load a separate library instance for every application. The applications can share one dynamic library. Loader makes sure all required shared libraries are loaded before the executable is started. Note that in <b>bare-metal systems</b> there is no OS and no loader, so linker must prepare complete system memory image.

The common tool for C compilation in Linux is `gcc`. It is user-friendly by default, and runs steps 1-3 automatically if possible. It is still necessary to understand the process, because the process makes compilation error messages more understandable.

### Lab assignment:

In VM in your local repository folder (embedded-linux-labs/lab1),
- create a subfolder named native-build-hello and do following work in it:
- write tiny C application named lab1.c that prints timestamped hello message to an output file (use append)
- compile the code using VM native gcc (producing x86 architecture executable) `gcc lab1.c -o lab1`
- use command `file lab1` to check that the executable is built for x86(-64) architecture
- open two terminals, one with `tail -f your_output_filename` and in other you run your application.
- check git status
- add source file to git (and create .gitignore file and add executable `lab1` there), commit
- push local repository changes to your remote repository in TUAS gitlab 

Reflection: In embedded application, why would you write application output to a file instead of just printf it to terminal?
