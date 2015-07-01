#ifndef ENDIAN_H
#define ENDIAN_H
/**
@file Endian.h

A file with basic endian conversion routines.

@copyright
@copyright (c) 2014 Richard Damon
@parblock
MIT License:

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
@endparblock

*/

#include <stdint.h>
/**
A namespace to isolate these functions.
*/
namespace Endian {
	/**
	* endianTest
	* Returns 1 if we are little endian, else returns 0
	*/
	inline static int endianTest() {
		int16_t i = 1;
		return *(char *)&i;
	}
	/**
	Get 16 Bit Little Endian Value.

	@param buffer Buffer to fetch the value from.
	@returns 16 bit signed integer from buffer.
	*/
	inline int16_t get16LE(void const* buffer) {
		unsigned char const* buf = static_cast<unsigned char const*>(buffer);
		return (buf[1] << 8) + buf[0];
	}

	/**
	Get 16 Bit Little Endian Value.

	@param buffer Buffer to fetch the value from.
	@returns 16 bit unsigned integer from buffer.
	*/
	inline uint16_t getU16LE(void const* buffer) {
		unsigned char const* buf = static_cast<unsigned char const*>(buffer);
		return (buf[1] << 8) + buf[0];
	}
	/**
	Get 32 Bit Little Endian Value.

	@param buffer Buffer to fetch the value from.
	@returns 32 bit signed integer from buffer.
	*/

	inline int32_t get32LE(void const* buffer) {
		unsigned char const* buf = static_cast<unsigned char const*>(buffer);
		return (buf[3] << 24) + (buf[2] << 16) + (buf[1] << 8) + buf[0];
	}

	/**
	Get 32 Bit Little Endian Value.

	@param buffer Buffer to fetch the value from.
	@returns 32 bit unsigned integer from buffer.
	*/
	inline uint32_t getU32LE(void const* buffer) {
		unsigned char const* buf = static_cast<unsigned char const*>(buffer);
		return (buf[3] << 24) + (buf[2] << 16) + (buf[1] << 8) + buf[0];
	}

	/**
	Get Single Presision Floating Little Endian Value.

	This assume that the floating point is stored in same basic format, with possible endian swap.

	@param buffer Buffer to fetch the value from.
	@returns float from buffer.
	*/
	inline float getFloatLE(void const* buffer) {
		union {
			char b[4];
			float f;
		} u;
		unsigned char const* buf = static_cast<unsigned char const*>(buffer);
		if (endianTest()) {
			for (int i = 0; i < 4; i++) {
				u.b[i] = buf[i];
			}
		} else {
			for (int i = 0; i < 4; i++) {
				u.b[3-i] = buf[i];
			}

		}
		return u.f;
	}

	/**
	Get Double Presision Floating Little Endian Value.

	This assume that the floating point is stored in same basic format, with possible endian swap.

	@param buffer Buffer to fetch the value from.
	@returns double from buffer.
	*/
	inline double getDoubleLE(void const* buffer) {
		union {
			char b[8];
			double f;
		} u;
		unsigned char const* buf = static_cast<unsigned char const*>(buffer);
		if (endianTest()) {
			for (int i = 0; i < 8; i++) {
				u.b[i] = buf[i];
			}
		} else {
			for (int i = 0; i < 8; i++) {
				u.b[7 - i] = buf[i];
			}

		}
		return u.f;
	}

	/**
	Get 16 Bit Big Endian Value.

	@param buffer Buffer to fetch the value from.
	@returns 16 bit signed integer from buffer.
	*/
	inline int16_t get16BE(void const* buffer) {
		unsigned char const* buf = static_cast<unsigned char const*>(buffer);
		return (buf[0] << 8) + buf[1];
	}

	/**
	Get 16 Bit Big Endian Value.

	@param buffer Buffer to fetch the value from.
	@returns 16 bit unsigned integer from buffer.
	*/
	inline uint16_t getU16BE(void const* buffer) {
		unsigned char const* buf = static_cast<unsigned char const*>(buffer);
		return (buf[0] << 8) + buf[1];
	}

	/**
	Get 32 Bit Big Endian Value.

	@param buffer Buffer to fetch the value from.
	@returns 32 bit signed integer from buffer.
	*/
	inline int32_t get32BE(void const* buffer) {
		unsigned char const* buf = static_cast<unsigned char const*>(buffer);
		return (buf[0] << 24) + (buf[1] << 16) + (buf[2] << 8) + buf[3];
	}

	/**
	Get 32 Bit Big Endian Value.

	@param buffer Buffer to fetch the value from.
	@returns 32 bit unsigned integer from buffer.
	*/
	inline uint32_t getU32BE(void const* buffer) {
		unsigned char const* buf = static_cast<unsigned char const*>(buffer);
		return (buf[0] << 24) + (buf[1] << 16) + (buf[2] << 8) + buf[3];
	}

	/**
	Get Single Presision Floating Big Endian Value.

	This assume that the floating point is stored in same basic format, with possible endian swap.

	@param buffer Buffer to fetch the value from.
	@returns float from buffer.
	*/
	inline float getFloatBE(void const* buffer) {
		union {
			char b[4];
			float f;
		} u;
		unsigned char const* buf = static_cast<unsigned char const*>(buffer);
		if (endianTest()) {
			for (int i = 0; i < 4; i++) {
				u.b[3 - i] = buf[i];
			}
		} else {
			for (int i = 0; i < 4; i++) {
				u.b[i] = buf[i];
			}

		}
		return u.f;

	}

	/**
	Get Double Presision Floating Big Endian Value.

	This assume that the floating point is stored in same basic format, with possible endian swap.

	@param buffer Buffer to fetch the value from.
	@returns double from buffer.
	*/
	inline double getDouble(void const* buffer) {
		union {
			char b[8];
			double f;
		} u;
		unsigned char const* buf = static_cast<unsigned char const*>(buffer);
		if (endianTest()) {
			for (int i = 0; i < 8; i++) {
				u.b[7 - i] = buf[i];
			}
		} else {
			for (int i = 0; i < 8; i++) {
				u.b[i] = buf[i];
			}

		}
		return u.f;
	}
};

#endif