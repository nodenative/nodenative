#include <native/UriTemplate.hpp>
#include <native/UriTemplateFormat.hpp>

#include <gtest/gtest.h>
#include <string>

using namespace native;

namespace {
void checkNewGlobalFormat(const std::string key,
                          const std::string value,
                          const std::string matchPattern,
                          const std::string extractionPattern) {
  EXPECT_NO_THROW(UriTemplateFormat::AddGlobalFormat(key, value));

  EXPECT_STREQ(UriTemplateFormat::GetGlobalFormat(key).getTemplate().getTemplate().c_str(), value.c_str());
  EXPECT_STREQ(UriTemplateFormat::GetGlobalFormat(key).getPattern().c_str(), matchPattern.c_str());
  EXPECT_STREQ(UriTemplateFormat::GetGlobalFormat(key).getPattern(true).c_str(), extractionPattern.c_str());
}
} /* namespace */

TEST(UriTemplateFormatTest, GlobalFormatSimpleText) {
  UriTemplateFormat::ClearGlobalFormats();
  checkNewGlobalFormat("GlobalFormatSimpleText", "someTextValue", "someTextValue", "(someTextValue)");
  EXPECT_EQ(UriTemplateFormat::GetGlobalFormat("GlobalFormatSimpleText").isRegExOnly(), true);
}

TEST(UriTemplateFormatTest, GrobalFormatExceptionOnDuplicate) {
  UriTemplateFormat::ClearGlobalFormats();
  const std::string formatKey("GrobalFormatExceptionOnDuplicate");
  const std::string formatValue("sameTextValue");

  EXPECT_NO_THROW(UriTemplateFormat::AddGlobalFormat(formatKey, formatValue));
  EXPECT_ANY_THROW(UriTemplateFormat::AddGlobalFormat(formatKey, formatValue));
}

TEST(UriTemplateFormatTest, GlobalFormatSimpleRegExp) {
  UriTemplateFormat::ClearGlobalFormats();
  checkNewGlobalFormat("GlobalFormatSimpleRegExp", "[A-Z]+", "[A-Z]+", "([A-Z]+)");
  EXPECT_EQ(UriTemplateFormat::GetGlobalFormat("GlobalFormatSimpleRegExp").isRegExOnly(), true);
}

TEST(UriTemplateFormatTest, GlobalFormatNoneCapturingGroup) {
  UriTemplateFormat::ClearGlobalFormats();
  checkNewGlobalFormat("GlobalFormatNoneCapturingGroup", "(?:[A-Z]+)SomeText", "(?:[A-Z]+)SomeText",
                       "((?:[A-Z]+)SomeText)");
}

TEST(UriTemplateFormatTest, GrobalFormatExceptionOnCapturingGroup) {
  UriTemplateFormat::ClearGlobalFormats();
  const std::string formatKey("GrobalFormatExceptionOnCapturingGroup");
  const std::string formatValue("(CapturingGroup)");

  EXPECT_ANY_THROW(UriTemplateFormat::AddGlobalFormat(formatKey, formatValue));
}

TEST(UriTemplateFormatTest, GrobalFormatUriTemplate) {
  UriTemplateFormat::ClearGlobalFormats();
  checkNewGlobalFormat("GrobalFormatUriTemplatePar1", "[A-Z]+", "[A-Z]+", "([A-Z]+)");
  checkNewGlobalFormat("GrobalFormatUriTemplate", "someText{par1:GrobalFormatUriTemplatePar1}", "someText[A-Z]+",
                       "(someText([A-Z]+))");
  EXPECT_EQ(UriTemplateFormat::GetGlobalFormat("GrobalFormatUriTemplate").isRegExOnly(), false);
}

TEST(UriTemplateFormatTest, GrobalFormatUriTemplate2) {
  UriTemplateFormat::ClearGlobalFormats();
  checkNewGlobalFormat("GrobalFormatUriTemplate2Par1", "Before1[A-Z]+After1", "Before1[A-Z]+After1",
                       "(Before1[A-Z]+After1)");
  checkNewGlobalFormat("GrobalFormatUriTemplate2Par2", "Before2[A-Z]+{par1:GrobalFormatUriTemplate2Par1}After2",
                       "Before2[A-Z]+Before1[A-Z]+After1After2", "(Before2[A-Z]+(Before1[A-Z]+After1)After2)");
  checkNewGlobalFormat("GrobalFormatUriTemplate2", "Before{par1:GrobalFormatUriTemplate2Par2}After",
                       "BeforeBefore2[A-Z]+Before1[A-Z]+After1After2After",
                       "(Before(Before2[A-Z]+(Before1[A-Z]+After1)After2)After)");
}
