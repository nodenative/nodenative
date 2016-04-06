#include <native/UriTemplate.hpp>
#include <native/UriTemplateFormat.hpp>
#include <native/UriTemplateValue.hpp>

#include <gtest/gtest.h>

using namespace native;

namespace
{
    void initGlobalFormats()
    {
        static bool initiated = false;
        if(initiated)
        {
            return;
        }

        initiated = true;
        UriTemplateFormat::AddGlobalFormat("formatName1", "(?:[A-Z0-9\\-]{2}|[A-Z]{3})");
        UriTemplateFormat::AddGlobalFormat("FormatName2", "[0-9]{1,4}[A-Z]?");
        UriTemplateFormat::AddGlobalFormat("FormatName3", "[0-9]{2}[A-Z]{3}[0-9]{2}");
        UriTemplateFormat::AddGlobalFormat("FormatComplex", "{format1:formatName1}_{number:FormatName2}/{date:FormatName3}");
        UriTemplateFormat::AddGlobalFormat("FormatName6", "[A-Z]{3}");
        UriTemplateFormat::AddGlobalFormat("FormatName4", "{complex:FormatComplex}/someText/{format5:FormatName6}");
    }

    void checkUriTemplate(
            const std::string pattern,
            const std::string matchPattern,
            const std::string extractionPattern)
    {
        EXPECT_NO_THROW(const UriTemplate uriTemplate(pattern));
        const std::string matchPatternWithEndAnchor(matchPattern+"$");
        const std::string extractionPatternWithEndAnchor(extractionPattern+"$");
        const UriTemplate uriTemplate(pattern);

        EXPECT_STREQ(uriTemplate.getTemplate().c_str(), pattern.c_str());
        EXPECT_STREQ(uriTemplate.getPattern().c_str(), matchPatternWithEndAnchor.c_str());
        EXPECT_STREQ(uriTemplate.getPattern(false, false).c_str(), matchPattern.c_str());
        EXPECT_STREQ(uriTemplate.getPattern(true).c_str(), extractionPatternWithEndAnchor.c_str());
        EXPECT_STREQ(uriTemplate.getPattern(true, false).c_str(), extractionPattern.c_str());
    }
} /* namespace */

TEST(UriTemplateTest, SimpleText)
{
    checkUriTemplate("simpleText", "simpleText", "simpleText");
}

TEST(UriTemplateTest, SimpleRegExp)
{
    checkUriTemplate("simpleText[A-Z]+", "simpleText[A-Z]+", "simpleText[A-Z]+");
}

TEST(UriTemplateTest, NoneCapturingRegExp)
{
    checkUriTemplate("simpleText(?:[A-Z]+)", "simpleText(?:[A-Z]+)", "simpleText(?:[A-Z]+)");
}

TEST(UriTemplateTest, ExceptionOnCapturingRegExp)
{
    EXPECT_ANY_THROW(const UriTemplate uriTemplate("simpleText([A-Z]+)"));
}

TEST(UriTemplateTest, UriTemplate)
{
    initGlobalFormats();
    checkUriTemplate("someText{par1:formatName1}", "someText(?:[A-Z0-9\\-]{2}|[A-Z]{3})", "someText((?:[A-Z0-9\\-]{2}|[A-Z]{3}))");
}

TEST(UriTemplateTest, UriTemplate2)
{
    initGlobalFormats();
    checkUriTemplate(
        "someText/{par1:FormatName4}",
        "someText/(?:[A-Z0-9\\-]{2}|[A-Z]{3})_[0-9]{1,4}[A-Z]?/[0-9]{2}[A-Z]{3}[0-9]{2}/someText/[A-Z]{3}",
        "someText/((((?:[A-Z0-9\\-]{2}|[A-Z]{3}))_([0-9]{1,4}[A-Z]?)/([0-9]{2}[A-Z]{3}[0-9]{2}))/someText/([A-Z]{3}))");
}

TEST(UriTemplateTest, NoFormatNames)
{
    initGlobalFormats();
    const UriTemplate uriTemplate("someText");
    EXPECT_EQ(int(uriTemplate.getFormatNames().size()), 0);
    UriTemplateValue extractedValues;
    EXPECT_NO_THROW(uriTemplate.extract(extractedValues, "someText"));
    EXPECT_EQ(int(extractedValues.size()), 0);
}

TEST(UriTemplateTest, ExtractedValueSimple)
{
    initGlobalFormats();
    const UriTemplate uriTemplate("someText/{par1:formatName1}");
    EXPECT_EQ(int(uriTemplate.getFormatNames().size()), 1);
    UriTemplateValue extractedValues;
    EXPECT_EQ(uriTemplate.extract(extractedValues, "someText/AB"), true);
    EXPECT_EQ(int(extractedValues.size()), 1);
    EXPECT_STREQ(extractedValues["par1"].getString().c_str(), "AB");
    EXPECT_EQ(int(extractedValues["par1"].size()), 0);
}

TEST(UriTemplateTest, ExtractNotMatchIfExistsUnmatchedPreffix)
{
    initGlobalFormats();
    const UriTemplate uriTemplate("someText/{par1:formatName1}");
    EXPECT_EQ(int(uriTemplate.getFormatNames().size()), 1);
    UriTemplateValue extractedValues;
    EXPECT_EQ(uriTemplate.extract(extractedValues, "1someText/AB"), false);
}

TEST(UriTemplateTest, ExtractNotMatchIfExistsUnmatchedSuffix)
{
    initGlobalFormats();
    const UriTemplate uriTemplate("someText/{par1:formatName1}");
    EXPECT_EQ(int(uriTemplate.getFormatNames().size()), 1);
    UriTemplateValue extractedValues;
    EXPECT_EQ(uriTemplate.extract(extractedValues, "someText/AB2"), false);
}

TEST(UriTemplateTest, ExceptionOnDuplicateParameterName)
{
    initGlobalFormats();
    EXPECT_ANY_THROW(const UriTemplate uriTemplate("someText/{par1:formatName1}/{par1:FormatComplex}"));
}

TEST(UriTemplateTest, ExtractedValuesComplex)
{
    initGlobalFormats();
    const UriTemplate uriTemplate("someText/{someText:FormatName4}/longUri/{format1:formatName1}");
    EXPECT_EQ(int(uriTemplate.getFormatNames().size()), 2);
    UriTemplateValue extractedValues;
    EXPECT_EQ(uriTemplate.extract(extractedValues, "someText/AC_1234/01JAN15/someText/ABC/longUri/AB"), true);
    EXPECT_EQ(int(extractedValues.size()), 2);
    EXPECT_STREQ(extractedValues["someText"].getString().c_str(), "AC_1234/01JAN15/someText/ABC");
    EXPECT_EQ(int(extractedValues["someText"].size()), 2);

    EXPECT_STREQ(extractedValues["someText"]["complex"].getString().c_str(), "AC_1234/01JAN15");
    EXPECT_EQ(int(extractedValues["someText"]["complex"].size()), 3);
    EXPECT_STREQ(extractedValues["someText"]["complex"]["format1"].getString().c_str(), "AC");
    EXPECT_STREQ(extractedValues["someText"]["complex"]["number"].getString().c_str(), "1234");
    EXPECT_STREQ(extractedValues["someText"]["complex"]["date"].getString().c_str(), "01JAN15");

    EXPECT_STREQ(extractedValues["someText"]["format5"].getString().c_str(), "ABC");
    EXPECT_EQ(int(extractedValues["someText"]["format5"].size()), 0);

    EXPECT_STREQ(extractedValues["format1"].getString().c_str(), "AB");
    EXPECT_EQ(int(extractedValues["format1"].size()), 0);
}
