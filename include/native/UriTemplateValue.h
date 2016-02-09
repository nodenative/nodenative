#ifndef _NATIVE_URITEMPLATEVALUE_H_
#define _NATIVE_URITEMPLATEVALUE_H_

#include <string>
#include <map>

namespace native {
    /**
     * @brief Key & Value URI template extracted value.
     *
     * @param iKey [description]
     * @param iValue [description]
     *
     * @return [description]
     */
    class UriTemplateValue
    {
    public:
        // type of child value container
        typedef std::map<std::string, UriTemplateValue> ValuesType;

        /// Default constructor
        UriTemplateValue() {}

        /**
         * @brief constructor
         *
         * @param iName value name
         * @param iValue value string
         */
        UriTemplateValue(const std::string &iName, const std::string &iValue);

        /// Returns value name
        std::string & getName() { return _name; }

        /// Returns value string
        std::string & getString() { return _value;}

        /// Clear value class and all child values
        void clear() {_name = ""; _value = ""; _childValues.clear(); }

        /// const method of return value name
        const std::string & getName() const { return _name; }

        /// const method of return value string
        const std::string & getString() const { return _value;}

        /**
         * @brief const version of access child value by name
         *
         * @param iName name to access child value
         * @return child value const reference
         */
        const UriTemplateValue & operator [](const std::string &iName) const;

        /**
         * @brief Return number of child values
         * @return number of child values
         */
        ValuesType::size_type size() const { return _childValues.size(); }

        /**
         * @brief Add a new child value in the current value
         *
         * @param iName child value name
         * @param iValue child value string
         *
         * @return the new created child value reference
         */
        UriTemplateValue & addChild(const std::string &iName, const std::string &iValue);

        /**
         * @brief Return child values
         * @return child values
        */
        const ValuesType & getChildValues() const { return _childValues; }

    private:
        /// value name
        std::string _name;
        /// value string
        std::string _value;
        // Child values container
        ValuesType _childValues;
    };
}

#endif //_NATIVE_URITEMPLATEVALUE_H_
