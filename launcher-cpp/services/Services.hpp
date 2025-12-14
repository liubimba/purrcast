//
// Created by bimba on 11/15/25.
//

#ifndef SERVICES_HPP
#define SERVICES_HPP
#include <memory>
#include <typeindex>
#include <unordered_map>
#include "IService.hpp"
#include "spdlog/spdlog.h"
#include "spdlog/sinks/ansicolor_sink.h"

class Services
{
public:
    Services() = default;

    ~Services() = default;

    template <typename T>
    std::shared_ptr<T> get() const
    {
        static_assert(std::is_base_of<IService, T>::value, "T must inherit from IService");
        const std::type_index key(typeid(T));
        auto it = services_.find(key);
        if (it == services_.end())
        {
            throw std::runtime_error("Service not found");
        }
        return std::static_pointer_cast<T>(it->second);
    }

    template <typename T>
    void add(std::shared_ptr<T> service)
    {
        static_assert(std::is_base_of<IService, T>::value, "T must inherit from IService");
        if (service == nullptr)
        {
            throw std::runtime_error("passed service is null pointer");
        }
        const std::type_index key(typeid(T));
        auto it = services_.find(key);
        if (it != services_.end())
            throw std::runtime_error("Service already exists");
        services_[key] = std::move(service);
    }

    template <typename T>
    bool has() const
    {
        static_assert(std::is_base_of<IService, T>::value, "T must inherit from IService");
        const std::type_index key(typeid(T));
        return services_.find(key) != services_.end();
    }

private:
    std::unordered_map<std::type_index, std::shared_ptr<void>> services_;
};
#endif //SERVICES_HPP
