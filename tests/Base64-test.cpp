#include <gtest/gtest.h>
#include <x0/Buffer.h>
#include <x0/Base64.h>
#include <cstdio>

using namespace x0;

// {{{ debug helper
void print(const Buffer& b, const char *msg = 0)
{
	if (msg && *msg)
		printf("\nbuffer(%s): '%s'\n", msg, b.str().c_str());
	else
		printf("\nbuffer: '%s'\n", b.str().c_str());
}

void print(const BufferRef& v, const char *msg = 0)
{
	char prefix[64];
	if (msg && *msg)
		snprintf(prefix, sizeof(prefix), "\nbuffer.view(%s)", msg);
	else
		snprintf(prefix, sizeof(prefix), "\nbuffer.view");

	if (v)
		printf("\n%s: '%s' (size=%ld)\n", prefix, v.str().c_str(), v.size());
	else
		printf("\n%s: NULL\n", prefix);
}

bool testDecode(const Buffer& decoded, const Buffer& encoded) {
	auto ref = encoded.ref();
	Buffer result = Base64::decode(ref);
	return equals(result, decoded);
}
//}}}

TEST(Base64Test, encode)
{
	ASSERT_EQ("YQ==", Base64::encode(std::string("a")));
	ASSERT_EQ("YWI=", Base64::encode(std::string("ab")));
	ASSERT_EQ("YWJj", Base64::encode(std::string("abc")));
	ASSERT_EQ("YWJjZA==", Base64::encode(std::string("abcd")));
	ASSERT_EQ("Zm9vOmJhcg==", Base64::encode(std::string("foo:bar")));
}

TEST(Base64Test, decode)
{
	ASSERT_EQ(true, testDecode("a",    "YQ=="));
	ASSERT_EQ(true, testDecode("ab",   "YWI="));
	ASSERT_EQ(true, testDecode("abc",  "YWJj"));
	ASSERT_EQ(true, testDecode("abcd", "YWJjZA=="));
	ASSERT_EQ(true, testDecode("foo:bar", "Zm9vOmJhcg=="));
}
