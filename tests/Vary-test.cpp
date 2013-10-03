#include <gtest/gtest.h>
#include <x0/http/Vary.h>
#include <vector>

using namespace x0;

typedef HttpHeader<std::string> Header;

void dump(const Vary& vary)
{
	printf("Vary fields (%zu):\n", vary.size());
	for (size_t i = 0, e = vary.size(); i != e; ++i) {
		printf("%20s: %s\n", vary.names()[i].c_str(),
				vary.values()[i].c_str());
	}
}

TEST(VaryTest, Empty)
{
	Vary vary(0);
	ASSERT_EQ(0, vary.size());
}

auto requestHeaders() -> const std::vector<Header>& {
	static const std::vector<Header> headers = {
		{ "Accept-Encoding", "gzip" },
		{ "X-Test", "42" },
		{ "User-Agent", "gtest" },
	};
	return headers;
}

TEST(VaryTest, Create1)
{
	auto vary = Vary::create("Accept-Encoding", requestHeaders());

	ASSERT_EQ(1, vary->size());
	ASSERT_EQ("Accept-Encoding", vary->names()[0]);
	ASSERT_EQ("gzip", vary->values()[0]);
}

TEST(VaryTest, Create2)
{
	auto vary = Vary::create("Accept-Encoding,User-Agent", requestHeaders());

	ASSERT_EQ(2, vary->size());

	ASSERT_EQ("Accept-Encoding", vary->names()[0]);
	ASSERT_EQ("gzip", vary->values()[0]);

	ASSERT_EQ("User-Agent", vary->names()[1]);
	ASSERT_EQ("gtest", vary->values()[1]);
}

TEST(VaryTest, Foreach0)
{
	auto vary = Vary::create("", requestHeaders());
	dump(*vary);
	auto i = vary->begin();
	auto e = vary->end();
	ASSERT_EQ(true, i == e);
}

TEST(VaryTest, Foreach2)
{
	auto vary = Vary::create("Accept-Encoding,User-Agent", requestHeaders());

	auto i = vary->begin();
	auto e = vary->end();
	ASSERT_EQ("Accept-Encoding", i.name());
	ASSERT_EQ("gzip", i.value());

	++i;
	ASSERT_EQ("User-Agent", i.name());
	ASSERT_EQ("gtest", i.value());

	++i;
	ASSERT_EQ(true, i == e);
}