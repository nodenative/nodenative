#include "native/UriTemplate.hpp"

#include "native/UriTemplateFormat.hpp"
#include "native/UriTemplateValue.hpp"
#include "native/helper/trace.hpp"

#include <set>

namespace {

bool getNextParameter(std::string::const_iterator& iBegin, std::string::const_iterator iEnd, std::string& iText, std::string& Name, std::string& iFormat) {
    // regex is compiled only at the first call
    static const std::unique_ptr<native::Regex> re = native::Regex::Create("\\{([a-zA-Z][a-zA-Z0-9]*):([a-zA-Z][a-zA-Z0-9]*)\\}"); //e.g.: {param:formatName}
    std::unique_ptr<native::Smatch> results;

    if(!re->match(iBegin, iEnd, results, native::Regex::UNANCHORED)) {
        return false;
    }

    iText.assign(iBegin, iBegin + results->position());
    Name = results->str(1);
    iFormat = results->str(2);

    iBegin += results->position() + results->length();

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
        native::UriTemplateValue &ioParsedValues,
        int &ioPosition,
        const std::unique_ptr<native::Smatch> &iMatchResults,
        const std::vector<std::string> &iParams,
        const std::vector<std::string> &iFormatNames) {
    NNATIVE_ASSERT(iParams.size() == iFormatNames.size());
    NNATIVE_ASSERT(iMatchResults->size() - ioPosition >= static_cast<int>(iParams.size()));
    for(std::vector<std::string>::size_type i = 0; i < iParams.size(); ++i)
    {
        const std::string value = iMatchResults->str(ioPosition++);
        const std::string &name = iParams[i];
        const std::string &formatName = iFormatNames[i];
        native::UriTemplateValue &childValue = ioParsedValues.addChild(name, value);
        const native::UriTemplateFormat &format = native::UriTemplateFormat::GetGlobalFormat(formatName);

        if(format.isRegExOnly())
        {
            continue;
        }

        // Parse subvalues of format name
        const native::UriTemplate &uriTemplate = format.getTemplate();
        NNATIVE_DEBUG("Extract child values from \"" << value << "\" with key \"" << name << "\" by using format name \"" << formatName << "\" with pattern \"" << uriTemplate.getPattern(true) << "\"");
        saveValues(childValue, ioPosition, iMatchResults, uriTemplate.getParams(), uriTemplate.getFormatNames());
    }
}

bool extractAndSaveValues(native::UriTemplateValue &oValues,
                          const std::string &iUri,
                          std::unique_ptr<native::Regex> &iExtractRegex,
                          const std::string &iExtractPattern,
                          const std::vector<std::string> &iParams,
                          const std::vector<std::string> &iFormatNames,
                          const bool iAnchorEnd = true) {
    std::unique_ptr<native::Smatch> matchResults;
    if(!iExtractRegex) {
        iExtractRegex = native::Regex::Create(iExtractPattern);
    }

    if(!iExtractRegex->match(iUri, matchResults, (iAnchorEnd ? native::Regex::ANCHOR_BOTH : native::Regex::ANCHOR_START))) {
        NNATIVE_DEBUG("No match found");
        return false;
    }

    int position = 0;
    // No name to the first parsed value
    oValues.clear();
    oValues.getString() = matchResults->str(position++);
    // save child values
    saveValues(oValues, position, matchResults, iParams, iFormatNames);

    return true;
}

bool containCapturingGroup(const std::string &iPattern) {
    // regex is compiled only at the first call
    static const std::unique_ptr<native::Regex> reCapturingGroup = native::Regex::Create("\\([^\\?][^\\)]+\\)"); // true for "(test|other)", but false for non capturing groups (?:test|other)
    std::unique_ptr<native::Smatch> dummyResults;
    return reCapturingGroup->match(iPattern, dummyResults, native::Regex::UNANCHORED);
}

} /* namespace */

namespace native {

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
    NNATIVE_DEBUG("New URI Template \"" << _template << "\", size:" << _template.size());
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
    return extractAndSaveValues(oValues, iUri, _extractRegex, _extractPattern, getParams(), getFormatNames(), iAnchorEnd);
}

bool UriTemplate::ContainCapturingGroup(const std::string &iPattern) {
    return ::containCapturingGroup(iPattern);
}

} /* namespace native */
