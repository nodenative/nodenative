#ifndef __NATIVE_URITEMPLATE_HPP__
#define __NATIVE_URITEMPLATE_HPP__

#include "Regex.hpp"

#include <string>
#include <vector>

namespace native {

class UriTemplateFormat;
class UriTemplateValue;

/// URI Template class based on boost::regex
class UriTemplate {
public:
  /**
   * @brief URI template constructor.
   * URI template can be mixed with regex excluding capturing group e.g. "(CapturedRegEx)".
   * If groups a really required then use non capturing group. e.g. "(?:nonCapturedRegEx)".
   *
   * @param iTemplate URI template string. e.g. "flight/{leg:flightLeg}$"
   */
  UriTemplate(const std::string &iTemplate);

  /**
   * @brief Constructor based on URItemplate as template prefix and string template suffix
   * @details This constructor concatenate both templates and parse the final URI Template
   *
   * @param iBaseUriTemplate URI template as prefix
   * @param iTemplateSuffix string template as suffix
   */
  UriTemplate(const UriTemplate &iBaseUriTemplate, const std::string &iTemplateSuffix);

  /**
   * @brief Copy constructor
   *
   * @param iOther source object from where will be copied
   */
  UriTemplate(const UriTemplate &iOther);

  /**
   * @brief Return URI template string
   */
  const std::string &getTemplate() const { return _template; };

  /**
   * @brief Return regex pattern string.
   * @param iForExtraction default false. if true, return regex pattern with capturing groups, false otherwise (to
   * speed-up match operations).
   * @param iAddEndAnchor default true. add end anchor regex flag ("$").
   * @return regex pattern string
   * @note regex strings are initiated in the constructor
   */
  std::string getPattern(const bool iForExtraction = false, const bool iAddEndAnchor = true) const {
    return (iForExtraction ? _extractPattern : _matchPattern) + (iAddEndAnchor ? "$" : "");
  };

  /**
   * @brief parsed parameters from URI template string ordered by position in URI template
   */
  const std::vector<std::string> &getParams() const { return _params; }

  /**
   * @brief Return format name vector in the same order as URI template String
   * @return format name vector
   */
  const std::vector<std::string> &getFormatNames() const { return _formatNames; }

  /**
   * @brief Extract data values from URI based on URI template.
   * Regex is applied only one time to extract all the required data (groups and subgroups).
   * Extracted data (groups and subgroups) is placed in oValues recursively based on parameter names and format names.
   *
   * @param oValues container where will store extracted values
   * @param iUri URI string for extracting
   * @return true if URI match, false otherwise
   */
  bool extract(UriTemplateValue &oValues, const std::string &iUri, const bool iAnchorEnd = true) const;

  /**
   * @brief Return true if iPattern contains at least 1 capturing group
   *
   * @param iPattern patter string to check
   * @return true if iPattern contains capturing group, otherise fale
   */
  static bool ContainCapturingGroup(const std::string &iPattern);

private:
  /**
   * @brief Parse URI Template string to generate regex patterns and to extract parameters
   */
  void parse();

  // URI template string
  std::string _template;
  // Pattern string for match (without capturing groups)
  std::string _matchPattern;
  // Pattern string for extraction (with capturing groups)
  std::string _extractPattern;
  // parsed parameter vector container
  std::vector<std::string> _params;
  // parsed format name vector container
  std::vector<std::string> _formatNames;
  mutable std::unique_ptr<Regex> _extractRegex;
};

} /* mespace native */

#endif /* __NATIVE_URITEMPLATE_HPP__ */
