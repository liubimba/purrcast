//
// Created by bimba on 11/15/25.
//

#ifndef ISERVICE_HPP
#define ISERVICE_HPP

class IService
{
public:
    virtual ~IService() = default;
    virtual bool available() = 0;
};
#endif //ISERVICE_HPP
