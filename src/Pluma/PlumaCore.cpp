#include "PlumaCore.hpp"

std::vector<std::string> PlumaCore::Application::_procArguments = {};
std::string PlumaCore::Application::_procRunPath = "";
std::string PlumaCore::Application::_procRunDir = "";
std::string PlumaCore::Application::_procRunName = "";
std::map<std::string,std::function<void(std::string&)>> PlumaCore::Application::_argsCalled = {};
std::map<std::string,std::function<void(void)>> PlumaCore::Application::_channelCalled = {};
std::map<std::string,std::string> PlumaCore::Application::_argsMapping = {};
PlumaCore::Debug *PlumaCore::Debug::_This = nullptr;
std::mutex PlumaCore::Debug::_mut;

PlumaCore::Application::Application(int &argc, char *argv[])
{
    for(int idx = 0; idx < argc; idx++)
    {
        if(idx == 0)
        {
            _procRunPath = replace(argv[0],"\\","/");
            size_t pos = _procRunPath.find_last_of("/");
            _procRunName = _procRunPath.substr(pos + 1,_procRunDir.size() - pos);
            _procRunDir = _procRunPath.substr(0,pos);
        }
        else
        {
            _procArguments.push_back(argv[idx]);
        }
    }

    auto itera = _procArguments.begin();
    for(;itera < _procArguments.end(); itera ++)
    {
        if(itera->find("-") == 0)
        {
            _argsMapping.insert({*itera,*(itera+1)});
        }
    }
}

std::vector<std::string> PlumaCore::Application::arguments()
{
    return _procArguments;
}

std::vector<std::string> PlumaCore::Application::channels()
{
    std::vector<std::string> result;
    for(auto val: _channelCalled)
    {
        result.push_back(val.first);
    }
    return result;
}

std::map<std::string, std::string> PlumaCore::Application::argumentMapping()
{
    return  _argsMapping;
}

std::string PlumaCore::Application::runPath()
{
    return _procRunPath;
}

std::string PlumaCore::Application::runDir()
{
    return  _procRunDir;
}

std::string PlumaCore::Application::name()
{
    return _procRunName;
}

void PlumaCore::Application::regArgsCalled(std::string flag, std::function<void (std::string &)> handler)
{
    _argsCalled.insert({flag,handler});
}

void PlumaCore::Application::regChannelCalled(std::string flag,std::function<void(void)> handler)
{
    _channelCalled.insert({flag,handler});
}

void PlumaCore::Application::argsCall()
{
    for(auto mapV : _argsCalled)
    {
        auto listItera = _procArguments.begin();
        while(listItera != _procArguments.end())
        {
            if(*listItera == mapV.first)
            {
                try
                {
                    mapV.second(*(listItera+1));
                }
                catch (std::exception& err)
                {
                    std::string errString = "arguments called error:" + std::string(err.what());
                    throw errString;
                }
            }
            listItera++;
        }
    }
}

void PlumaCore::Application::channelCall(std::string flag)
{
    auto func = _channelCalled[flag];
    if(func != nullptr)
    {
        try 
        {
            func();
        }
        catch (std::exception err) 
        {
            throw std::string("callback channnel") + FUNCTION_INFO + ":" + err.what();
        }
    }
    else 
    {
        PDebug() << "not found to call channel(process function)";
    }

    PDebug() << "";
}

std::string PlumaCore::Application::argsValue(std::string _key)
{
    return _argsMapping[_key];
}
