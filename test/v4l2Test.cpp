#include "pluginManager.h"

using namespace std;
using namespace micros_sensor_plugin;

int main(int argc, char *argv[])
{
    boost::shared_ptr<PluginManager> pmPtr;
    int ret;

    pmPtr.reset(new PluginManager());
    //解析label、sensor并建立两者之间的映射
    ret = pmPtr->loadPlugins("../resources/v4l2.xml");
    assert(!ret);

    cout << "1. Sensor/label plugin load successfully, and the mappting relationship between them have created! " << endl;

    cout << "2. There are " << pmPtr->getSensorNum() << " sensor in " << pmPtr->getSensorPluginFileNum() << " file(s)!" << endl;

    ret = pmPtr->initSensorPlugins();
    if (ret == 0)
    {
        cout << "3. Sensor plugins initialized! " << endl;
    }

    ret = pmPtr->enableSensorPlugins();
    if (ret == 0)
    {
        cout << "4. Sensor plugins are enabled now! " << endl;
    }
    ret = pmPtr->enableLabelPlugins();
    if (ret == 0)
    {
        cout << "5. Label plugins are enabled now! " << endl;
    }

    ret = pmPtr->startLabelPlugins();
    //这个执行过程中会在调用label插件的时候出错，因为sensor配置了label，但是找不到label的对象（因为没有解析labelXml）
    if (ret == 0)
    {
        cout << "6. Label plugins start successfully! " << endl;
    }
    else
    {
        cout << "6. Start the label plugins failed!" << endl;
    }

    ret = pmPtr->startSensorPlugins();
    //这个执行过程中会在调用label插件的时候出错，因为sensor配置了label，但是找不到label的对象（因为没有解析labelXml）
    if (ret == 0)
    {
        cout << "7. Sensor plugins start successfully! " << endl;
    }
    else
    {
        cout << "7. Start the sensor plugins failed!" << endl;
    }

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
