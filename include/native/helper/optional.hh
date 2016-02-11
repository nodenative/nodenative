#ifndef __NATIVE_HELPER_OPTIONAL_H__
#define __NATIVE_HELPER_OPTIONAL_H__

#include "../error.h"

namespace native {
namespace helper {

template<typename V>
class optional {
    bool _exist;
    V _value;
public:
    optional() : _exist(false) {}
    operator bool() { return _exist;}

    template<typename U>
    optional& operator=(U&& u) {
        _exist = true;
        _value = u;

        return *this;
    }

    V& value() &{
        NNATIVE_ASSERT(_exist);
        return _value;
    }

    const V& value() const&{
        NNATIVE_ASSERT(_exist);
        return _value;
    }

    const V&& value() const&&{
        NNATIVE_ASSERT(_exist);
        return _value;
    }
};

template<typename V>
class optional<V&> {
    bool _exist;
    V* _value;
public:
    optional() : _exist(false), _value(nullptr) {}
    operator bool() { return _exist;}

    template<typename U>
    optional& operator=(U& u) {
        _exist = true;
        _value = &u;

        return *this;
    }

    V& value() &{
        NNATIVE_ASSERT(_exist);
        return *_value;
    }

    const V& value() const&{
        NNATIVE_ASSERT(_exist);
        return _value;
    }
};

} /* namespace helper */
} /* namespace native */

#endif /* __NATIVE_HELPER_OPTIONAL_H__ */

