#include "native/UriTemplate.hpp"

#include "native/UriTemplateFormat.hpp"
#include "native/UriTemplateValue.hpp"
#include "native/helper/trace.hpp"

#if NNATIVE_USE_RE2 == 1
#include <re2.h>
#elif NNATIVE_USE_STDREGEX == 1
#include <regex>
#else

#endif // elif NNATIVE_USE_STDREGEX == 1

#include <set>

using namespace native;

namespace {

#if NNATIVE_USE_RE2 == 1

bool getNextParameter(std::string::const_iterator& iBegin, std::string::const_iterator iEnd, std::string& iText, std::string& Name, std::string& iFormat) {
    // regex is compiled only at the first call
    static const re2::RE2 reParam("\\{([a-zA-Z][a-zA-Z0-9]*):([a-zA-Z][a-zA-Z0-9]*)\\}"); //e.g.: {param:formatName}
    re2::StringPiece text(&(*iBegin), std::distance(iBegin, iEnd));
    re2::StringPiece results[3];

    if(!reParam.Match(text, 0, text.size(), re2::RE2::UNANCHORED, results, 3)) {
        return false;
    }

    iText.assign(text.data(), results[0].begin() - text.begin());
    Name.assign(results[1].data(), results[1].size());
    iFormat.assign(results[2].data(), results[2].size());

    iBegin += results[0].end() - text.begin();

    return true;
}

/**
 * Recursive save extracted data by regex into values
 * @param ioParsedValues parsed values container to be populated
 * @param ioPosition current smatch position
 * @iParams parameters name vector to be added as key values
 * @iFormatNames format name vector to parse child values, if exists
 */
void saveValues(
        UriTemplateValue &ioParsedValues,
        int &ioPosition,
        const re2::StringPiece* iMatchResults,
        const int iMatchResultsSize,
        const std::vector<std::string> &iParams,
        const std::vector<std::string> &iFormatNames) {
    NNATIVE_ASSERT(iParams.size() == iFormatNames.size());
    NNATIVE_ASSERT(iMatchResultsSize - ioPosition >= static_cast<int>(iParams.size()));
    for(std::vector<std::string>::size_type i = 0; i < iParams.size(); ++i)
    {
        const std::string value = iMatchResults[ioPosition++].as_string();
        const std::string &name = iParams[i];
        const std::string &formatName = iFormatNames[i];
        UriTemplateValue &childValue = ioParsedValues.addChild(name, value);
        const UriTemplateFormat &format = UriTemplateFormat::GetGlobalFormat(formatName);

        if(format.isRegExOnly())
        {
            continue;
        }

        // Parse subvalues of format name
        const UriTemplate &uriTemplate = format.getTemplate();
        NNATIVE_DEBUG("Extract child values from \"" << value << "\" with key \"" << name << "\" by using format name \"" << formatName << "\" with pattern \"" << uriTemplate.getPattern(true) << "\"");
        saveValues(childValue, ioPosition, iMatchResults, iMatchResultsSize, uriTemplate.getParams(), uriTemplate.getFormatNames());
    }
}

bool extractAndSaveValues(UriTemplateValue &oValues,
                          const std::string &iUri,
                          const std::string &iExtractPattern,
                          const std::vector<std::string> &iParams,
                          const std::vector<std::string> &iFormatNames,
                          const bool iAnchorEnd = true) {
    const re2::RE2 rePattern(iExtractPattern);
    const int matchResultsSize = 1+rePattern.NumberOfCapturingGroups();
    re2::StringPiece matchResults[matchResultsSize];
    re2::StringPiece uriPiece(iUri);

    if(!rePattern.Match(uriPiece, 0, uriPiece.size(), (iAnchorEnd ? re2::RE2::ANCHOR_BOTH : re2::RE2::ANCHOR_START), matchResults, matchResultsSize)) {
        NNATIVE_DEBUG("No match found");
        return false;
    }

    int position = 0;
    // No name to the first parsed value
    oValues.clear();
    oValues.getString() = matchResults[position++].as_string();
    // save child values
    saveValues(oValues, position, matchResults, matchResultsSize, iParams, iFormatNames);

    return true;
}

bool containCapturingGroup(const std::string &iPattern) {
    // regex is compiled only at the first call
    static const re2::RE2 reCapturingGroup("\\([^?\\)]+\\)"); // true for "(test|other)", but false for non capturing groups (?:test|other)
    re2::StringPiece strPiece(iPattern);

    return reCapturingGroup.Match(strPiece, 0, strPiece.size(), re2::RE2::UNANCHORED, nullptr, 0);
}

#elif NNATIVE_USE_STDREGEX == 1
#include <regex>

bool getNextParameter(std::string::const_iterator& iBegin, std::string::const_iterator iEnd, std::string& iText, std::string& Name, std::string& iFormat) {
    // regex is compiled only at the first call
    static const std::regex reParam("\\{([a-zA-Z][a-zA-Z0-9]*):([a-zA-Z][a-zA-Z0-9]*)\\}"); //e.g.: {param:formatName}
    std::smatch results;

    if(!std::regex_search(iBegin, iEnd, results, reParam)) {
        return false;
    }

    iText.assign(iBegin, iBegin + results.position());
    Name = results.str(1);
    iFormat = results.str(2);

    iBegin += results.position() + results.length();

    return true;
}

/**
 * Recursive save extracted data by regex into values
 * @param ioParsedValues parsed values container to be populated
 * @param ioPosition current smatch position
 * @iParams parameters name vector to be added as key values
 * @iFormatNames format name vector to parse child values, if exists
 */
void saveValues(
        UriTemplateValue &ioParsedValues,
        int &ioPosition,
        const boost::smatch &iMatchResults,
        const std::vector<std::string> &iParams,
        const std::vector<std::string> &iFormatNames) {
    NNATIVE_ASSERT(iParams.size() == iFormatNames.size());
    NNATIVE_ASSERT(iMatchResults.size() - ioPosition >= iParams.size());
    for(std::vector<std::string>::size_type i = 0; i < iParams.size(); ++i)
    {
        const std::string value = iMatchResults.str(ioPosition++);
        const std::string &name = iParams[i];
        const std::string &formatName = iFormatNames[i];
        UriTemplateValue &childValue = ioParsedValues.addChild(name, value);
        const UriTemplateFormat &format = UriTemplateFormat::GetGlobalFormat(formatName);

        if(format.isRegExOnly())
        {
            continue;
        }

        // Parse subvalues of format name
        const UriTemplate &uriTemplate = format.getTemplate();
        NNATIVE_DEBUG("Extract child values from \"" << value << "\" with key \"" << name << "\" by using format name \"" << formatName << "\" with pattern \"" << uriTemplate.getPattern(true) << "\"");
        saveValues(childValue, ioPosition, iMatchResults, uriTemplate.getParams(), uriTemplate.getFormatNames());
    }
}

bool extractAndSaveValues(UriTemplateValue &oValues,
                          const std::string &iUri,
                          const std::string &iExtractPattern,
                          const std::vector<std::string> &iParams,
                          const std::vector<std::string> &iFormatNames,
                          const bool iAnchorEnd = true) {
    std::smatch matchResults;
    const boost::regex rePattern(iExtractPattern);
    if(!std::regex_match(iUri, matchResults, rePattern, std::match_extra /*consider capturing subgroups also*/)) {
        NNATIVE_DEBUG("No match found");
        return false;
    }

    int position = 0;
    // No name to the first parsed value
    oValues.clear();
    oValues.getString() = matchResults.str(position++);
    // save child values
    saveValues(oValues, position, matchResults, iParams, iFormatNames);

    return true;
}

bool containCapturingGroup(const std::string &iPattern) {
    // regex is compiled only at the first call
    static const boost::regex reCapturingGroup("\\([^?\\)]+\\)"); // true for "(test|other)", but false for non capturing groups (?:test|other)
    boost::smatch dummyResults;
    return boost::regex_search(iPattern, dummyResults, reCapturingGroup, boost::match_any);
}

#endif // elif NNATIVE_USE_STDREGEX == 1

} /* namespace */

UriTemplate::UriTemplate(const std::string &iTemplate) : _template(iTemplate) {
    parse();
}

UriTemplate::UriTemplate(const UriTemplate &iBaseUriTemplate, const std::string &iTemplateSuffix) :
        _template(iBaseUriTemplate.getTemplate() + iTemplateSuffix) {
    parse();
}

UriTemplate::UriTemplate(const UriTemplate &iOther) :
        _template(iOther._template),
        _matchPattern(iOther._matchPattern),
        _extractPattern(iOther._extractPattern),
        _params(iOther._params),
        _formatNames(iOther._formatNames) {
    // Avoid parsing because template string is already parsed
}

void UriTemplate::parse() {
    NNATIVE_DEBUG("New URI Template \"" << _template << "\"");
    // should not contain capturing groups. e.g. "/path/(caputeText)/{param1:format1}"
    NNATIVE_ASSERT_MSG(!ContainCapturingGroup(_template), "URI template \"" << _template << "\" contain capturing groups");

    _matchPattern = "";
    _extractPattern = "";
    std::set<std::string> uniqParams;
    _params.clear();
    _formatNames.clear();

    // extract format names
    std::string::const_iterator begin = _template.begin();
    std::string::const_iterator end = _template.end();
    std::string textStr;
    std::string paramName;
    std::string formatName;


    while (getNextParameter(begin, end, textStr, paramName, formatName)) {
        NNATIVE_DEBUG("paramName:"<<paramName<<", formatName:"<<formatName);
        const UriTemplateFormat& globalFormat = UriTemplateFormat::GetGlobalFormat(formatName);

        // Check if parameter name duplicate. e.g: "/path{sameParamName:anyFormatName}/other/{sameParamName:anyFormatName}"
        if(!uniqParams.insert(paramName).second) {
            NNATIVE_DEBUG("Duplicate parameter name found " << paramName);
            NNATIVE_ASSERT(0);
        }

        _params.push_back(paramName);
        _formatNames.push_back(formatName);

        _matchPattern += textStr + globalFormat.getPattern();
        _extractPattern += textStr + globalFormat.getPattern(true);
    }
    // add the remaing string
    std::string textRemained(begin, end);
    _matchPattern += textRemained;
    // extract pattern may be part of a format name, do net add end of string ("$")
    _extractPattern += textRemained;

    NNATIVE_DEBUG("Pattern for match: \"" << _matchPattern << "\"");
    NNATIVE_DEBUG("Pattern for extraction: \"" << _extractPattern << "\"");
}

bool UriTemplate::extract(UriTemplateValue &oValues, const std::string &iUri, const bool iAnchorEnd) const {
    NNATIVE_DEBUG("Extract values from \"" << iUri << "\" by using URI template \"" << _template << "\", pattern: \"" << _extractPattern << "\")");
    return extractAndSaveValues(oValues, iUri, _extractPattern, getParams(), getFormatNames(), iAnchorEnd);
}

bool UriTemplate::ContainCapturingGroup(const std::string &iPattern) {
    return ::containCapturingGroup(iPattern);
}

