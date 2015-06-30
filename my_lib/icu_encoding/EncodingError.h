#pragma once
#include "unicode/utypes.h"
namespace icu_encoding
{
	class EncodingError
	{
	public:
		EncodingError(int=0);
		~EncodingError();
		operator bool() const;
		bool operator!() const;
	private:
		int err_val;
	};
}


