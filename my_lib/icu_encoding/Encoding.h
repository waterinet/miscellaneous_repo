#pragma once
#include "unicode/utypes.h"
#include "unicode/ucnv.h"
#include "EncodingError.h"

namespace icu_encoding
{
	typedef UChar wchar;
	class Encoding
	{
	public:
		virtual ~Encoding() { /*this will be called*/ };
		static void convert(Encoding*, Encoding*, std::string, std::string&, EncodingError&);
		virtual std::string get_name() = 0;
		virtual int get_max_char_size() = 0;
		virtual int get_min_char_size() = 0;
		virtual int get_max_byte_count(int wchar_count) = 0;
		virtual int from_unicode(const wchar*, int, char*, int, EncodingError&) = 0;
		virtual int to_unicode(const char*, int, wchar*, int, EncodingError&) = 0;
	};
}


