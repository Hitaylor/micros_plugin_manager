#include "pluginManager.h"

using namespace std;
using namespace micros_label_gen;

int main(int argc, char * argv[])
{
    boost::shared_ptr<PluginManager> pmPtr;
    pmPtr.reset(new PluginManager());
    int ret;
    ret = pmPtr->loadLabelPlugins("../resources/twoInOne.xml");
    cout<<"twoInOne.xml load successfully! "<<endl;
    ret = pmPtr->enableLabelPlugins();
    if(ret == 0)
    {
        cout<<"Plugins configed in the twoInOne.xml is availabel now! "<<endl;
    }
    ret = pmPtr->startLabelPlugins();
    if(ret == 0)
    {
        cout<<"Plugins configed in the twoInOne.xml start successfully! "<<endl;
    }
    getchar();
    sleep(50);
   // ret = pmPtr->loadLabelPlugins("../resources/plugin.xml");
//    pm.loadPlugins();
    
}