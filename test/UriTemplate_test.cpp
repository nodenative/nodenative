#include <native/UriTemplate.hpp>
#include <native/UriTemplateFormat.hpp>
#include <mative/UriTemplateValue.hpp>

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
        UriTemplateFormat::AddGlobalFormat("FormatName5", "{format1:formatName1}{number:FormatName2}-{departureDate:FormatName3}");
        UriTemplateFormat::AddGlobalFormat("FormatName6", "[A-Z]{3}");
        UriTemplateFormat::AddGlobalFormat("FormatName4", "{FormatName5:FormatName5}/someText/{departurePort:FormatName6}");
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
}

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
        "someText/(?:[A-Z0-9\\-]{2}|[A-Z]{3})[0-9]{1,4}[A-Z]?-[0-9]{2}[A-Z]{3}[0-9]{2}/someText/[A-Z]{3}",
        "someText/((((?:[A-Z0-9\\-]{2}|[A-Z]{3}))([0-9]{1,4}[A-Z]?)-([0-9]{2}[A-Z]{3}[0-9]{2}))/someText/([A-Z]{3}))");
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
    EXPECT_ANY_THROW(const UriTemplate uriTemplate("someText/{par1:formatName1}/{par1:FormatName5}"));
}

TEST(UriTemplateTest, ExtractedValuesComplex)
{
    initGlobalFormats();
    const UriTemplate uriTemplate("someText/{someText:FormatName4}/linkedCarrier/{format1:formatName1}");
    EXPECT_EQ(int(uriTemplate.getFormatNames().size()), 2);
    UriTemplateValue extractedValues;
    EXPECT_EQ(uriTemplate.extract(extractedValues, "someText/AC5004-01JAN15/someText/ABC/linkedCarrier/AB"), true);
    EXPECT_EQ(int(extractedValues.size()), 2);
    EXPECT_STREQ(extractedValues["someText"].getString().c_str(), "AC5004-01JAN15/someText/ABC");
    EXPECT_EQ(int(extractedValues["someText"].size()), 2);

    EXPECT_STREQ(extractedValues["someText"]["FormatName5"].getString().c_str(), "AC5004-01JAN15");
    EXPECT_EQ(int(extractedValues["someText"]["FormatName5"].size()), 3);
    EXPECT_STREQ(extractedValues["someText"]["FormatName5"]["format1"].getString().c_str(), "AC");
    EXPECT_STREQ(extractedValues["someText"]["FormatName5"]["number"].getString().c_str(), "5004");
    EXPECT_STREQ(extractedValues["someText"]["FormatName5"]["departureDate"].getString().c_str(), "01JAN15");

    EXPECT_STREQ(extractedValues["someText"]["departurePort"].getString().c_str(), "ABC");
    EXPECT_EQ(int(extractedValues["someText"]["departurePort"].size()), 0);

    EXPECT_STREQ(extractedValues["format1"].getString().c_str(), "AB");
    EXPECT_EQ(int(extractedValues["format1"].size()), 0);
}
