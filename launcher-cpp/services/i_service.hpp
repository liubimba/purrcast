//
// Created by bimba on 11/15/25.
//

#ifndef ISERVICE_HPP
#define ISERVICE_HPP

class i_service
{
public:
    virtual ~i_service() = default;
    virtual bool available() = 0;
};
#endif //ISERVICE_HPP
