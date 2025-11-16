#pragma once
#include "Basis.hpp"
#include <map>

class Collider {

    virtual void loadObject(const std::shared_ptr<Object>& object) = 0;
    virtual void loadObjects(const std::vector<std::shared_ptr<Object>>& objects) = 0;
    virtual void removeObject(const std::shared_ptr<Object>& object) = 0;
    virtual std::map<Object::TId, std::vector<Object::TId>> getIntersections() = 0;

};
