//
// Created by bimba on 11/15/25.
//

#include "Tests.hpp"

class Service : public IService
{
public:
    Service() = default;
    ~Service() override = default;

    bool available() override
    {
        return true;
    }
};

TEST(Services, add)
{
    Services services;
    auto service = std::make_shared<Service>();
    ASSERT_ANY_THROW(services.add<Service>(nullptr));
    services.add<Service>(service);
    ASSERT_ANY_THROW(services.add<Service>(service));
}

TEST(Services, get)
{
    Services services;
    auto service = std::make_shared<Service>();
    services.add<Service>(service);
    ASSERT_EQ(service, services.get<Service>());
}

TEST(Services, has)
{
    Services services;
    auto service = std::make_shared<Service>();
    services.add<Service>(service);
    ASSERT_TRUE(services.has<Service>());
}
