#include "pluginManager.h"

using namespace std;
//using namespace micros_label_gen;

int main(int argc, char * argv[])
{
    boost::shared_ptr<PluginManager> pmPtr;
    pmPtr.reset(new PluginManager());
    int testNum = 0;
    int ret;

    cout<<"------------------Begin to the sensor plugin xml parsing test------------------"<<endl;
    
    cout<<"------------------Test:#"<<testNum++<<" full elements test--------------------------"<<endl;
    ret = pmPtr->parseSensorXML("../resources/sensorPluginFullElementTest.xml");
    
    assert(ret == 0); 
    cout<<"------------------Test:#"<<testNum<<" full elements test finished!----------------"<<endl;

    cout<<"------------------Test:#"<<testNum++<<" missing sensorPlugins element test---------"<<endl;
    ret = pmPtr->parseSensorXML("../resources/sensorPluginWithoutSensorPluginsElementTest.xml");
    
    assert(ret == 0); 
    cout<<"------------------Test:#"<<testNum<<" missing sensorPlugins element test finished!--"<<endl;
    
    ///////////////////////////////////other tests///////////////////////////////

    cout<<"------------------The sensor plugin xml parsing test finished!----------------------------"<<endl;
    getchar();
    sleep(50);
   // ret = pmPtr->loadLabelPlugins("../resources/plugin.xml");
//    pm.loadPlugins();
    
}