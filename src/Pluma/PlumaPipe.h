#ifndef PLUMAPIPE_H
#define PLUMAPIPE_H

#include <list>
#include <functional>
#include <iostream>
#include <thread>
#include <bitset>
#include <map>

#include "PlumaCore.h"

template<typename...Args>
class PlumaPipe
{
    typedef std::multimap<PlumaCore::MethodVarient,std::function<void(Args...)>> CallBackMapT; //函数转发列表
    CallBackMapT _mapping;

public:
    enum ConnType //size is one
    {
        Ct_CallBack = 0b00000001,     //默认回调 defualt
        Ct_ThJoined = 0b00000011,     //线程等待
        Ct_ThDirect = 0b00000010,     //线程分离
        Ct_OnlyOne  = 0b00000100      //只有绑定一次
    };

    explicit PlumaPipe() //has defualt value
    {

    }

    virtual ~PlumaPipe()
    {

    }

    template<typename Ct,typename Ft = void(Ct::*)(Args...)>
    void connect(Ct* handler,Ft func,ConnType type = Ct_CallBack)
    {

        switch (type)
        {
        case Ct_CallBack:

            _mapping.insert({PlumaCore::_ConvtAnyBAToFormat<Ft>(func),
                             [=](Args...as)
                             {
                                 (handler->*func)(as...);
                             }
                            });

            break;

        case Ct_ThJoined:

            _mapping.insert({PlumaCore::_ConvtAnyBAToFormat<Ft>(func),
                             [=](Args...as){
                                 std::thread th(func,handler,as...);
                                 th.join();
                             }});

            break;

        case Ct_ThDirect:

            _mapping.insert({PlumaCore::_ConvtAnyBAToFormat<Ft>(func),
                             [=](Args...as)
                             {
                                 std::thread th(func,handler,as...);
                                 th.detach();
                             }
                            });

            break;

        case Ct_OnlyOne:
            if(_mapping.find(PlumaCore::_ConvtAnyBAToFormat<Ft>(func)) != _mapping.end())
                return;
            else
            {
                _mapping.insert({PlumaCore::_ConvtAnyBAToFormat<Ft>(func),
                                 [=](Args...as)
                                 {
                                     (handler->*func)(as...);
                                 }
                                });
            }
            break;

        default:
            throw "Error, can't connect class method,case connect type is unkonwn";
            break;
        }
    }

    template<typename Ct,typename Ft = void(Ct::*)(Args...)>
    void disconnect(Ct* handler,Ft func)
    {
        _mapping.erase(PlumaCore::_ConvtAnyBAToFormat<Ft>(func));
    }

    void send(Args...as)
    {
        for(auto var:_mapping)
        {
            (var.second)(as...);
        }
    }
};

#endif // PLUMAPIPE_H
