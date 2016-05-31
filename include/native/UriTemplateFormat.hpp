#ifndef __NATIVE_URITEMPLATEFORMAT_HPP__
#define __NATIVE_URITEMPLATEFORMAT_HPP__

#include "UriTemplate.hpp"

#include <map>

namespace native {
/**
 * @brief URI Template format name class.
 * @details It holds a URITemplate and a given name. The URITemplate may contains dependency of other global formats.
 * The dependency will be checked at constructor level of URITemplate.
 * For regex, do not use capturing groups or end anchor. if groups are really required, use non capturing groups e.g.
 * (?:[A-Z0-9]{2}|[A-Z]{3})
 */
class UriTemplateFormat {
public:
  /**
   * @brief Main constructor
   *
   * @param iName format name
   * @param iTemplate format string
   */
  UriTemplateFormat(const std::string &iName, const std::string &iTemplate);

  /**
   * @brief Copy constructor
   *
   * @param iOther Other object from where data will be copied
   */
  UriTemplateFormat(const UriTemplateFormat &iOther);

  /**
   * @brief Check if the given template is regex only (not contains any dependent format names)
   * @return True if template is regex only, False otherwise
   */
  bool isRegExOnly() const { return _template.getParams().size() == 0; }

  /**
   * @brief Return generated regex pattern depends on needs
   *
   * @param iForExtraction True if require for extraction (include capturing groups), False for match only propose
   * (exclude capturing groups to gain speed)
   * @return regex patter including or excluding capturing groups, depends on parameter
   */
  std::string getPattern(const bool iForExtraction = false) const;

  /**
   * @brief Return URI template
   * @return template string
   */
  const UriTemplate &getTemplate() const { return _template; };

  /**
   * @brief Return format name
   * @return format name
   */
  const std::string &getName() const { return _name; };

  /**
   * @brief Return global format reference by format name
   *
   * @param iFormatName format name string
   * @return format reference
   * @error if format name is missing in the container
   */
  static const UriTemplateFormat &GetGlobalFormat(const std::string &iFormatName);

  /**
   * @brief Add global format. If a global format name already added, an error will throw.
   *
   * @param iFormatName format name
   * @param iTemplate template string
   * @error: assert if it trying to add a duplicate global format name
   */
  static void AddGlobalFormat(const std::string &iFormatName, const std::string &iTemplate);
  static void ClearGlobalFormats() { GetGlobalFormats().clear(); }
  static int GetGlobalFormatsSize() { return GetGlobalFormats().size(); };

private:
  /// format name string
  std::string _name;

  /// parsed URI template
  UriTemplate _template;

  /**
   * @brief Global static function to keep global formats
   * @return global formats map container
   */
  static std::map<std::string, UriTemplateFormat> &GetGlobalFormats();
};
}

#endif /* __NATIVE_URITEMPLATEFORMAT_HPP__ */
