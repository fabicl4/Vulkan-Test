#pragma once

#include <string>
#include <unordered_map>
#include <cstdint>
#include <memory>

#include <type_traits>
#include <typeindex>
#include <typeinfo>

#include <core/Log.h>
#include <core/types.h>

// auto& tex = resources.get<Texture>("player"_hs);

template<typename T>
struct Handle
{
    explicit Handle(uint32_t _id) : id(_id) {};
    explicit Handle() : id(0) {};

    uint32_t id;
};

class IResourceManager {
public:
    virtual ~IResourceManager() = default;
};

template <typename T>
class ResourceManagerBase : public IResourceManager {
public:

    virtual ~ResourceManagerBase() {
        releaseAll();
    }

    /*
    template <typename... Args>
    virtual void load(Args &&...args) {}
    */

    virtual void release(const std::string&) = 0;
    
    void releaseAll() {
        for (auto& it: m_cache) {
            release(it.first);
        }
    }

    const T* get(const std::string& name) const {
        auto it = m_cache.find(name);
        if (it == m_cache.end()) {
            LOG_ERROR("Resource {} does not exist", name);
            return nullptr;
        }
        return &it->second;
    }

    T* get(const std::string& name) {
        auto it = m_cache.find(name);
        if (it == m_cache.end()) {
            LOG_ERROR("Resource {} does not exist", name);
            return nullptr;
        }
        return &it->second;
    }

protected:
    std::unordered_map<std::string, T> m_cache {};
};

template <typename T>
struct ResourceTraits {
    using Handle = Handle<T>;
    using Manager = ResourceManagerBase<T>;
};

class ResourceSystem {
public:
    ResourceSystem() {}

    ~ResourceSystem() {
        // TODO:
    }

    template <typename T, typename... Args>
    bool load(const std::string& name, Args &&...args) {
        auto& manager = getManager<T>();
        return manager.load(name, std::forward<Args>(args)...);
    }

    template <typename T>
    const T* get(const std::string& name) const {
        auto manager = getManager<T>();
        return manager->get(name);
    }

    template <typename T>
    T* get(const std::string& name) {
        auto manager = getManager<T>();
        return manager->get(name);
    }

    template<typename T>
    typename ResourceTraits<T>::Manager* getManager()
    {
        auto it = m_registry.find(typeid(T));
        if (it == m_registry.end())
            throw std::runtime_error("Manager not registered for this resource");

        return dynamic_cast<typename ResourceTraits<T>::Manager*>(it->second);
    }

    template<typename T>
    void registerManager(typename ResourceTraits<T>::Manager* manager) {
        m_registry.emplace(
            std::type_index(typeid(T)),
            std::move(manager)
        );
    }

private:
    std::unordered_map<std::type_index,
        IResourceManager* > m_registry {};
};