#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>
#include <boost/test/unit_test.hpp>

#include "util.h"

BOOST_AUTO_TEST_SUITE(getarg_tests)

static void
ResetArgs(const std::string& strArg)
{
    std::vector<std::string> vecArg;
    boost::split(vecArg, strArg, boost::is_space(), boost::token_compress_on);

    // Insert dummy executable name:
    vecArg.insert(vecArg.begin(), "testbitcoin");

    // Convert to char*:
    std::vector<const char*> vecChar;
    BOOST_FOREACH(std::string& s, vecArg)
        vecChar.push_back(s.c_str());

    ParseParameters(vecChar.size(), &vecChar[0]);
}

BOOST_AUTO_TEST_CASE(boolarg)
{
    ResetArgs("-h4h");
    BOOST_CHECK(GetBoolArg("-h4h"));
    BOOST_CHECK(GetBoolArg("-h4h", false));
    BOOST_CHECK(GetBoolArg("-h4h", true));

    BOOST_CHECK(!GetBoolArg("-fo"));
    BOOST_CHECK(!GetBoolArg("-fo", false));
    BOOST_CHECK(GetBoolArg("-fo", true));

    BOOST_CHECK(!GetBoolArg("-h4ho"));
    BOOST_CHECK(!GetBoolArg("-h4ho", false));
    BOOST_CHECK(GetBoolArg("-h4ho", true));

    ResetArgs("-h4h=0");
    BOOST_CHECK(!GetBoolArg("-h4h"));
    BOOST_CHECK(!GetBoolArg("-h4h", false));
    BOOST_CHECK(!GetBoolArg("-h4h", true));

    ResetArgs("-h4h=1");
    BOOST_CHECK(GetBoolArg("-h4h"));
    BOOST_CHECK(GetBoolArg("-h4h", false));
    BOOST_CHECK(GetBoolArg("-h4h", true));

    // New 0.6 feature: auto-map -nosomething to !-something:
    ResetArgs("-noh4h");
    BOOST_CHECK(!GetBoolArg("-h4h"));
    BOOST_CHECK(!GetBoolArg("-h4h", false));
    BOOST_CHECK(!GetBoolArg("-h4h", true));

    ResetArgs("-noh4h=1");
    BOOST_CHECK(!GetBoolArg("-h4h"));
    BOOST_CHECK(!GetBoolArg("-h4h", false));
    BOOST_CHECK(!GetBoolArg("-h4h", true));

    ResetArgs("-h4h -noh4h");  // -h4h should win
    BOOST_CHECK(GetBoolArg("-h4h"));
    BOOST_CHECK(GetBoolArg("-h4h", false));
    BOOST_CHECK(GetBoolArg("-h4h", true));

    ResetArgs("-h4h=1 -noh4h=1");  // -h4h should win
    BOOST_CHECK(GetBoolArg("-h4h"));
    BOOST_CHECK(GetBoolArg("-h4h", false));
    BOOST_CHECK(GetBoolArg("-h4h", true));

    ResetArgs("-h4h=0 -noh4h=0");  // -h4h should win
    BOOST_CHECK(!GetBoolArg("-h4h"));
    BOOST_CHECK(!GetBoolArg("-h4h", false));
    BOOST_CHECK(!GetBoolArg("-h4h", true));

    // New 0.6 feature: treat -- same as -:
    ResetArgs("--h4h=1");
    BOOST_CHECK(GetBoolArg("-h4h"));
    BOOST_CHECK(GetBoolArg("-h4h", false));
    BOOST_CHECK(GetBoolArg("-h4h", true));

    ResetArgs("--noh4h=1");
    BOOST_CHECK(!GetBoolArg("-h4h"));
    BOOST_CHECK(!GetBoolArg("-h4h", false));
    BOOST_CHECK(!GetBoolArg("-h4h", true));

}

BOOST_AUTO_TEST_CASE(stringarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-h4h", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-h4h", "eleven"), "eleven");

    ResetArgs("-h4h -bar");
    BOOST_CHECK_EQUAL(GetArg("-h4h", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-h4h", "eleven"), "");

    ResetArgs("-h4h=");
    BOOST_CHECK_EQUAL(GetArg("-h4h", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-h4h", "eleven"), "");

    ResetArgs("-h4h=11");
    BOOST_CHECK_EQUAL(GetArg("-h4h", ""), "11");
    BOOST_CHECK_EQUAL(GetArg("-h4h", "eleven"), "11");

    ResetArgs("-h4h=eleven");
    BOOST_CHECK_EQUAL(GetArg("-h4h", ""), "eleven");
    BOOST_CHECK_EQUAL(GetArg("-h4h", "eleven"), "eleven");

}

BOOST_AUTO_TEST_CASE(intarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-h4h", 11), 11);
    BOOST_CHECK_EQUAL(GetArg("-h4h", 0), 0);

    ResetArgs("-h4h -bar");
    BOOST_CHECK_EQUAL(GetArg("-h4h", 11), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);

    ResetArgs("-h4h=11 -bar=12");
    BOOST_CHECK_EQUAL(GetArg("-h4h", 0), 11);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 12);

    ResetArgs("-h4h=NaN -bar=NotANumber");
    BOOST_CHECK_EQUAL(GetArg("-h4h", 1), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);
}

BOOST_AUTO_TEST_CASE(doubledash)
{
    ResetArgs("--h4h");
    BOOST_CHECK_EQUAL(GetBoolArg("-h4h"), true);

    ResetArgs("--h4h=verbose --bar=1");
    BOOST_CHECK_EQUAL(GetArg("-h4h", ""), "verbose");
    BOOST_CHECK_EQUAL(GetArg("-bar", 0), 1);
}

BOOST_AUTO_TEST_CASE(boolargno)
{
    ResetArgs("-noh4h");
    BOOST_CHECK(!GetBoolArg("-h4h"));
    BOOST_CHECK(!GetBoolArg("-h4h", true));
    BOOST_CHECK(!GetBoolArg("-h4h", false));

    ResetArgs("-noh4h=1");
    BOOST_CHECK(!GetBoolArg("-h4h"));
    BOOST_CHECK(!GetBoolArg("-h4h", true));
    BOOST_CHECK(!GetBoolArg("-h4h", false));

    ResetArgs("-noh4h=0");
    BOOST_CHECK(GetBoolArg("-h4h"));
    BOOST_CHECK(GetBoolArg("-h4h", true));
    BOOST_CHECK(GetBoolArg("-h4h", false));

    ResetArgs("-h4h --noh4h");
    BOOST_CHECK(GetBoolArg("-h4h"));

    ResetArgs("-noh4h -h4h"); // h4h always wins:
    BOOST_CHECK(GetBoolArg("-h4h"));
}

BOOST_AUTO_TEST_SUITE_END()
