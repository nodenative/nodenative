#include <gmock/gmock.h>
#include <native/native.hpp>

using namespace native;
using namespace http;

TEST(decodeUrl, doNotChange) {
  const std::string url("http://localhost/foo/bar.html?fizz=buzz#readme&other=value_");
  EXPECT_EQ(decodeUrl(url), url);
}

TEST(decodeUrl, doNotChangeIPv6) {
  const std::string url("http://[::1]:8080/foo/bar");
  EXPECT_EQ(decodeUrl(url), url);
}

TEST(decodeUrl, shouldEncodeLF) {
  const std::string url("http://localhost/%0Asnow.html");
  const std::string urlExpect("http://localhost/\nsnow.html");
  EXPECT_EQ(decodeUrl(url), urlExpect);
}

TEST(decodeUrl, shouldEncodeFF) {
  const std::string url("http://localhost/%0Csnow.html");
  const std::string urlExpect("http://localhost/\fsnow.html");
  EXPECT_EQ(decodeUrl(url), urlExpect);
}

TEST(decodeUrl, shouldEncodeCR) {
  const std::string url("http://localhost/%0Dsnow.html");
  const std::string urlExpect("http://localhost/\rsnow.html");
  EXPECT_EQ(decodeUrl(url), urlExpect);
}

TEST(decodeUrl, shouldEncodeSP) {
  const std::string url("http://localhost/%20snow.html");
  const std::string urlExpect("http://localhost/ snow.html");
  EXPECT_EQ(decodeUrl(url), urlExpect);
}

TEST(decodeUrl, shouldEncodeNULL) {
  const std::string url("http://localhost/%00snow.html");
  const std::string urlExpect("http://localhost/\0snow.html", 27);
  EXPECT_EQ(decodeUrl(url), urlExpect);
}

TEST(decodeUrl, shouldEncodePercentSequence) {
  const std::string url("http://localhost/%2520snow.html");
  const std::string urlExpect("http://localhost/%20snow.html");
  EXPECT_EQ(decodeUrl(url), urlExpect);
}

TEST(decodeUrl, shouldEncodePercentChar) {
  const std::string url("http://localhost/%25foo%25bar%25zap");
  const std::string urlExpect("http://localhost/%foo%bar%zap");
  EXPECT_EQ(decodeUrl(url), urlExpect);
}

TEST(decodeUrl, shouldEncodeCharactersFromASCII) {
  const std::string url("/%00%01%02%03%04%05%06%07%08%09%0A%0B%0C%0D%0E%0F"
                        "/%10%11%12%13%14%15%16%17%18%19%1A%1B%1C%1D%1E%1F"
                        "/%20!%22#$%25&\'()*%2B,-./"
                        "/0123456789:;%3C=%3E?"
                        "/@ABCDEFGHIJKLMNO"
                        "/PQRSTUVWXYZ[%5C]%5E_"
                        "/%60abcdefghijklmno"
                        "/pqrstuvwxyz%7B%7C%7D~%7F"
                        "/+");
  std::string urlExpect("/\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0a\x0b\x0c\x0d\x0e\x0f"
                        "/\x10\x11\x12\x13\x14\x15\x16\x17\x18\x19\x1a\x1b\x1c\x1d\x1e\x1f"
                        "/\x20\x21\x22\x23\x24\x25\x26\x27\x28\x29\x2a\x2b\x2c\x2d\x2e\x2f"
                        "/\x30\x31\x32\x33\x34\x35\x36\x37\x38\x39\x3a\x3b\x3c\x3d\x3e\x3f"
                        "/\x40\x41\x42\x43\x44\x45\x46\x47\x48\x49\x4a\x4b\x4c\x4d\x4e\x4f"
                        "/\x50\x51\x52\x53\x54\x55\x56\x57\x58\x59\x5a\x5b\x5c\x5d\x5e\x5f"
                        "/\x60\x61\x62\x63\x64\x65\x66\x67\x68\x69\x6a\x6b\x6c\x6d\x6e\x6f"
                        "/\x70\x71\x72\x73\x74\x75\x76\x77\x78\x79\x7a\x7b\x7c\x7d\x7e\x7f"
                        "/ ",
                        17 * 8 + 2);
  EXPECT_EQ(decodeUrl(url), urlExpect);
}

TEST(decodeUrl, shouldEncodeUnicode) {
  const std::string url("Hello%20G%C3%BCnter");
  const std::string urlExpect("Hello Günter");
  EXPECT_EQ(decodeUrl(url), urlExpect);
}
