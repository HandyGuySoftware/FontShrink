
FontShrink.cpp
Stephen Fried, November 2016

Reduce the size of an Arduino custom font by only including the characters you need in the header file.

This program will parse through the information found in an Adafruit Arduino GFX font definition header file
and extract only the bitmap information for the chcracters it needs. By eliminating the bitmap information
for unneeded characters (or "glyphs", in the header definition) you can dramatically reduce the amount
of Arduino memory needed to store the font.

INSTRUCTIONS:

1. Copy the header file you are trying to shrink (e.g. FreeSansBold24pt7b.h) to a separate directory.
You'll want to work on a copy of the header file, not the original.

2. Edit the header file to remove the keyword "PROGMEM" from the structure definitions. Most C/C++ compilers don't 
understand the keyword and will fail to compile the code. The program will re-insert the PROGMEM keyword in the appropriate place as it builds
the output.

3. Go through this program and change every reference to the font name or font file to the name of your font. Look for
places that say "// ********** EDIT THIS **********". Be careful, some of the font name references are embedded as part of
a longer variable or struct name. Change ONLY the font name, leaving the rest of the variable/struct name in tact.

4. Edit the "wantedCharacters" variable definition below to include only the characters you are looking for. 

5. Compile the program. I used Visual Studio to build this as a WIN32 console application, but the code should be generic and straightforward enough that you can easily 
port it to your language/compiler/platform of choice.

6. Run the program, redirecting the standard output stream (stdout) to the new header file. For example:

C:\Users\me> FontShrink.exe > FreeSansBold24pt7b-mini.h

7. If you want to see runtime messages, uncomment the line that says "//#define __DEBUG__" and recompile. This will include printf debug statements
into the program. The debug statements will print to the standard error stream (stderr), so it won't affect the resulting header file sent to stdout.

8. Take the resulting header file ("i.e. FreeSansBold24pt7b-mini.h") and include that in your Arduino sketch.

This program was built for a specific use case and has not been extensively tested. However, feel free to adapt the code to your own needs.

This program is released into the wild as open-source software under Unilicense. See the file UNILICENSE.txt for terms and conditions.

