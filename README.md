# Operation Systems (OSs) Course Assignment 2

### For Computer Science B.S.c Ariel University

**By Roy Simanovich and Linor Ronen**

## Description

* **Compare (cmp)** – The tool compare two files, and return "0" if they are equal, or "1" if not (or incase of an error). The tool supports -v flag for verbose output. By this we mean that the tool will print "equal" or "distinct", in addition to returning the int value. The tool also supports -i flag, that mean "ignore case" so "AAA" and "aaa" meaning equals.
* **Copy (copy)** – The tool copy a file to another place and/or name, and return "0" on success, or "1" on failure. The tool creates a new file, if it does not exist, but it will not overwrite a file if it do exist. The tool supports -v flag, that will output "success" if the file is copied, or "target file exist" if this is
  the case, or "general failure" on some other problem (in addition to the returned value). The tool also supports -f flag (that means force), that allows to overwrite the target file.
* **Encoder (encode) and Decoder (decode)** – A small program that uses a shared library (codec) to encode a given text. libcodecA.so covert all lower case chars to upper case, and all upper case to lower case. All other chars will remain unchanged. libcodecB.so convert all chars to the 3-rd next char (adding a number of 3 to the ASCII value). The codecs are reversiable functions, meaning they have encoders and decoders.
* **Students Shell (stshell)** – A small shell program that supports the following features:
  1. Be able to run CMD tools that exist on system.
  2. Be able to stop running tool by pressing Ctrl+C, but not killing the shell itself.
  3. Be able to redirect output with ">" and ">>", and allow piping with "|" (unlimited pipes).
  4. Be able to stop itself by "exit" command.

## Requirements

* Linux machine
* GNU C Compiler
* Make

## Building

```
# Cloning the repo to local machine.
git clone https://github.com/RoySenpai/opsys_hw2.git

# Building all the necessary files & the main programs.
make all

# Export shared libraries.
export LD_LIBRARY_PATH="."
```

## Running

```
# Run the compering files program.
./cmp <file1> <file2> [-iv]

# Run the copying files program.
./copy <src> <dest> [-fv]

# Run the encoder program with codecA.
./encode codecA "text"

# Run the encoder program with codecB.
./encode codecB "text"

# Run the decoder program with codecA.
./decode codecA "text"

# Run the decoder program with codecB.
./decode codecB "text"

# Run the students shell.
./stshell

# Run the compering files program via student shell.
cmp <file1> <file2> [-iv]

# Run the copying files program via student shell.
copy <src> <dest> [-fv]

# Run the encoder program with codecA via student shell.
encode codecA text

# Run the encoder program with codecB via student shell.
encode codecB text

# Run the decoder program with codecA via student shell.
decode codecA text

# Run the decoder program with codecB via student shell.
decode codecB text
```
