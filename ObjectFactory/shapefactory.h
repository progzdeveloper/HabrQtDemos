#pragma once
#include "shapes.h"
#include "objectfactory.h"
#include "genericfactory.h"

using ShapeFactory = ObjectFactory<String, Shape>;

// Mayers singletone
ShapeFactory& shapeFactory();

//
template<class _Shape, class... _Args>
struct ShapeRegisterer
{
    using KeyType = ShapeFactory::key_type;
    explicit ShapeRegisterer(const KeyType& k)
    {
        shapeFactory().registrate<_Shape, _Args...>(k);
    }
};

// Shape registration macro
#define REGISTRATE_SHAPE_CLASS(_Class, _Id, ...) \
    static const ShapeRegisterer<_Class, ##__VA_ARGS__> __##_Class##Registerer##_Id{ #_Class };
