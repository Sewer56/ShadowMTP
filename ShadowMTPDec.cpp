/*
	HeroesBLK, my third C++ program, utility to decompress Shadow The Hedgehog's
	Motion Packages .MTP, archives into raw Renderware .anm(s).
    Copyright (C) 2017  Sewer56lol

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>
*/

// basic file operations
#include <iostream> //I/O Stream
#include <iomanip> //I/O Manipulation
#include <fstream> //File Stream
#include <string> // String
#include <cstring> //Strings
#include <sstream> //String Stream
#include <vector> //Vector the crocodiles

#include <dirent.h> // Directory Entities

// Speedup: Use this on Linux! Search for #DANKMEME.
//#include "byteswap.h" //Woot, mah endians m8

using namespace std; //Standard Namespace

class ShadowANMObject
{
	public:
		// Default Constructor, choochoo!
		ShadowANMObject() {}

		// Animation Name, obtained from the Animation Name Offset
		string AnimationName;

		// Animation Size, obtained from the Animation Offset
		unsigned int AnimationSize;

		// Animation Property Size, obtained from the AnimationPropertyOffset offset, up until the next AnimationPropertyOffset offset.
		unsigned int AnimationPropertySize;

		/* --------------------------------------------------------------- */

		// Offset where the animation size will be contained. (at 0x8 offset)
		unsigned int AnimationDataOffset;

		// Offset where the animation
		unsigned int AnimationNameOffset;

		// Unknown Value
		unsigned int AnimationPropertyOffset;
};

void MTPToANM(string InputFile, string OutputFile) {

	// File size
	size_t FileSize = 0;

	// A copy of the file will be stored here.
	char* MTPFileData;

	// Just for storing individual bytes, makes my head feel more comfortable.
	char* TwoByteBuffer; TwoByteBuffer = new char[2];
	char* FourByteBuffer; FourByteBuffer = new char[4];
	char* EightByteBuffer; EightByteBuffer = new char[8];

	// Amount of individual stored animations.
	unsigned short int AnimationEntries;

	// Make the directory specified
	const char* OutputDirectory = OutputFile.c_str();
	mkdir(OutputDirectory);

	// Load the file.
	ifstream FileReader(InputFile,ios::binary);

	// Load file and get file size ==>
	// Validates if reading of the file has been successful.
	if (! FileReader) { cout << "Error opening file" << endl; } else { cout << "Successfully opened object file\n" << endl; }
	FileReader.seekg(0, ios::end); // Set the pointer to the end
	FileSize = FileReader.tellg(); // Get the pointer location
	FileReader.seekg(0, ios::beg); // Set the pointer to the beginning

	// Make a vector to store all of the animation names, offsets and sizes.
	vector<ShadowANMObject> ShadowObjects;

	// Allocates enough room for the MTP file.
	MTPFileData = new char[FileSize + 1];

	// Read the file into the array.
	FileReader.read(MTPFileData, FileSize);

	// This will get us our animation entry amount #.
	TwoByteBuffer[0] = MTPFileData[3]; TwoByteBuffer[1] = MTPFileData[2];

	// Set the Animation Entries to the found hex value;
	AnimationEntries = *(unsigned short *)TwoByteBuffer;

	// Resize the vector to equal the amount of animation entries in file;
	// ShadowObjects.resize(AnimationEntries);

	// Print ObjectHeader
	cout << "--------------" << endl;
	cout << "Animation Data" << endl;
	cout << "--------------\n" << endl;

	// Print AnimCount to console.
	cout << "Shadow MotionPackage Animations Count: " << AnimationEntries << endl;

	// Start the CurrentAnimationPointerLocation at 0x10; Where Animation Entries begin.
	unsigned int CurrentAnimationLocation = 0x10;

	// For each animation entry in .MTP
	// This is the main information extraction loop.
	for(unsigned int x = 0; x < AnimationEntries;)
	{
		cout << "\n =>> Animation: " << x << endl;
		ShadowANMObject* TemporaryShadowObject = new ShadowANMObject();

		// Note: We're working with Little Endian systems, load the data backwards.

		/* Get the Unknown Value */
		FourByteBuffer[0] = MTPFileData[CurrentAnimationLocation + 3];
		FourByteBuffer[1] = MTPFileData[CurrentAnimationLocation + 2];
		FourByteBuffer[2] = MTPFileData[CurrentAnimationLocation + 1];
		FourByteBuffer[3] = MTPFileData[CurrentAnimationLocation + 0];
		TemporaryShadowObject -> AnimationPropertyOffset = *(unsigned int *)FourByteBuffer;

		/* Get the Animation Name Offset */
		FourByteBuffer[0] = MTPFileData[CurrentAnimationLocation + 7];
		FourByteBuffer[1] = MTPFileData[CurrentAnimationLocation + 6];
		FourByteBuffer[2] = MTPFileData[CurrentAnimationLocation + 5];
		FourByteBuffer[3] = MTPFileData[CurrentAnimationLocation + 4];
		TemporaryShadowObject -> AnimationNameOffset = *(unsigned int *)FourByteBuffer;

		/* Get the Animation Data Offset */
		FourByteBuffer[0] = MTPFileData[CurrentAnimationLocation + 11];
		FourByteBuffer[1] = MTPFileData[CurrentAnimationLocation + 10];
		FourByteBuffer[2] = MTPFileData[CurrentAnimationLocation + 9];
		FourByteBuffer[3] = MTPFileData[CurrentAnimationLocation + 8];
		TemporaryShadowObject -> AnimationDataOffset = *(unsigned int *)FourByteBuffer;

		/* Print Details of Object */
		cout << "Unknown Optional Archive Property Offset: " << TemporaryShadowObject -> AnimationPropertyOffset << endl;
		cout << "Animation Name Offset: " << TemporaryShadowObject -> AnimationNameOffset << endl;
		cout << "Animation Data Offset: " << TemporaryShadowObject -> AnimationDataOffset << endl;

		/* Resolve Data Size Of File */
		/*
			Note, we are grabbing it from the .anm file itself at offset 0x8 inside the file.
			This data starts at AnimationDataOffset.
		*/

		/* Get data size offset from inside the .anm file */
		unsigned int AnimationDataSizeOffset = (TemporaryShadowObject -> AnimationDataOffset) + 0x8;

		/* Dump this data size to the object */
		FourByteBuffer[0] = MTPFileData[AnimationDataSizeOffset + 3];
		FourByteBuffer[1] = MTPFileData[AnimationDataSizeOffset + 2];
		FourByteBuffer[2] = MTPFileData[AnimationDataSizeOffset + 1];
		FourByteBuffer[3] = MTPFileData[AnimationDataSizeOffset + 0];
		TemporaryShadowObject -> AnimationSize = *(unsigned int *)FourByteBuffer;

		cout << "Animation Size: " << TemporaryShadowObject -> AnimationSize << endl;

		/* Resolve Object Name */
		/*
			Reads hex string at the offset until meeting \x00.
			Merely appends to a vector.
		*/
		unsigned int AnimationNameOffset = TemporaryShadowObject -> AnimationNameOffset;
		unsigned int StringCharacter = 0; // Current character in string.
		string ObjectNameRaw;
		bool Delimiter = false;

		// Get characters until \x00 is hit.
		while (Delimiter == false)
		{
			// If not null.
			if ( MTPFileData[AnimationNameOffset + StringCharacter] != 0x00 )
			{
				ObjectNameRaw += (char)MTPFileData[AnimationNameOffset + StringCharacter];
			}
			else
			{
				Delimiter = true;
			}

			// Go to next char.
			StringCharacter++;
		}

		TemporaryShadowObject -> AnimationName = ObjectNameRaw + ".anm";
		cout << "Animation Name: " << TemporaryShadowObject -> AnimationName << endl;

		// Add the new object data to the list of objects in vector.
		// Must point to pointer of this object.
		ShadowObjects.push_back( *(ShadowANMObject *)TemporaryShadowObject );
		// Seek to next animation's base address.
		CurrentAnimationLocation += 0xC;
		x++; // Let's go, to the next one, my friend.
	}

	/*
		Alright, we have all the info we need, on every object, now let's extract the objects :3
	*/

	cout << "\n--------------\nWriting Files:\n" << endl;
	for (auto &ShadowANMObject : ShadowObjects)
	{
		// DEBUG //
		string TemporaryFilePath = OutputFile + "/" + ShadowANMObject.AnimationName;
		cout << TemporaryFilePath << endl;

		// Creates an outputstream for the ANM file
		ofstream ANMFile;
		// Opens the outputstream for binary writing
		ANMFile.open(TemporaryFilePath,ios::binary);

		/* If the file is successfully opened and created */
		if (ANMFile.is_open())
		{
			// Gets our first byte address for file.
			unsigned int DataOffset = ShadowANMObject.AnimationDataOffset;
			// Gets our last byte address for file.
			unsigned int DataOffsetEnd = DataOffset + ShadowANMObject.AnimationSize;

			/* Copy relevant data byte by byte */
			/*
				There's probably a better, faster way to copy the data range, I'm just worried
				about preserving Big Endian though, as I'm still relatively new to C++.
			*/
			for (unsigned int x = DataOffset; x < DataOffsetEnd;)
			{
				// Push the current byte to file.
				ANMFile << MTPFileData[x];
				// Add a byte to loop.
				x++;
			}
		}
		else { cout << "Couldn't generate/write new file, do you have write permissions in your chosen directory?" << endl; }

		ANMFile.close();

		/* Get the property data range */
	}


}

int main(int argc, char ** argv)
{
	string InputFile; //Input file
	string OutputFile; //Output file
	int Action = 0; // Extract or compile?

	// Identify the command line passed arguments.
	for(int i = 1; i < argc; i++)
	{
		if (strcmp (argv[i], "--extract") == 0) { Action = 1; }
		if (strcmp (argv[i], "--compile") == 0) { Action = 2; }
		if (strcmp (argv[i], "-i") == 0) { InputFile = argv[i+1]; cout << "Input File/Folder: " << InputFile << endl; }
		if (strcmp (argv[i], "-o") == 0) { OutputFile = argv[i+1]; cout << "Output File/Folder: " << OutputFile << endl; }
	}

	if (Action == 1) { MTPToANM(InputFile, OutputFile); }
	else if (Action == 2) { } // Do Nothing
	else
	{
		cout << "\n\nYou have not specified an action. Try running with parameters:" << endl;
		cout << "--extract -i <MTPFile> -o <ANMFolder>" << endl;
	}
	return 0;
}
