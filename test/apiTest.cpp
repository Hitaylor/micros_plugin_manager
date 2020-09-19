#include "pluginManager.h"

using namespace std;
using namespace micros_sensor_plugin;

int main(int argc, char *argv[])
{
    boost::shared_ptr<PluginManager> pmPtr;
    int ret;

    pmPtr.reset(new PluginManager());
    //解析label、sensor并建立两者之间的映射
    pmPtr->run("../resources/v4l2.xml");

    std::string sensorName = "v4l2SensorPlugin";

    while (1)
    {
        microsSensorLabelInfo data_info;
        ret = pmPtr->getLatestSensorAndLabelData(sensorName, data_info);
        if (ret == 0)
        {
            cout << "8. Get Latest Sensor And Label Data successfully! " << endl;

            cout << "info.dataLength=" << data_info.dataLength << endl;
            cout << "info.data =" << data_info.data << endl;
            cout << "info.label size =" << data_info.labelList.size() << endl;
            for (microsLabelData labelData : data_info.labelList)
            {
                cout << "labelData:labelName =" << labelData.labelName << endl;
                cout << "labelData:labelContentLength =" << labelData.labelContentLength << endl;
                cout << "labelData:labelContent =" << labelData.labelContent << endl;
            }
        }
        else
        {
            cout << "8. Get Latest Sensor And Label Data failed!" << endl;
        }
        sleep(5);
    }

    getchar();
    sleep(50);
    // ret = pmPtr->loadLabelPlugins("../resources/plugin.xml");
    //    pm.loadPlugins();
}
