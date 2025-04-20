#pragma once
#include <memory>
#include <algorithm>
#include <functional>
#include <unordered_map>
#include <type_traits>

#ifdef QT_VERSION
#if (QT_VERSION <= QT_VERSION_CHECK(5, 14, 0))
#include <QString>
#include <QHash>

namespace std
{
    template<>
    struct hash<QString>
    {
        size_t operator()(const QString& s) const noexcept {
            return qHash(s);
        }
    };
}
#endif
#endif

template<
    class _Key,
    class _Base,
    class... _Args
>
class GenericFactory
{
public:
    using Creator = std::function<_Base*(_Args&&...)>;
    using Storage = std::unordered_map<_Key, Creator>;
    using size_type = typename Storage::size_type;
    using key_type = _Key;
    using base_type = _Base;

    template<class _Product>
    void registrate(const _Key& key, const Creator& c)
    {
        static_assert (std::is_base_of_v<_Base, _Product>, "_Product must be derived from _Base");
        creators[key] = c;
    }

    template<class _Product>
    void registrate(const _Key& key)
    {
        static_assert (std::is_base_of_v<_Base, _Product>, "_Product must be derived from _Base");
        creators[key] = [](_Args&&... args) { return new _Product(std::forward<_Args>(args)...); };
    }

    [[nodiscard]] _Base* create(const _Key& key, _Args&&... args) const
    {
        auto it = creators.find(key);
        return it != creators.cend() ? (it->second)(std::forward<_Args>(args)...) : nullptr;
    }

    template<class _OutIt>
    _OutIt keys(_OutIt out) const
    {
        return std::transform(creators.cbegin(), creators.cend(), out, [](const auto& p) { return p.first; });
    }

    bool empty() const noexcept { return creators.empty(); }

    size_type count(const _Key& k) const noexcept { return creators.count(k); }

    size_type size() const noexcept { return creators.size(); }

private:
    Storage creators;
};
