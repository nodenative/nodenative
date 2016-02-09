#include "native/UriTemplate.h"

#include "native/UriTemplateFormat.h"
#include "native/UriTemplateValue.h"

#include <boost/regex.hpp>

using namespace native;

namespace
{
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
            const std::vector<std::string> &iFormatNames
            )
    {
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
}

UriTemplate::UriTemplate(const std::string &iTemplate) : _template(iTemplate)
{
    parse();
}

UriTemplate::UriTemplate(const UriTemplate &iBaseUriTemplate, const std::string &iTemplateSuffix) :
        _template(iBaseUriTemplate.getTemplate() + iTemplateSuffix)
{
    parse();
}

UriTemplate::UriTemplate(const UriTemplate &iOther) :
        _template(iOther._template),
        _matchPattern(iOther._matchPattern),
        _extractPattern(iOther._extractPattern),
        _params(iOther._params),
        _formatNames(iOther._formatNames)
{
    // No needs parse because template string is already parsed
}

void UriTemplate::parse()
{
    NNATIVE_DEBUG("New URI Template \"" << _template << "\"");
    // should not contain capturing groups. e.g. "/path/(caputeText)/{param1:format1}"
    if(ContainCapturingGroup(_template))
    {
        NNATIVE_DEBUG("URI template \"" << _template << "\" contain capturing groups");
        NNATIVE_ASSERT(0);
    }

    _matchPattern = "";
    _extractPattern = "";
    std::set<std::string> uniqParams;
    _params.clear();
    _formatNames.clear();

    // extract format names
    std::string::const_iterator begin = _template.begin();
    std::string::const_iterator end = _template.end();
    boost::smatch results;
    // regex is compiled only at the first call
    //TODO: switch to re2 to gain even more performance
    static const boost::regex reParam("\\{([a-zA-Z][a-zA-Z0-9]*):([a-zA-Z][a-zA-Z0-9]*)\\}"); //e.g.: {param:formatName}

    while (boost::regex_search(begin, end, results, reParam))
    {
        const std::string paramName = results.str(1);
        const std::string formatName = results.str(2);
        NNATIVE_DEBUG("paramName:"<<paramName<<", formatName:"<<formatName);
        const UriTemplateFormat& globalFormat = UriTemplateFormat::GetGlobalFormat(formatName);

        // Check if parameter name duplicate. e.g: "/path{sameParamName:anyFormatName}/other/{sameParamName:anyFormatName}"
        if(!uniqParams.insert(paramName).second)
        {
            NNATIVE_DEBUG("Duplicate parameter name found " << paramName);
            NNATIVE_ASSERT(0);
        }

        _params.push_back(paramName);
        _formatNames.push_back(formatName);

        std::string textStr(begin, begin + results.position());
        _matchPattern += textStr + globalFormat.getPattern();
        _extractPattern += textStr + globalFormat.getPattern(true);

        begin += results.position() + results.length();
    }
    // add the remaing string
    std::string textStr(begin, end);
    _matchPattern += textStr;
    // extract pattern may be part of a format name, do net add end of string ("$")
    _extractPattern += textStr;

    NNATIVE_DEBUG("Pattern for match: \"" << _matchPattern << "\"");
    NNATIVE_DEBUG("Pattern for extraction: \"" << _extractPattern << "\"");
}

bool UriTemplate::extract(UriTemplateValue &oValues, const std::string &iUri) const
{
    NNATIVE_DEBUG("Extract values from \"" << iUri << "\" by using URI template \"" << _template << "\", pattern: \"" << _extractPattern << "\")");
    boost::smatch matchResults;
    const boost::regex rePattern(_extractPattern);
    if(!boost::regex_match(iUri, matchResults, rePattern, boost::match_extra /*consider capturing subgroups also*/))
    {
        NNATIVE_DEBUG("No match found");
        return false;
    }

    int position = 0;
    // No name to the first parsed value
    oValues.clear();
    oValues.getString() = matchResults.str(position++);
    // save child values
    saveValues(oValues, position, matchResults, getParams(), getFormatNames());

    return true;
}

bool UriTemplate::ContainCapturingGroup(const std::string &iPattern)
{
    // regex is compiled only at the first call
    static const boost::regex reCapturingGroup("\\([^?\\)]+\\)"); // true for "(test|other)", but false for non capturing groups (?:test|other)
    boost::smatch dummyResults;
    return boost::regex_search(iPattern, dummyResults, reCapturingGroup, boost::match_any);
}

