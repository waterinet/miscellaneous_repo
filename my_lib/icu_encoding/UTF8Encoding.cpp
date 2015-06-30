#include "UTF8Encoding.h"
#include <exception>

using namespace std;
using namespace icu_encoding;

UTF8Encoding::UTF8Encoding()
{
	UErrorCode ec = U_ZERO_ERROR;
	cnv_ = ucnv_open("utf-8", &ec);
	if (U_FAILURE(ec))
		throw std::exception("fail to open converter");
}

UTF8Encoding::~UTF8Encoding()
{
	if (cnv_) {
		ucnv_close(cnv_);
	}
}

string UTF8Encoding::get_name()
{
	UErrorCode ec = U_ZERO_ERROR;
	return ucnv_getName(cnv_, &ec);
}

int UTF8Encoding::get_max_char_size()
{
	return ucnv_getMaxCharSize(cnv_);
}

int UTF8Encoding::get_min_char_size()
{
	return ucnv_getMinCharSize(cnv_);
}

int UTF8Encoding::get_max_byte_count(int wchar_count)
{
	return UCNV_GET_MAX_BYTES_FOR_STRING(wchar_count, get_max_char_size());
}

int UTF8Encoding::from_unicode(const wchar* src, int src_size, char* dst, int dst_size, EncodingError& err)
{
	UErrorCode ec = U_ZERO_ERROR;
	int dst_length = ucnv_fromUChars(cnv_, dst, dst_size, src, src_size, &ec);
	err = EncodingError(ec);
	if (err) {
		reset();
	}
	return dst_length;
}

int UTF8Encoding::to_unicode(const char* src, int src_size, wchar* dst, int dst_size, EncodingError& err)
{
	UErrorCode ec = U_ZERO_ERROR;
	int dst_length = ucnv_toUChars(cnv_, dst, dst_size, src, src_size, &ec);
	err = EncodingError(ec);
	if (err) {
		reset();
	}
	return dst_length;
}

void UTF8Encoding::reset()
{
	ucnv_reset(cnv_);
}
