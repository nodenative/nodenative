#include "native/UriTemplate.hpp"

#include "native/UriTemplateFormat.hpp"
#include "native/UriTemplateValue.hpp"
#include "native/helper/trace.hpp"

#if !defined(NNATIVE_USE_RE2) && !defined(NNATIVE_USE_STDREGEX)

#define NNATIVE_USE_RE2 1

#endif // if !defined(NNATIVE_USE_RE2) && !defined(NNATIVE_USE_STDREGEX)

#if NNATIVE_USE_RE2 == 1

#define NNATIVE_USE_REGEX_NAME "re2"
#include <re2/re2.h>

#elif NNATIVE_USE_STDREGEX == 1

#define NNATIVE_USE_REGEX_NAME "std"
#include <regex>

#elif NNATIVE_USE_BOOSTREGEX == 1

#define NNATIVE_USE_REGEX_NAME "boost"
#include <boost/regex.hpp>

#else

#error Use at least one of NNATIVE_USE_RE2, NNATIVE_USE_STDREGEX or NNATIVE_USE_BOOSTREGEX with value 1

#endif // elif NNATIVE_USE_STDREGEX == 1

#include <set>
#include <memory>

namespace {

#if NNATIVE_USE_RE2 == 1

class RegEx_RE2;

class Smatch_RE2 : public native::Smatch {
    friend class RegEx_RE2;
public:
    Smatch_RE2(const re2::RE2& iRegex, const re2::StringPiece &iText) : _text(iText) {
        _size = iRegex.NumberOfCapturingGroups() + 1;
        _pieces = std::unique_ptr<re2::StringPiece[]>(new re2::StringPiece[_size]);
    }

    int size() const override { return _size; }

    std::string str(int i) const override { return _pieces[i].as_string(); }

    int position() const override { return _pieces[0].begin() - _text.begin(); }
    int length() const override { return _pieces[0].size(); }

private:
    int _size;
    std::unique_ptr<re2::StringPiece[]> _pieces;
    re2::StringPiece _text;
};

class RegEx_RE2 : public native::Regex {
public:
    RegEx_RE2(const std::string &iText) : _regex(iText) {}

    bool match(const re2::StringPiece iText, std::unique_ptr<native::Smatch> &iResult, native::Regex::Anchor iAnchor) const {
        std::unique_ptr<Smatch_RE2> smatch(new Smatch_RE2(_regex, iText));

        re2::RE2::Anchor anchor;

        if(iAnchor == ANCHOR_BOTH) {
            anchor = re2::RE2::ANCHOR_BOTH;
        } else if (iAnchor == ANCHOR_START) {
            anchor = re2::RE2::ANCHOR_START;
        } else {
            anchor = re2::RE2::UNANCHORED;
        }

        if(!_regex.Match(iText, 0, iText.size(), anchor, smatch->_pieces.get(), smatch->size())) {
            return false;
        }

        iResult = std::move(smatch);

        return true;
    }

    bool match(const std::string &iText, std::unique_ptr<native::Smatch> &iResult, native::Regex::Anchor iAnchor) const override {
        re2::StringPiece text(iText);
        return match(text, iResult, iAnchor);
    }

    bool match(std::string::const_iterator iBegin, std::string::const_iterator iEnd, std::unique_ptr<native::Smatch> &iResult, native::Regex::Anchor iAnchor) const override {
        re2::StringPiece text(&(*iBegin), std::distance(iBegin, iEnd));
        return match(text, iResult, iAnchor);
    }
private:
    re2::RE2 _regex;
};

#elif NNATIVE_USE_STDREGEX == 1

class RegEx_STD;

class Smatch_STD : public native::Smatch {
    friend class RegEx_STD;
public:
    Smatch_STD(const std::regex iRegex) {}

    int size() const override { return _pieces.size(); }

    std::string str(int i) const override { return _pieces.str(i); }

    int position() const override { return _pieces.position(); }
    int length() const override { return _pieces.length(); }

private:
    std::smatch _pieces;
};

class RegEx_STD : public native::Regex {
public:
    RegEx_STD(const std::string &iText) : _regex(iText) {}

    bool match(std::string::const_iterator iBegin, std::string::const_iterator iEnd, std::unique_ptr<native::Smatch> &iResult, native::Regex::Anchor iAnchor) const override {
        std::unique_ptr<Smatch_STD> smatch(new Smatch_STD(_regex));

        if(iAnchor == ANCHOR_BOTH) {
            if(!std::regex_match(iBegin, iEnd, smatch->_pieces, _regex)) {
                return false;
            }
        } else {
            if(!std::regex_search(iBegin, iEnd, smatch->_pieces, _regex, (iAnchor == ANCHOR_START ? std::regex_constants::match_continuous : std::regex_constants::match_default))) {
                return false;
            }
        }

        iResult = std::move(smatch);

        return true;
    }

    bool match(const std::string &iText, std::unique_ptr<native::Smatch> &iResult, native::Regex::Anchor iAnchor) const override {
        return match(iText.begin(), iText.end(), iResult, iAnchor);
    }
private:
    std::regex _regex;
};

#elif NNATIVE_USE_BOOSTREGEX == 1

class RegEx_BOOST;

class Smatch_BOOST : public native::Smatch {
    friend class RegEx_BOOST;
public:
    Smatch_BOOST(const boost::regex iRegex) {}

    int size() const override { return _pieces.size(); }

    std::string str(int i) const override { return _pieces.str(i); }

    int position() const override { return _pieces.position(); }
    int length() const override { return _pieces.length(); }

private:
    boost::smatch _pieces;
};

class RegEx_BOOST : public native::Regex {
public:
    RegEx_BOOST(const std::string &iText) : _regex(iText) {}

    bool match(std::string::const_iterator iBegin, std::string::const_iterator iEnd, std::unique_ptr<native::Smatch> &iResult, native::Regex::Anchor iAnchor) const override {
        std::unique_ptr<Smatch_BOOST> smatch(new Smatch_BOOST(_regex));

        if(iAnchor == ANCHOR_BOTH) {
            if(!boost::regex_match(iBegin, iEnd, smatch->_pieces, _regex)) {
                return false;
            }
        } else {
            if(!boost::regex_search(iBegin, iEnd, smatch->_pieces, _regex, (iAnchor == ANCHOR_START ? boost::regex_constants::match_continuous : boost::regex_constants::match_default))) {
                return false;
            }
        }

        iResult = std::move(smatch);

        return true;
    }

    bool match(const std::string &iText, std::unique_ptr<native::Smatch> &iResult, native::Regex::Anchor iAnchor) const override {
        return match(iText.begin(), iText.end(), iResult, iAnchor);
    }
private:
    boost::regex _regex;
};

#endif // elif NNATIVE_USE_STDREGEX == 1

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

std::unique_ptr<Regex> Regex::Create(const std::string &iText) {
#if NNATIVE_USE_RE2 == 1
    return std::unique_ptr<Regex>(new RegEx_RE2(iText));
#elif NNATIVE_USE_STDREGEX == 1
    return std::unique_ptr<Regex>(new RegEx_STD(iText));
#elif NNATIVE_USE_BOOSTREGEX == 1
    return std::unique_ptr<Regex>(new RegEx_BOOST(iText));
#endif
}

std::string getRegexLibName() {
  return NNATIVE_USE_REGEX_NAME;
}

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
