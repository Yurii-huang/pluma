#include <iostream>
#include "PlumaCore.h"
#include "PlumaPipe.h"

//__PRETTY_FUNCTION__ is mingw definitions print current function name

class test2
{
public:
    test2(){}

    int rihandler(int ar){PDebug() << __PRETTY_FUNCTION__ << ar; return ar*2;}

    void handler(int ar){PDebug() << __PRETTY_FUNCTION__ << ar;}

    void handler(){PDebug()<<__PRETTY_FUNCTION__;}

    void handlerTh1Join(){PDebug()<<__PRETTY_FUNCTION__;}

    void handlerTh2Join(){PDebug() << __PRETTY_FUNCTION__;}

    void handlerTh1Dirc()
    {
        while(1){PDebug()<<__PRETTY_FUNCTION__;}
    }

    void handlerTh2Dirc()
    {
        while(1){PDebug()<<__PRETTY_FUNCTION__;}
    }

    static void shandler(int ar){PDebug() << __PRETTY_FUNCTION__ << ar;}
};


static void testFunc()
{
    std::cout << __PRETTY_FUNCTION__ << std::endl;
}

//argument -Core in Args
static void coreProc(std::string& coreName)
{
    std::cout<< __PRETTY_FUNCTION__ << coreName << std::endl;
}

//argument -Plugin in Args
static void pluginPathProc(std::string& pluginPath)
{
    std::cout<< __PRETTY_FUNCTION__ << pluginPath << std::endl;
}

static void interfaceProc(std::string& interfaces)
{
    std::cout<< __PRETTY_FUNCTION__ << interfaces << std::endl;
}

static void deviceProc(std::string& devices)
{
    std::cout<< __PRETTY_FUNCTION__ << devices << std::endl;
}

/* 0: defualt model is keypress 存在输入提示
 * 1: process model not keypress 关闭输入提示
 * */
static int s_RunModel = 0;
static void runmodelProc(std::string& model)
{
    if(model == "ShowKeyPress")
        s_RunModel = 0;

    if(model == "HideKeyPress")
        s_RunModel = 1;

    if(model == "JsonFormat")
        s_RunModel = 2;
}



static void readChannel()
{
    PDebug()<<__PRETTY_FUNCTION__;
}

static void writeChannel()
{
    PDebug()<<__PRETTY_FUNCTION__;
}

static void helpChannel()
{
    std::map<std::string,std::string> helpInfor;

    helpInfor.insert({
                         "help",
                         "show command help"
                     });

    helpInfor.insert({
                         "readChannel",
                         std::string("std::cout from current process out data") +
                         ", current application: " +
                         PlumaCore::Application::name()
                     });

    helpInfor.insert({
                         "writeCahnnel",
                         std::string("need transfer data to current process") +
                         ", current application: " +
                         PlumaCore::Application::name()
                     });

    PDebug() << "command help:";
    for(auto val: helpInfor)
    {
        PDebug() << "  " << val.first << " : " << val.second;
    }
}

int main(int argc, char *argv[])
{
    PlumaCore::Application a(argc,argv);

    for(auto var: PlumaCore::Application::arguments()){std::cout << var << std::endl;}
    std::cout << "\n" << std::endl;
    std::cout << PlumaCore::Application::runPath() << std::endl;
    std::cout << PlumaCore::Application::runDir()  << std::endl;
    std::cout << PlumaCore::Application::name()    << std::endl;

    PlumaCore::Application::regArgsCalled("-interface",interfaceProc);
    PlumaCore::Application::regArgsCalled("-pluginPath",pluginPathProc);
    PlumaCore::Application::regArgsCalled("-core",coreProc);
    PlumaCore::Application::regArgsCalled("-devices",deviceProc);
    PlumaCore::Application::regArgsCalled("-runModel",runmodelProc);
    PlumaCore::Application::argsCall();

    PDebug() << std::hex << std::uppercase << 255 << 10 << 16;
    PDebug() << PlumaCore::Application::argumentMapping();
    PDebug() << PlumaCore::Application::arguments();
    PDebug() << std::hex << std::list<int>{255,10,16};
    PDebug() << std::list<std::string>{"hhh","ggg","jjj"};
    PDebug() << std::vector<int>{2,3,4};

    auto foo = [=](){std::cout << __PRETTY_FUNCTION__ << std::endl;};

    test2 t2;

    std::array<unsigned char,sizeof(test2*)> classBA = PlumaCore::_ConvtAnyBAToFormat(&t2);
    PlumaCore::_ConvtAnyFormatToBA<test2*>(classBA)->handler(10);


    std::array<unsigned char,sizeof(void (test2::*)(int))> handlerBA = PlumaCore::_ConvtAnyBAToFormat<void (test2::*)(int)>(&test2::handler);  //类型需要指定否则函数重载将不会被识别
    (t2.*PlumaCore::_ConvtAnyFormatToBA<void (test2::*)(int)>(handlerBA))(20);
    PlumaCore::Functor<test2,void,int>(&t2,&test2::handler).call(1000);


    std::array<unsigned char,sizeof(void (test2::*)())> handlerBA1 = PlumaCore::_ConvtAnyBAToFormat<void (test2::*)()>(&test2::handler); //空参数
    (t2.*PlumaCore::_ConvtAnyFormatToBA<void (test2::*)()>(handlerBA1))();
    PlumaCore::Functor<test2,void>(&t2,&test2::handler).call();


    std::array<unsigned char,sizeof(int (test2::*)(int))> rihandler = PlumaCore::_ConvtAnyBAToFormat<int (test2::*)(int)>(&test2::rihandler);
    PDebug() << (t2.*PlumaCore::_ConvtAnyFormatToBA<int (test2::*)(int)>(rihandler))(30) ;
    PDebug() << PlumaCore::Functor<test2,int,int>(&t2,&test2::rihandler).call(100);


    std::array<unsigned char,sizeof(void (*)(int))> shandlerBA = PlumaCore::_ConvtAnyBAToFormat(&test2::shandler);         //静态函数
    (*PlumaCore::_ConvtAnyFormatToBA<decltype(&test2::shandler)>(shandlerBA))(40);

    (PlumaCore::_ConvtAnyFormatToBA<test2*>(classBA)->*PlumaCore::_ConvtAnyFormatToBA<void (test2::*)()>(handlerBA1))();   //索引调用

    PDebug() << PlumaCore::_SysClassPSize << "|" << PlumaCore::_SysClassFSize;
    PDebug() << sizeof(&test2::rihandler);

    PlumaPipe<> pipe;
    pipe.connect(&t2,&test2::handler);
    pipe.connect(&t2,&test2::handler);
    pipe.connect(&t2,&test2::handler);

    pipe.connect(&t2,&test2::handlerTh1Join,PlumaPipe<>::Ct_ThJoined);
    pipe.connect(&t2,&test2::handlerTh2Join,PlumaPipe<>::Ct_ThJoined);

    //    pipe.connect(&t2,&test2::handlerTh1Dirc,PlumaPipe<>::Ct_ThDirect);
    //    pipe.connect(&t2,&test2::handlerTh2Dirc,PlumaPipe<>::Ct_ThDirect);

    pipe.send();

    pipe.disconnect(&t2,&test2::handler);

    pipe.send();

    PlumaPipe<int> pipe1;
    pipe1.connect(&t2,&test2::handler);
    pipe1.connect(&t2,&test2::handler);
    pipe1.connect(&t2,&test2::handler);
    pipe1.send(30);

    //注册交互回调
    PlumaCore::Application::regChannelCalled("readChannel",readChannel);
    PlumaCore::Application::regChannelCalled("writeChannel",writeChannel);
    PlumaCore::Application::regChannelCalled("help",helpChannel);

    PDebug() << "Any channel keypress from :\n" << PlumaCore::Application::channels();

    while(1)
    {
        std::string str;
        if(0 == s_RunModel)
        {
            PDebug() << "keypress:";
        }
        std::cin >> str;

        if(0 == s_RunModel)
        {
            PDebug() << "result:" << str;
        }

        a.channelCall(str);
    }

    return 0;
}
