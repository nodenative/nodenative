#include "native/UriTemplateValue.hpp"
#include "native/helper/trace.hpp"

using namespace native;

UriTemplateValue::UriTemplateValue(const std::string &iName, const std::string &iValue) :
        _name(iName),
        _value(iValue)
{
}

const UriTemplateValue & UriTemplateValue::operator[](const std::string &iName) const
{
    ValuesType::const_iterator it = _childValues.find(iName);
    if(it == _childValues.end())
    {
        NNATIVE_DEBUG("Child URI template value not found by name \"" << iName << "\" in \"" << (_name.empty() ? "<no name>" : _name) << "\" with value \"" << getString() << "\"");
        NNATIVE_ASSERT(0);
    }
    return it->second;
}

UriTemplateValue & UriTemplateValue::addChild(const std::string &iName, const std::string &iValue)
{
    std::pair<ValuesType::iterator, bool> it = _childValues.insert(std::make_pair(iName, UriTemplateValue(iName, iValue)));
    if(!it.second)
    {
        NNATIVE_DEBUG("Child URI template value already exists by name \"" << iName << "\" in \"" << (_name.empty() ? "<no name>" : _name) << "\"");
        NNATIVE_ASSERT(0);
    }
    return it.first->second;
}

