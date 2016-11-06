/*
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

*/

#include <stdafx.h>
#include <string.h>

// ********** EDIT THIS (Maybe) **********
// Uncomment this next line to force printing of runtime debug information to stderr.
// Not required for normal operation, but helpful to see progress along the way as the program runs.
//#define __DEBUG__
#ifdef __DEBUG__
int debugFlag = 1;
#else
int debugFlag = 0;
#endif // __DEBUG__

// Typedefs to mimic Arduino types & structures used in the font header file
typedef unsigned char 		uint8_t;
typedef unsigned short int 	uint16_t;
typedef signed char 		int8_t;

// Typedefs to mimic Arduino types & structures used in the font header file
typedef struct { // Data stored PER GLYPH
	uint16_t bitmapOffset;     // Pointer into GFXfont->bitmap
	uint8_t  width, height;    // Bitmap dimensions in pixels
	uint8_t  xAdvance;         // Distance to advance cursor (x axis)
	int8_t   xOffset, yOffset; // Dist from cursor pos to UL corner
} GFXglyph;

// Typedefs to mimic Arduino types & structures used in the font header file
typedef struct { // Data stored for FONT AS A WHOLE:
	uint8_t  *bitmap;      // Glyph bitmaps, concatenated
	GFXglyph *glyph;       // Glyph array
	uint8_t   first, last; // ASCII extents
	uint8_t   yAdvance;    // Newline distance (y axis)
} GFXfont;


// ********** EDIT THIS **********
// Include the full font file. 
// You MUST ALSO remove the "PROGMEM" keyword from the typedefs in the original header file
// because most non-arduino compilers don't recognize the PROGMEM directive
#include "FreeSansBold24pt7b.h"  // Change this to your desired font file

// ********** EDIT THIS **********
// REPLACE THIS CHARACTER STRING with the characters you need from the font file.
static char *wantedCharacters = "0123456789ABCDEF:/";

// Function returns index into wantedCharacters if the character is needed
// returns -1 if it is not wanted
int isWantedChar(char c)
{
	int len = strlen(wantedCharacters);
	int i;

	for (i=0; i<len; ++i)
	{
		if (c == wantedCharacters[i]) // Found the character we want
			return i;
	}
	
	return -1;	// Didn't find the character we want.
}
	
int main()
{
	int newBitmapOffset, neededElementIndex;
	int oldGlyphArraySize, oldBitmapArraySize;
	int firstUseableChar = -1, lastUseableChar = -1;
	int startOffsetPointer, endOffsetPointer;
	int i, j;
	
	if (debugFlag)
		fprintf(stderr, "Starting Font Compression!\n");

	// Get sizes of old structures (# of elements, not memory size)
	// ********** EDIT THIS **********
	// Change the name of the font in the two sizeof() calls below to the name of your font
	oldBitmapArraySize = sizeof(FreeSansBold24pt7bBitmaps);
	oldGlyphArraySize = sizeof(FreeSansBold24pt7bGlyphs) / sizeof(GFXglyph);
	
	if (debugFlag)
		fprintf(stderr, "Old bitmap size: %d elements. Old glyph size: %d elements\n", oldBitmapArraySize, oldGlyphArraySize );

	// Create structures for the new font data
	uint8_t *newBitmaps = new uint8_t[oldBitmapArraySize];
	GFXglyph *newGlyphs = new GFXglyph[oldGlyphArraySize];

	if (debugFlag)
		fprintf(stderr, "%d glyphs in old list.  %d bitmaps in old list\n", oldGlyphArraySize, oldBitmapArraySize);
	
	// Offset 0 in the newBitMaps array is reserved for all the characters we don't want.
	newBitmaps[0] = 0x00;

	// Offset 1 starts the bitmaps for the elements we do want
	newBitmapOffset = 1;
	for (i = 0; i < oldGlyphArraySize; ++i) // Loop through all 95 standard glyphs
	{
		if (debugFlag)
			fprintf(stderr, "Glyph %d\n", i);

		// See if the current glyph is one of the "wanted" glyphs
		// First possible character is ' '(space), hex 20, dec 32
		neededElementIndex = isWantedChar(i + 32); 
		if (neededElementIndex >= 0) // Got a wanted character
		{
			newGlyphs[i].bitmapOffset = newBitmapOffset;		// Find starting point of old bitmaps
			// ********** EDIT THIS **********
			// Change the font name in the 5 lines below to your font name
			newGlyphs[i].width = FreeSansBold24pt7bGlyphs[i].width;		// Set character dimensions same as the old ones
			newGlyphs[i].height= FreeSansBold24pt7bGlyphs[i].height;		// Set character dimensions same as the old ones
			newGlyphs[i].xAdvance= FreeSansBold24pt7bGlyphs[i].xAdvance;	// Set character dimensions same as the old ones
			newGlyphs[i].xOffset = FreeSansBold24pt7bGlyphs[i].xOffset;	// Set character dimensions same as the old ones
			newGlyphs[i].yOffset = FreeSansBold24pt7bGlyphs[i].yOffset;	// Set character dimensions same as the old ones
			
			if (debugFlag)
				fprintf(stderr, "\tNeed Glyph: %d. Offset=%d Width=%d Height=%d xAdvance=%d xOffset=%d, yOffset=%d\n", 
				i, newGlyphs[i].bitmapOffset, newGlyphs[i].width, newGlyphs[i].height, newGlyphs[i].xAdvance, newGlyphs[i].xOffset, newGlyphs[i].yOffset);

			// Copy old bitmaps for this character to new bitmap array
			// Get the original start & end offsets for the character
			// ********** EDIT THIS **********
			// Change the font name in the 2 lines below to your font name
			startOffsetPointer = FreeSansBold24pt7bGlyphs[i].bitmapOffset;
			endOffsetPointer = FreeSansBold24pt7bGlyphs[i+1].bitmapOffset; 	// Actually, this is the start of the bitmap array for the next char. 
																			//Use this as the upper bound of the current character

			if (debugFlag)
				fprintf(stderr, "\tOffset Pointer: Start=%d. End=%d\n", startOffsetPointer, endOffsetPointer);

			// Add those bitmaps to the new bitmap array
			for (j = startOffsetPointer; j < endOffsetPointer; ++j)
			{
				// ********** EDIT THIS **********
				// Change the font name in the 1 line below to your font name
				newBitmaps[newBitmapOffset] = FreeSansBold24pt7bBitmaps[j];

				if (debugFlag)// Print out bitmap elements
					fprintf(stderr, "%02x-", newBitmaps[newBitmapOffset]);

				++newBitmapOffset; // Increment new bitmap offset so the next bitmap element goes in the next available location
			}

			if (debugFlag)
				fprintf(stderr, "\n");

			// The GFXFont structure allows you to store the 1st & last usable character. Let's save those numbers 
			// See of this is the first character in the glyph array that we need
			if (firstUseableChar == -1)	  // Only set this one time
				firstUseableChar = i;		
			lastUseableChar = i;		// Set this for every useable character. You can't know if this is the last one until you're done
			
		}
		else
		{
			// Set character parameters to the same as space character - smallest in set, no width/height, offset location 0
			newGlyphs[i].bitmapOffset = 0;
			newGlyphs[i].width = 0;	
			newGlyphs[i].height = 0;
			newGlyphs[i].xAdvance = 13;
			newGlyphs[i].xOffset = 0;
			newGlyphs[i].yOffset = 1;
			
			if (debugFlag)
				fprintf(stderr, "Not Needed\n");
		}
	}

	if (debugFlag)
		fprintf(stderr, "First useable char:%d  Last useable char: %d\n", firstUseableChar, lastUseableChar);

	// Print new bitmap array header
	// ********** EDIT THIS **********
	// Change the font name in the 1 line below to your font name
	fprintf(stdout, "const uint8_t FreeSansBold24pt7bBitmaps[] PROGMEM = {\n");
	
	// Loop through all the bitmaps in the new array
	for (i = 0; i < newBitmapOffset; ++i)
	{
		fprintf(stdout, "0x%02X, ", newBitmaps[i]);

		// Output in neat rows of 12 data points each
		// I don't think there's any computing significance to groups of 12,
		// but it makes for easy printing/reading on a standard page.
		// This is how the original font files are formatted, so this is how we will output them
		if ((i > 0) && ((i % 12) == 0))
			fprintf(stdout, "\n");
	}

	// This is an extra (unneeded) element, but we use it as padding so we don't have to go through 
	// lots of formatting & comma gymnastics in the previous for/fprintf loop.
	fprintf(stdout, "0x00 };\n\n");  

	// Print new glyph array
	// ********** EDIT THIS **********
	// Change the font name in the 1 line below to your font name
	fprintf(stdout, "const GFXglyph FreeSansBold24pt7bGlyphs[] PROGMEM = {\n");
	
	
	// Loop through all the useable characters except the last
	// (formatting issues, again)
	for (i=firstUseableChar; i< lastUseableChar; ++i)
	{
		fprintf(stdout, "{ %4d, %4d, %4d, %4d, %4d, %4d }, // 0x%02X '%c'\n",
			newGlyphs[i].bitmapOffset, newGlyphs[i].width, newGlyphs[i].height, newGlyphs[i].xAdvance, newGlyphs[i].xOffset, newGlyphs[i].yOffset, (i + 32), (i + 32));
	}

	// Now the last useable character
	fprintf(stdout, "{ %4d, %4d, %4d, %4d, %4d, %4d } }; // 0x%02X '%c'\n\n",
		newGlyphs[i].bitmapOffset, newGlyphs[i].width, newGlyphs[i].height, newGlyphs[i].xAdvance, newGlyphs[i].xOffset, newGlyphs[i].yOffset, (i + 32), (i + 32));

	// ********** EDIT THIS **********
	// Change the font name in the 3 lines below to your font name
	fprintf(stdout, "const GFXfont FreeSansBold24pt7b PROGMEM = {\n");
	fprintf(stdout, "(uint8_t  *)FreeSansBold24pt7bBitmaps,\n");
	fprintf(stdout, "(GFXglyph *)FreeSansBold24pt7bGlyphs,\n");

	// Now provide the first & last useable characters
	fprintf(stdout, "0x%02X, 0x%02X, 56 };\n", firstUseableChar + 32, lastUseableChar + 32);

	if (debugFlag)
		fprintf(stderr, "Ending Font Compression!\n");


	return 0;
}

