#include "pluginManager.h"

using namespace std;
using namespace micros_sensor_plugin;

int main(int argc, char * argv[])
{
    boost::shared_ptr<PluginManager> pmPtr;
      int ret;

    pmPtr.reset(new PluginManager());
  
    ret = pmPtr->loadSensorPlugins("../resources/twoInOne.xml");
    assert(!ret);

    cout<<"twoInOne.xml load successfully! "<<endl;
    
    cout<<"There are "<<pmPtr->getSensorNum()<<" sensor in "<<pmPtr->getSensorPluginFileNum()<<" file(s)!"<<endl;

    ret = pmPtr->parseSensorLabelMappingXML("../resources/twoInOne.xml");
    if(ret == 0)
    {
        cout<<"Parsing the mapping relationship configed in the twoInOne.xml finished! "<<endl;
    }

    ret = pmPtr->initSensorPlugins();
    if(ret == 0)
    {
        cout<<"Plugins configed in the twoInOne.xml initialized! "<<endl;
    }

    ret = pmPtr->enableSensorPlugins();
     if(ret == 0)
    {
        cout<<"Plugins configed in the twoInOne.xml is availabel now! "<<endl;
    }

    ret = pmPtr->startSensorPlugins();
    //这个执行过程中会在调用label插件的时候出错，因为sensor配置了label，但是找不到label的对象（因为没有解析labelXml）
    if(ret == 0)
    {
        cout<<"Plugins configed in the twoInOne.xml start successfully! "<<endl;
    }
    else
    {//输出这一项是我们预期的。
        cout<<"Start the sensor plugin failed!"<<endl;
    }
    
    getchar();
    sleep(50);
   // ret = pmPtr->loadLabelPlugins("../resources/plugin.xml");
//    pm.loadPlugins();
    
}