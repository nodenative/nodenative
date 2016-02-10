#include "native/UriTemplateFormat.hh"

#include <map>
#include <utility>

using namespace native;

UriTemplateFormat::UriTemplateFormat(const std::string &iName, const std::string &iTemplate) :
        _name(iName),
        _template(iTemplate)
{
    // No needs for cyclic dependency check, because at each UriTemplate constructor the dependency is checked and added in series
}

UriTemplateFormat::UriTemplateFormat(const UriTemplateFormat &iOther) :
        _name(iOther._name),
        _template(iOther._template)
{
}

std::string UriTemplateFormat::getPattern(const bool iForExtraction) const
{
    std::string result;

    if(iForExtraction)
    {
        result += "(";
    }

    result += _template.getPattern(iForExtraction, false);

    if(iForExtraction)
    {
        result += ")";
    }

    return result;
}

const UriTemplateFormat & UriTemplateFormat::GetGlobalFormat(const std::string &iFormatName)
{
    std::map<std::string, UriTemplateFormat>::const_iterator it = GetGlobalFormats().find(iFormatName);
    if(it == GetGlobalFormats().end())
    {
        NNATIVE_DEBUG("Missing global format name " << iFormatName);
        NNATIVE_ASSERT(0);
    }
    return it->second;
}

std::map<std::string, UriTemplateFormat> & UriTemplateFormat::GetGlobalFormats()
{
    static std::map<std::string, UriTemplateFormat> formats;
    return formats;
}

void UriTemplateFormat::AddGlobalFormat(const std::string &iFormatName, const std::string &iTemplate)
{
    NNATIVE_DEBUG("Adding new global URI template format " << iFormatName << " = \"" << iTemplate << "\"");
    std::map<std::string, UriTemplateFormat>::const_iterator it = GetGlobalFormats().find(iFormatName);
    if(it != GetGlobalFormats().end())
    {
        // Multiple format name register
        NNATIVE_DEBUG("Exists multiple global formats by name " << iFormatName);
        NNATIVE_ASSERT(0);
    }

    UriTemplateFormat newgrobalFormat(iFormatName, iTemplate);

    GetGlobalFormats().insert(std::make_pair(iFormatName, newgrobalFormat));
}
