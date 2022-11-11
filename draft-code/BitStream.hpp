#ifndef BITSTREAM_H
#define BITSTREAM_H

#include <iostream>
#include <fstream>
#include <string>
#include <stdlib.h>
#include <map>

using namespace std;

/**
 * Class to handle reading and writing bit-wise on a file.
 * Stream-based I/O is used. Class fstream.
 */
class BitStream
{
private:
	fstream file;
	int mode;			  // write = 0 / read = 1
	unsigned char buffer; // buffer[7] Msb , ... , buffer[0] Lsb
	int pointer;		  // goes from 0 to 7
	// pointer in 'r' mode points to last read bit
	// pointer in 'w' mode points to last written bit
	int size; // file size in bytes

public:
	/**
	 * Empty BitStream Class Constructor
	 */
	BitStream();

	/**
	 * BitStream Class Constructor
	 * @param filename path to a file where to read or write.
	 * @param mode character 'w' for writing in a file and 'r' for reading in a file.
	 */
	BitStream(const char *filename, char mode);

	/**
	 * Reads 1 bit from file
	 * @return unsigned char with the bit read from the file, either '0' or '1'.
	 */
	unsigned char readBit();

	/**
	 * Write 1 bit in the file
	 * @param bit char with the bit to write. Bit written will be the least significant bit in byte represented by the char
	 */
	void writeBit(char bit);

	/**
	 * Reads n bits from file
	 * @param bits where the bits will be read to.
	 * @param n number of bits to read.
	 */
	void readNbits(char *bits, int n);

	/**
	 * Writes n bits in the file
	 * @param bits bits to be written.
	 * @param n number of bits to be written.
	 */
	void writeNbits(char *bits, int n);

	/**
	 * Returns true if stream reached end of file, false if not.
	 * @return boolean
	 */
	bool eof();

	/**
	 * Closes file stream. If 'w' mode, last byte is written.
	 */
	void close();
};

#endif