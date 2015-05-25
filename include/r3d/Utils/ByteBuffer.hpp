/*
	Copyright (C) 2012 Alexander Overvoorde

	Permission is hereby granted, free of charge, to any person obtaining a copy of
	this software and associated documentation files (the "Software"), to deal in
	the Software without restriction, including without limitation the rights to
	use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
	the Software, and to permit persons to whom the Software is furnished to do so,
	subject to the following conditions:

	The above copyright notice and this permission notice shall be included in all
	copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
	FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
	COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
	IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
	CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE
*/

#pragma once

#ifndef __R3D_UTILS_BYTEBUFFER_HPP
#define __R3D_UTILS_BYTEBUFFER_HPP

#include <vector>
#include <climits>
#include <cstdint>

namespace r3d
{
	/*
		Byte reader
	*/
	class ByteReader
	{
	public:
		ByteReader( uint32_t length, bool littleEndian ) : buffer( new uint8_t[length] ), length( length ), ptr( 0 ), littleEndian( littleEndian ) {}
		~ByteReader() { delete [] buffer; }

		void Reuse( uint32_t length )
		{
			delete [] buffer;
			buffer = new uint8_t[length];
			this->length = length;
			ptr = 0;
		}

		uint8_t* Data() { return buffer; }
		uint32_t Length() { return length; }

		void Advance( uint32_t count ) { ptr += count; }
		void Move( uint32_t location ) { ptr = location; }

		bool Compare( uint32_t location, uint32_t length, const uint8_t* data )
		{
			if ( location + length > this->length ) return false;
			
			for ( uint32_t i = 0; i < length; i++ )
				if ( buffer[ location + i ] != data[i] ) return false;
			
			return true;
		}

		uint8_t PeekByte( uint32_t offset = 0 ) { return buffer[ptr+offset]; }

		uint8_t ReadUbyte()
		{
			uint8_t val = buffer[ptr];
			ptr++;
			return val;
		}

		uint16_t ReadUshort()
		{
			uint16_t val;
			if ( littleEndian ) val = buffer[ptr+0] + ( buffer[ptr+1] << 8 );
			else val = buffer[ptr+1] + ( buffer[ptr+0] << 8 );
			ptr += 2;
			return val;
		}

		uint32_t ReadUint()
		{
			uint32_t val;
			if ( littleEndian ) val = buffer[ptr+0] + ( buffer[ptr+1] << 8 ) + ( buffer[ptr+2] << 16 ) + ( buffer[ptr+3] << 24 );
			else val = buffer[ptr+3] + ( buffer[ptr+2] << 8 ) + ( buffer[ptr+1] << 16 ) + ( buffer[ptr+0] << 24 );
			ptr += 4;
			return val;
		}

		int ReadInt()
		{
			uint32_t val = ReadUint();
			if ( val <= INT_MAX )
				return val;
			else
				return INT_MIN + val - INT_MAX - 1;
		}

		void Read( uint8_t* dest, uint32_t length )
		{
			for ( uint32_t i = 0; i < length; i++ )
				dest[i] = buffer[ptr+i];
			ptr += length;
		}

	private:
		uint8_t* buffer;
		uint32_t length;
		uint32_t ptr;
		bool littleEndian;

		ByteReader( const ByteReader& );
		const ByteReader& operator=( const ByteReader& );
	};

	/*
		Byte writer
	*/
	class ByteWriter
	{
	public:
		ByteWriter( bool littleEndian ) : littleEndian( littleEndian ) {};

		uint8_t* Data() { return &buffer[0]; }
		uint32_t Length() { return buffer.size(); }

		void Pad( uint32_t count )
		{
			for ( uint32_t i = 0; i < count; i++ )
				buffer.push_back( 0 );
		}

		void WriteUbyte( uint8_t val )
		{
			buffer.push_back( val );
		}
	
		void WriteUshort( uint16_t val )
		{
			if ( littleEndian ) {
				buffer.push_back( val & 0x00FF );
				buffer.push_back( ( val & 0xFF00 ) >> 8 );
			} else {
				buffer.push_back( ( val & 0xFF00 ) >> 8 );
				buffer.push_back( val & 0x00FF );
			}
		}

		void WriteUint( uint32_t val )
		{
			if ( littleEndian ) {
				buffer.push_back( (uint8_t)( val & 0x000000FF ) );
				buffer.push_back( (uint8_t)( ( val & 0x0000FF00 ) >> 8 ) );
				buffer.push_back( (uint8_t)( ( val & 0x00FF0000 ) >> 16 ) );
				buffer.push_back( (uint8_t)( ( val & 0xFF000000 ) >> 24 ) );
			} else {
				buffer.push_back( (uint8_t)( ( val & 0xFF000000 ) >> 24 ) );
				buffer.push_back( (uint8_t)( ( val & 0x00FF0000 ) >> 16 ) );
				buffer.push_back( (uint8_t)( ( val & 0x0000FF00 ) >> 8 ) );
				buffer.push_back( (uint8_t)( val & 0x000000FF ) );
			}
		}

		void WriteString( const char* str )
		{
			while ( *str != 0 )
				buffer.push_back( *(str++) );
			buffer.push_back( 0 );
		}

	private:
		std::vector<uint8_t> buffer;
		bool littleEndian;

		ByteWriter( const ByteWriter& );
		const ByteWriter& operator=( const ByteWriter& );
	};
}

#endif