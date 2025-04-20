#pragma once
#include <cstring> // for memcmp
#include <memory>
#include <algorithm>
#include "metaobjectutils.h"

#ifdef QT_VERSION
#include <QList>
#include <QVarLengthArray>
#include <QMetaType>

class QMetaObject;

#define MetaTypeId(T) qMetaTypeId<T>()

#else // Non-Qt version
#include <vector>

template<typename T>
struct TypeId;

template<>
struct TypeId<void>
{
protected:
    static int counter;
};
inline int TypeId<void>::counter = 0;

template<typename T>
struct TypeId : TypeId<void> {
    static inline int id = [](){
           return TypeId<void>::counter++;
    }();
};

#define MetaTypeId(T) (TypeId<T>::id)

#endif


template<
    class _Key,
    class _Base,
    int _Prealloc = 8
>
class ObjectFactory
{
    class CreatorBase
    {
    public:
        CreatorBase(const _Key& k, size_t n)
            : key_(k)
            , size_(n)
        {}

        virtual ~CreatorBase() = default;
#ifdef QT_VERSION
        virtual const QMetaObject* metaObject() const { return nullptr; }
#endif
        virtual const int* argsMeta() const noexcept { return nullptr; }
        virtual int argsCount() const noexcept { return 0; }

        bool isMatched(const _Key& k, const int* args, int n) const
        {
            if (k != key_ || n != argsCount())
                return false;

            if (args && argsMeta())
                return memcmp(args, argsMeta(), n) == 0;
            return (!args && !argsMeta());
        }

        const _Key& key() const { return key_; }

    protected:
        _Key key_;
        const size_t size_;
    };


    template<class... _Args>
    class ClassCreator : public CreatorBase
    {
        static constexpr int kArgsCount = sizeof...(_Args);
        static const int kArgsMeta[sizeof...(_Args)];
    public:
        ClassCreator(const _Key& k, size_t n)
            : CreatorBase(k, n)
        {}

        const int* argsMeta() const noexcept override { return kArgsMeta; }
        int argsCount() const noexcept override { return kArgsCount; }

        template<class _Allocator>
        inline _Base* create(const _Allocator& al, _Args&&... args) const
        {
            // rebind allocator
            using allocator_type = typename std::allocator_traits<_Allocator>::template rebind_alloc<char>;
            using alloc_traits = std::allocator_traits<allocator_type>;

            allocator_type alloc_proxy(al);
            _Base* result = nullptr;
            char* addr = nullptr;
            try
            {
                // allocation and/or construction may throw
                addr = alloc_traits::allocate(alloc_proxy, this->size_);
                result = construct(addr, std::forward<_Args>(args)...);
            }
            catch (...)
            {
                alloc_traits::deallocate(alloc_proxy, addr, this->size_);
                addr = nullptr;
                throw;
            }

            return result;
        }

    protected:
        virtual _Base* construct(void*, _Args&&...) const = 0;
    };


    template<class _Product, class... _Args>
    class Creator : public ClassCreator<_Args...>
    {
    public:
        Creator(const _Key& k)
            : ClassCreator<_Args...>(k, sizeof(_Product))
        {}
 #ifdef QT_VERSION
        // \reimp from CreatorBase
        const QMetaObject* metaObject() const override
        {
            if constexpr (HasQtMetaObject<_Product>::value)
                return &_Product::staticMetaObject;
            else
                return nullptr;
        }
#endif
        // \reimp from ClassCreator<...>
        _Base* construct(void* addr, _Args&&... args) const override
        {
            return new(addr) _Product(std::forward<_Args>(args)...);
        }
    };

    template<class _Product, class... _Args>
    struct ClassRegisterer
    {
        static void registrate(ObjectFactory& factory, const _Key& key)
        {
            static_assert (std::is_base_of_v<_Base, _Product>, "_Product must be derived from _base type");
            static constexpr int argc = sizeof...(_Args);
            static const int argv[] = { MetaTypeId(_Args)... };
            for (const auto& c : std::as_const(factory.creators))
            {
                if (c->isMatched(key, argv, argc))
                    return;
            }
            factory.creators.push_back(std::make_unique<Creator<_Product, _Args...>>(key));
        }
    };

    template<class _Product, class... _Args>
    struct ClassRegisterer<_Product(_Args...)>
        : public ClassRegisterer<_Product, _Args...>
    {};

public:
    using key_type = _Key;
    using base_type = _Base;

    using AllocatorType = std::allocator<char>;
    using CreatorPointer = std::unique_ptr<CreatorBase>;
#ifdef QT_VERSION
    using CreatorStorage = QVarLengthArray<CreatorPointer, _Prealloc>;
#else
    using CreatorStorage = std::vector<CreatorPointer>;
#endif
    using size_type = typename CreatorStorage::size_type;


    template<class _Product, class... _Args>
    inline typename std::enable_if_t<!std::is_function_v<_Product>, void> registrate(const _Key& key)
    {
        ClassRegisterer<_Product, _Args...>::registrate(*this, key);
    }

    template<class _Fn>
    inline typename std::enable_if_t<std::is_function_v<_Fn>, void> registrate(const _Key& key)
    {
        ClassRegisterer<_Fn>::registrate(*this, key);
    }

    template<class... _Args>
    bool contains(const _Key& key) const noexcept
    {
        static constexpr int argc = sizeof...(_Args);
        static const int argv[] = { MetaTypeId(std::decay_t<_Args>)... };
        for (const auto& c : creators)
        {
            if (c->isMatched(key, argv, argc))
                return true;
        }
        return false;
    }
 #ifdef QT_VERSION
    template<class _Container = QList<_Key>>
#else
    template<class _Container = std::vector<_Key>>
#endif
    _Container keys() const
    {
        _Container result;
        for (const auto& c : creators)
            result.push_back(c->key());

        std::sort(result.begin(), result.end());
        result.erase(std::unique(result.begin(), result.end()), result.end());
        return result;
    }

    size_type count(const _Key& key) const noexcept
    {
        int n = 0;
        for (const auto& c : creators)
            n += (c->key() == key);
        return n;
    }

    size_type size() const noexcept
    {
        return creators.size();
    }

 #ifdef QT_VERSION
    const QMetaObject* metaData(const _Key& key) const noexcept
    {
        for (const auto& c : creators)
        {
            if (c->key() == key)
                return c->metaObject();
        }
        return nullptr;
    }
 #endif

    template<class _Allocator, class... _Args>
    [[nodiscard]] _Base* construct(const _Allocator& al, const _Key& key, _Args... args) const
    {
        using CreatorType = ClassCreator<_Args...>;
        static constexpr int argc = sizeof...(_Args);
        static const int argv[] = { MetaTypeId(std::decay_t<_Args>)... };
        for (const auto& c : creators)
        {
            if (c->isMatched(key, argv, argc))
            {
                const CreatorType* creator = reinterpret_cast<const CreatorType*>(c.get());
                return creator->create(al, std::forward<_Args>(args)...);
}
        }
        return nullptr;
    }

    template<class... _Args>
    [[nodiscard]] _Base* create(const _Key& key, _Args... args) const
    {
        return this->construct(AllocatorType{}, key, std::forward<_Args>(args)...);
    }

private:
    CreatorStorage creators;
};

template<class _Key, class _Base, int _Prealloc>
template<class... _Args>
const int ObjectFactory<_Key, _Base, _Prealloc>::ClassCreator<_Args...>::kArgsMeta[sizeof...(_Args)] = { MetaTypeId(_Args)... };
