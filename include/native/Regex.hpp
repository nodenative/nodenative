#ifndef __NATIVE_REGEX_HPP__
#define __NATIVE_REGEX_HPP__

#include <memory>
#include <string>

namespace native {

std::string getRegexLibName();

class Smatch {
public:
  virtual ~Smatch() {}

  /**
   * [str description]
   * @param  i [description]
   * @return   [description]
   */
  virtual std::string str(const int i = 0) const = 0;

  /**
   * @return Number of capturing groups
   */
  virtual int size() const = 0;
  virtual int position() const = 0;

  /**
   * @return Length of the matched string
   */
  virtual int length() const = 0;
};

/**
 * Regex wrapper. It give the possibility to wrap in a common interface one of the following library:
 * - RE2
 * - std::regex
 * - boost::regex
 * It uses ECMAScript rules
 */
class Regex {
public:
  enum Anchor { ANCHOR_BOTH, ANCHOR_START, UNANCHORED };
  enum ReplaceType { REPLACE_FIRST, REPLACE_ANY };

  /**
   * create the default wrapper regex.
   */
  static std::unique_ptr<Regex> Create(const std::string &iRegexText);

  /**
   * Virtual destructor because it has pure virtual methods
   */
  virtual ~Regex() {}

  /**
   * Match methods
   */
  virtual bool match(const std::string &str, std::unique_ptr<Smatch> &result, Anchor anchor = ANCHOR_BOTH) const = 0;
  virtual bool match(std::string::const_iterator start,
                     std::string::const_iterator end,
                     std::unique_ptr<Smatch> &smatch,
                     Anchor anchor = ANCHOR_BOTH) const = 0;

  virtual bool replace(std::string &str, const std::string &rewrite, ReplaceType type = REPLACE_FIRST) const = 0;
};

} /* namespace native */

#endif /* __NATIVE_REGEX_HPP__ */
