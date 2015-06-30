#include "EncodingError.h"

using namespace icu_encoding;

EncodingError::EncodingError(int val) : err_val(val)
{
}


EncodingError::~EncodingError()
{
}

EncodingError::operator bool() const
{
	//error if err_val > 0
	return err_val > 0;
}

bool EncodingError::operator!() const
{
	return err_val <= 0;
}
