#include "native/UriTemplateValue.hpp"
#include "native/helper/trace.hpp"

using namespace native;

UriTemplateValue::UriTemplateValue(const std::string &iName, const std::string &iValue)
    : _name(iName), _value(iValue) {}

UriTemplateValue::UriTemplateValue(const UriTemplateValue &other)
    : _name(other._name), _value(other._value), _childValues(other._childValues) {}

UriTemplateValue::UriTemplateValue(UriTemplateValue &&other)
    : _name(other._name), _value(other._value), _childValues(other._childValues) {}

const UriTemplateValue &UriTemplateValue::operator[](const std::string &iName) const {
  ValuesType::const_iterator it = _childValues.find(iName);
  if (it == _childValues.end()) {
    NNATIVE_DEBUG("Child URI template value not found by name \""
                  << iName << "\" in \"" << (_name.empty() ? "<no name>" : _name) << "\" with value \"" << getString()
                  << "\"");
    NNATIVE_ASSERT(0);
  }
  return it->second;
}

UriTemplateValue &
UriTemplateValue::addChild(const std::string &iName, const std::string &iValue, const bool replaceIfExists) {
  std::pair<ValuesType::iterator, bool> it =
      _childValues.insert(std::make_pair(iName, UriTemplateValue(iName, iValue)));
  if (!it.second) {
    NNATIVE_DEBUG("Child URI template value already exists by name \""
                  << iName << "\" in \"" << (_name.empty() ? "<no name>" : _name) << "\"");
    if (replaceIfExists) {
      NNATIVE_DEBUG("replacing it");
      it.first->second._value = iValue;
    } else {
      NNATIVE_ASSERT(0);
    }
  }
  return it.first->second;
}

bool UriTemplateValue::renameChild(const std::string &oldName, const std::string &newName) {
  ValuesType::iterator itOld = _childValues.find(oldName);
  if (itOld == _childValues.end()) {
    NNATIVE_DEBUG("child by name " << oldName << " does not exists. skip renaiming from " << oldName);
    return false;
  }

  ValuesType::const_iterator itNew = _childValues.find(oldName);
  if (itNew != _childValues.end()) {
    NNATIVE_DEBUG("child by name " << newName << " already exists. skip renaiming from " << oldName);
    return false;
  }

  ValuesType::value_type newKey(newName, std::move(itOld->second));
  _childValues.insert(std::move(newKey));
  _childValues.erase(itOld);

  return true;
}

bool UriTemplateValue::removeChild(const std::string &name) {
  ValuesType::iterator it = _childValues.find(name);
  if (it == _childValues.end()) {
    NNATIVE_DEBUG("child by name " << name << " does not exists");
    return false;
  }

  _childValues.erase(it);

  return true;
}
