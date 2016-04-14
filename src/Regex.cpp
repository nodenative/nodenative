#include "native/Regex.hpp"

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

} /* namespace */

namespace native {

std::string getRegexLibName() {
  return NNATIVE_USE_REGEX_NAME;
}

std::unique_ptr<Regex> Regex::Create(const std::string &iText) {
#if NNATIVE_USE_RE2 == 1
    return std::unique_ptr<Regex>(new RegEx_RE2(iText));
#elif NNATIVE_USE_STDREGEX == 1
    return std::unique_ptr<Regex>(new RegEx_STD(iText));
#elif NNATIVE_USE_BOOSTREGEX == 1
    return std::unique_ptr<Regex>(new RegEx_BOOST(iText));
#endif
}

} /* *mespace native */

