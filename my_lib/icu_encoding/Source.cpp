#include <iostream>
#include "ICU_Encoding.h"
using namespace std;
using namespace icu_encoding;

void main()
{
	string gbk_str("一个中国妻子@USA");
	string utf8_str;
	GBKEncoding gbk;
	UTF8Encoding utf8;
	EncodingError err;
	cout << "gbk string: " << gbk_str << "\n";
	Encoding::convert(&gbk, &utf8, gbk_str, utf8_str, err);
	if (err) {
		cout << "fail to convert\n";
		return;
	}
	cout << "utf-8 string: " << utf8_str << "\n";
	Encoding::convert(&utf8, &gbk, utf8_str, gbk_str, err);
	if (err) {
		cout << "fail to convert\n";
		return;
	}
	cout << "gbk string: " << gbk_str << "\n";
}