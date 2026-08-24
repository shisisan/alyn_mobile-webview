#include "include/encoding.h"

#include <spdlog/spdlog.h>
#include <iconv.h>
#include <stdexcept>

const char* charset = "CP1251"; // CP1251 | GBK

std::string Encoding::cp2utf(const std::string& s)
{
	iconv_t cd = iconv_open("UTF-8", charset);
	if (cd == (iconv_t) -1) {
		spdlog::error("iconv_open failed: {}", strerror(errno));
		return s;
	}

	std::string in = s;
	std::string out;
	out.resize(in.size() * 4);
	char* inbuf = const_cast<char*>(in.data());
	char* outbuf = const_cast<char*>(out.data());
	size_t inbytesleft = in.size();
	size_t outbytesleft = out.size();
	size_t res = iconv(cd, &inbuf, &inbytesleft, &outbuf, &outbytesleft);
	iconv_close(cd);
	if (res == (size_t) -1) {
		spdlog::error("iconv failed: {}", strerror(errno));
		return s;
	}
	out.resize(out.size() - outbytesleft);
	return out;
}
