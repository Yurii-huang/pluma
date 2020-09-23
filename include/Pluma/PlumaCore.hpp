#ifndef PLUMACORE_H
#define PLUMACORE_H

#include <iostream>
#include <functional>
#include <map>
#include <thread>
#include <list>
#include <string>
#include <vector>
#include <exception>
#include <mutex>
#include <inttypes.h>
#include <array>

#ifdef WIN32
#define __SysWorkDirCmd "chdir"
#elif linux
#define __SysWorkDirCmd "pwd"
#else
#define __SysWorkDirCmd "echo unknown directory execute command!"
#endif

namespace  PlumaCore //Pluma内核
{

const class _ts_t{ public: inline void tFunc(){} }_classSizeCheck;
const size_t _SysClassPSize = sizeof(&_classSizeCheck);
const size_t _SysClassFSize = sizeof(&_ts_t::tFunc);

typedef std::array<unsigned char,_SysClassPSize> ClassVarient;  //类万能转换类
typedef std::array<unsigned char,_SysClassFSize> MethodVarient; //方法万能转换类

/// <summary>
/// 任意类型转换成格式化地址 如:std::array<unsigned char,num>
/// </summary>
/// <typeparam name="T"> 模板T 可以传递任意类型</typeparam>
/// <param name="elem"> 模板变量T 的形参 此参数必须重载operarot =() </param>
/// <returns></returns>
template<typename T>
static std::array<unsigned char,sizeof(T)> _ConvtAnyBAToFormat(const T elem)  //Type has operator=()
{
    union Convt_t
    {
        T Base;
        std::array<unsigned char,sizeof(T)> iBase;
    }result;
    result.Base = elem;
    return result.iBase;
}

template<typename T>
static T _ConvtAnyFormatToBA(std::array<unsigned char,sizeof(T)> elem)  //Type has operator=()
{
    union Convt_t
    {
        T Base;
        std::array<unsigned char,sizeof(T)> iBase;
    }result;
    result.iBase = elem;
    return result.Base;
}

template<typename Ct,typename Rt, typename...Args>
class Functor
{
    typedef Rt(Ct::*Method_t)(Args...);
    std::array<unsigned char,sizeof(Method_t)> _methodBase;
    std::array<unsigned char,sizeof(Ct*)> _classBase;

public:
    explicit Functor()
    {

    }

    Functor(Ct* clazz, Method_t method)
    {
        _classBase = PlumaCore::_ConvtAnyBAToFormat<Ct*>(clazz);
        _methodBase = PlumaCore::_ConvtAnyBAToFormat<Method_t>(method);
    }

    void operator = (Functor& func)
    {
        this->_methodBase == func.getMethodBase();
        this->_classBase == func.getClassBase();
    }

    void getMethodBase()
    {
        return _methodBase;
    }

    void getClassBase()
    {
        return _classBase;
    }

    Rt call(Args...as)
    {
        using namespace PlumaCore;
        return (_ConvtAnyFormatToBA<Ct*>(_classBase)->*_ConvtAnyFormatToBA<Method_t>(_methodBase))(as...);
    }

};


static std::string replace(std::string src,std::string reg,std::string rep)
{
    std::string result;

    while(src.find(reg) > 0 && src.find(reg) < src.size())
    {
        size_t fpos = src.find(reg);
        src.replace(fpos,reg.size(),rep);
    }
    return src;
}

class Application
{
    static std::vector<std::string> _procArguments;
    static std::string _procRunPath;
    static std::string _procRunDir;
    static std::string _procRunName;
    static std::map<std::string,std::function<void(std::string&)>> _argsCalled;
    static std::map<std::string,std::string> _argsMapping;
    static std::map<std::string,std::function<void(void)>> _channelCalled;

public:
    explicit Application(int& argc,char* argv[]);

    static std::vector<std::string> arguments();

    static std::vector<std::string> channels();

    static std::map<std::string,std::string> argumentMapping();

    static std::string argsValue(std::string _key);

    static std::string runPath();

    static std::string runDir();

    static std::string name();

    static void regArgsCalled(std::string flag,std::function<void(std::string&)> handler);

    static void regChannelCalled(std::string flag,std::function<void(void)> handler);

    static void channelCall(std::string flag);

    static void argsCall();
};

/////////////////////////////////////////////////////////////////////////////////
// Class：Debug
// 实现功能：
// 1.线程输出的互斥锁
// 2.结尾自动换行
// 3.支持所有std::cout 输出范式
// 4.文件头结尾宏封装 PDebug
// 5.std::vec 和std::list 等容器的基本输出
// 使用方法：
// PDebug() << any;
/////////////////////////////////////////////////////////////////////////////////
class Debug
{
    static Debug *_This;
    static std::mutex _mut;

public:
    static Debug& getInstance()
    {
        std::cout << std::dec << std::endl;
        if(_This == nullptr)
        {
            _This = new Debug();
        }
        return *_This;
    }

    template<typename T>
    Debug& operator <<(const std::vector<T>& _vec) const
    {
        bool res = _mut.try_lock();
        int count = _vec.size();
        std::cout << "std::vector(";
        for(auto ite: _vec)
        {
            *this<< ite;
            if(count > 1)
            {
                *this<< ",";
                count --;
            }
        }
        std::cout << ")";
        if(res) _mut.unlock();
        return * _This;
    }

    template<typename T>
    Debug& operator <<(const std::list<T>& _list)
    {
        bool res = _mut.try_lock();
        int count = _list.size();
        std::cout << "std::list(";
        for(auto ite: _list)
        {
            *this<< ite;
            if(count > 1)
            {
                *this<< ",";
                count --;
            }
        }
        std::cout << ")";
        if(res) _mut.unlock();
        return * _This;
    }

    template<typename T>
    Debug& operator <<(const T& t) const
    {
        bool res = _mut.try_lock();
        std::cout << t;
        if(res) _mut.unlock();
        return *_This;
    }

    template<typename K,typename V>
    Debug& operator <<(const std::map<K,V>& _map) const
    {
        bool res = _mut.try_lock();
        std::cout << "std::map(";
        int count = _map.size();
        for(auto ite = _map.begin(); ite != _map.end(); ite++)
        {
            *this << ite->first << ":" << ite->second ;
            if(count > 1)
            {
                *this << "," ;
                count --;
            }
        }
        std::cout << ")";
        if(res) _mut.unlock();
        return *_This;
    }

    template<typename K,typename V>
    Debug& operator <<(const std::multimap<K,V>& _map) const
    {
        bool res = _mut.try_lock();
        std::cout << "std::multimap(";
        int count = _map.size();
        for(auto ite: _map)
        {
            *this << ite.first << ":" << ite.second;
            if(count > 1)
            {
                *this << "," ;
                count --;
            }
        }
        std::cout << ")";
        if(res) _mut.unlock();
        return *_This;
    }

    Debug& operator << (const char* t)const
    {
        bool res = _mut.try_lock();
        if(std::string(t).size() == 1
                && ((0x30 <= t[0] && t[0] <= 0x39)
                     || (0x41 <= t[0] && t[0] <= 0x5A)
                     || (0x61 <= t[0] && t[0] <= 0x7A))) //数字 小写字母 大写字母
        {
            std::cout <<"\"" << t << "\"" << " ";
        }
        else std::cout << t;
        if(res) _mut.unlock();
        return *_This;
    }

    Debug& operator << (const std::basic_string<char> t)const
    {
        bool res = _mut.try_lock();
        std::cout <<"\"" << t << "\"";
        if(res) _mut.unlock();
        return *_This;
    }


private:
    explicit Debug(){}
    virtual ~Debug(){}
};

}

#define PDebug PlumaCore::Debug::getInstance

// __FUNCSIG is MSVC definitions print current function name
// __PRETTY_FUNCTION__ is mingw definitions print current function name
#ifdef WIN32 
#define FUNCTION_INFO __FUNCSIG__
#elif linux 
#define FUNCTION_INFO __PRETTY_FUNCTION__ 
#endif // WIN32|WIN64

#endif // PLUMACORE_H
