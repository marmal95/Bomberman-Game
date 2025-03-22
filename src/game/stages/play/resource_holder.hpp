#pragma once

#include <cassert>
#include <map>
#include <memory>
#include <stdexcept>
#include <string>

template <typename Resource, typename Identifier> class ResourceHolder
{
  private:
    std::map<Identifier, std::unique_ptr<Resource>> mResourceMap;

  public:
    ResourceHolder();
    void load(Identifier id, const std::string& filename);
    void load(Identifier id, std::unique_ptr<Resource>&& resource);
    Resource& getResource(Identifier id);
    const Resource& getResource(Identifier id) const;
};

template <typename Resource, typename Identifier>
ResourceHolder<Resource, Identifier>::ResourceHolder() : mResourceMap()
{
}

template <typename Resource, typename Identifier>
void ResourceHolder<Resource, Identifier>::load(Identifier id, const std::string& filename)
{
    auto uPtr = std::make_unique<Resource>();
    if (!uPtr->loadFromFile(filename))
        throw std::runtime_error("Could not find resource:" + filename);

    mResourceMap.insert(std::make_pair(id, std::move(uPtr)));
}

template <typename Resource, typename Identifier>
void ResourceHolder<Resource, Identifier>::load(Identifier id, std::unique_ptr<Resource>&& resource)
{
    mResourceMap.insert(std::make_pair(id, std::move(resource)));
}

template <typename Resource, typename Identifier>
Resource& ResourceHolder<Resource, Identifier>::getResource(Identifier id)
{
    auto found = mResourceMap.find(id);
    return *found->second;
}

template <typename Resource, typename Identifier>
const Resource& ResourceHolder<Resource, Identifier>::getResource(Identifier id) const
{
    auto found = mResourceMap.find(id);
    return *found->second;
}