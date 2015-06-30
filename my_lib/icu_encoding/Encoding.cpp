#include "Encoding.h"
#include <exception>
#include "boost/shared_array.hpp"

using namespace std;
using namespace icu_encoding;

void Encoding::convert(Encoding* p_src, Encoding* p_dst, string src, string& dst, EncodingError& err)
{
	if (p_src == nullptr || p_dst == nullptr) {
		err = EncodingError(U_ILLEGAL_ARGUMENT_ERROR);
		return;
	}
	if (src.empty()) {
		dst = "";
		err = EncodingError(U_ZERO_ERROR);
		return;
	}
	int max_src_char_count = src.length() / p_src->get_min_char_size();
	int wbuf_size = max_src_char_count * sizeof(wchar);
	boost::shared_array<wchar> wbuf;
	try {
		wbuf = boost::shared_array<wchar>(new wchar[wbuf_size]);
	}
	catch (std::bad_alloc) {
		err = EncodingError(U_MEMORY_ALLOCATION_ERROR);
		return;
	}
	int wchar_count = p_src->to_unicode(src.c_str(), src.length(), wbuf.get(), wbuf_size, err);
	if (err) {
		return;
	}
	int buf_size = p_dst->get_max_byte_count(wchar_count);
	boost::shared_array<char> buf;
	try {
		buf = boost::shared_array<char>(new char[buf_size]);
	}
	catch (std::bad_alloc) {
		err = EncodingError(U_MEMORY_ALLOCATION_ERROR);
		return;
	}
	p_dst->from_unicode(wbuf.get(), wchar_count, buf.get(), buf_size, err);
	if (err) {
		return;
	}
	dst = string(buf.get());
}