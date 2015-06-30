#pragma once
#include "Encoding.h"
namespace icu_encoding
{
	class GBKEncoding : public Encoding
	{
	public:
		GBKEncoding();
		~GBKEncoding();
		std::string get_name() override;
		int get_max_char_size() override;
		int get_min_char_size() override;
		int get_max_byte_count(int wchar_count) override;
		int from_unicode(const wchar*, int, char*, int, EncodingError&) override;
		int to_unicode(const char*, int, wchar*, int, EncodingError&) override;
	private:
		void reset();
	private:
		UConverter* cnv_;
	};
}


