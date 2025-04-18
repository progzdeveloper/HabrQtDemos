#pragma once
#include <type_traits>

#ifdef QT_VERSION
// SFINAE Trait that tells is a the Object has a staticMetaObject member
template <class T>
class HasQtMetaObject
{
    template<class U, class = typename std::enable_if<!std::is_member_pointer<decltype(&U::staticMetaObject)>::value>::type>
    static std::true_type check(int);
    template <class>
    static std::false_type check(...);
public:
    static constexpr bool value = decltype(check<T>(0))::value;
};
#endif
