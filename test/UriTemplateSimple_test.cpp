#include <native/native.hpp>

#include <gtest/gtest.h>

using namespace native;

namespace {
void checkUriTemplate(const std::string pattern, const std::string matchPattern, const std::string extractionPattern) {
  EXPECT_NO_THROW(const UriTemplate uriTemplate(pattern));
  const std::string matchPatternWithEndAnchor(matchPattern + "$");
  const std::string extractionPatternWithEndAnchor(extractionPattern + "$");
  const UriTemplate uriTemplate(pattern);

  EXPECT_STREQ(uriTemplate.getTemplate().c_str(), pattern.c_str());
  EXPECT_STREQ(uriTemplate.getPattern().c_str(), matchPatternWithEndAnchor.c_str());
  EXPECT_STREQ(uriTemplate.getPattern(false, false).c_str(), matchPattern.c_str());
  EXPECT_STREQ(uriTemplate.getPattern(true).c_str(), extractionPatternWithEndAnchor.c_str());
  EXPECT_STREQ(uriTemplate.getPattern(true, false).c_str(), extractionPattern.c_str());
}
} /* namespace */

TEST(UriTemplateSimpleTest, Preffix) {
  std::string pattern = ":key/simpleText123";
  checkUriTemplate(pattern, "[^/]+/simpleText123", "([^/]+)/simpleText123");
  const UriTemplate uriTemplate(pattern);
  UriTemplateValue values;
  EXPECT_TRUE(uriTemplate.extract(values, "123/simpleText123"));
  EXPECT_EQ(values["key"].getString(), std::string("123"));
}

TEST(UriTemplateSimpleTest, Suffix) {
  std::string pattern = "simpleText123/:key";
  checkUriTemplate(pattern, "simpleText123/[^/]+", "simpleText123/([^/]+)");
  const UriTemplate uriTemplate(pattern);
  UriTemplateValue values;
  EXPECT_TRUE(uriTemplate.extract(values, "simpleText123/abc"));
  EXPECT_EQ(values["key"].getString(), std::string("abc"));
}

TEST(UriTemplateSimpleTest, Middle) {
  std::string pattern = "simpleText123/:key/SomeOtherText";
  checkUriTemplate(pattern, "simpleText123/[^/]+/SomeOtherText", "simpleText123/([^/]+)/SomeOtherText");
  const UriTemplate uriTemplate(pattern);
  UriTemplateValue values;
  EXPECT_TRUE(uriTemplate.extract(values, "simpleText123/abc/SomeOtherText"));
  EXPECT_EQ(values["key"].getString(), std::string("abc"));
}

TEST(UriTemplateSimpleTest, Preffix2) {
  std::string pattern = "{key}/simpleText123";
  checkUriTemplate(pattern, "[^/]+/simpleText123", "([^/]+)/simpleText123");
  const UriTemplate uriTemplate(pattern);
  UriTemplateValue values;
  EXPECT_TRUE(uriTemplate.extract(values, "123/simpleText123"));
  EXPECT_EQ(values["key"].getString(), std::string("123"));
}

TEST(UriTemplateSimpleTest, Suffix2) {
  std::string pattern = "simpleText123/{key}";
  checkUriTemplate(pattern, "simpleText123/[^/]+", "simpleText123/([^/]+)");
  const UriTemplate uriTemplate(pattern);
  UriTemplateValue values;
  EXPECT_TRUE(uriTemplate.extract(values, "simpleText123/abc"));
  EXPECT_EQ(values["key"].getString(), std::string("abc"));
}

TEST(UriTemplateSimpleTest, Middle2) {
  std::string pattern = "simpleText123/{key}/SomeOtherText";
  checkUriTemplate(pattern, "simpleText123/[^/]+/SomeOtherText", "simpleText123/([^/]+)/SomeOtherText");
  const UriTemplate uriTemplate(pattern);
  UriTemplateValue values;
  EXPECT_TRUE(uriTemplate.extract(values, "simpleText123/abc/SomeOtherText"));
  EXPECT_EQ(values["key"].getString(), std::string("abc"));
}
