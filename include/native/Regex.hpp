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
 */
class Regex {
public:
  enum Anchor { ANCHOR_BOTH, ANCHOR_START, UNANCHORED };

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
  virtual bool
  match(const std::string &iText, std::unique_ptr<Smatch> &iResult, Anchor iAnchor = ANCHOR_BOTH) const = 0;
  virtual bool match(std::string::const_iterator iBegin,
                     std::string::const_iterator iEnd,
                     std::unique_ptr<Smatch> &iMatch,
                     Anchor iAnchor = ANCHOR_BOTH) const = 0;
};

} /* namespace native */

#endif /* __NATIVE_REGEX_HPP__ */
