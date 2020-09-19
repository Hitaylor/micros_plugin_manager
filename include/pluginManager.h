#ifndef _H_PLUGIN_MANEGER_H
#define _H_PLUGIN_MANEGER_H
//#include "a.h" //a.h should be in /usr/local/include or /opt/micros/1.0.1/include
#include "label.h"
#include "sensor.h"
#include "constParam.h"
#include "labelGeneratorBase.h"
#include "sensorPluginBase.h"
#include "class_loader/class_loader.h"
#include <tinyxml2.h>
#include <vector>
#include <map>
#include <set>
#include <thread>
#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <iostream>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h> //used for linux write()
#include <sys/stat.h>  //used for linux write()
#include <fcntl.h>     //used for linux write()

#include <boost/shared_ptr.hpp>
#include <boost/filesystem.hpp> //获取xml文件中pluginGeneratorPath的绝对路径
#include <boost/filesystem/operations.hpp>

using namespace std;
using namespace micros_label_gen;
using namespace micros_sensor_plugin;
using namespace boost::filesystem;

enum pluginStatus
{
    errStatus = -2,
    notLoadedStatus,
    LoadedStatus,
    runningStatus,
    pauseStatus,
    destroyedStatus
};

class PluginManager
{
    ///number of label generator plugins, should equal to the labelNum
    int labelPluginNum;
    ///number of library file proposing the label generator plugin (number of .so files)
    int labelPluginFileNum;
    int sensorPluginFileNum;
    ///the label number
    unsigned long labelNum;
    ///the sensor number
    unsigned long sensorNum;
    ///used to map className to lable name
    std::map<std::string, std::string> classNameToLabelNameMap;
    std::map<std::string, std::string> classNameToSensorNameMap;
    std::vector<boost::shared_ptr<Label>> labelObjVec;   //should delete
    std::vector<boost::shared_ptr<Sensor>> sensorPtrVec; //should delete
    std::map<std::string, boost::shared_ptr<Label>> nameToLabelPtrMap;
    std::map<std::string, boost::shared_ptr<Sensor>> nameToSensorPtrMap;

    std::vector<boost::shared_ptr<LabelGeneratorBase>> labelGeneratorObjVec; //

    std::map<std::string, boost::shared_ptr<LabelGeneratorBase>> nameTolabelGeneratorPtrMap; //修改
    std::map<std::string, boost::shared_ptr<SensorPluginBase>> nameToSensorPluginPtrMap;
    ///the map traces the status of each plugins.
    ///status:
    ///   -2: error
    ///   -1: not loaded, initial status
    ///    0: loaded
    ///    1: running
    ///    2: pause
    ///    3: destroyed
    std::map<std::string, int> nameToLabelGeneratorStatusMap;
    std::map<std::string, int> nameToSensorPluginStatusMap;
    ///label generator plugin loader, used for loading different label generator plugins
    class_loader::ClassLoader *labelGeneratorClassLoaderPtr; //改成labelGenClassLoaderPtr
    class_loader::ClassLoader *sensorPluginClassLoaderPtr;

    ///maintains the absolute path without duplication of the plugins for the plugin loading process
    std::set<std::string> labelGeneratorPathSet;
    std::set<std::string> sensorPluginPathSet;
    ///记录标签和对应的线程对象的map，需要确认detach之后，是否还可以根据线程object操作线程
    std::map<std::string, std::thread> nameToLabelGeneratorThreadMap;
    ///记录标签和对应的线程id的map
    std::map<std::string, int> nameToLabelGeneratorThreadIDMap;
    ///记录所有线程的vec，这个可能用不到，要删掉
    std::vector<std::thread> labelGeneratorThreadVec;
    ///maintains the relationship between sensor object and the corresponding label objects
    std::map<std::string, std::list<std::string>> sensorNameToLabelNamesMap;

    std::map<std::string, int> sensorNameToFd; //used for test,将来用episode的对象代替

public:
    PluginManager();
    ~PluginManager();

    ///parse the label generator plugins partition of the @xml file
    int parseLabelXML(const std::string &xml);
    ///parse the sensor plugins partition of the @xml file
    int parseSensorXML(const std::string &xml);

    int parseSensorLabelMappingXML(const std::string &xml);

    ///load the label generator plugins and the sensor data capture plugins according to the @xml file
    int loadPlugins(const std::string &xml);
    ///load label generator plugins according to the @xml file
    int loadLabelPlugins(const std::string &xml); //support  load plugin  multiple times
    ///load sensor plugins according to the @xml file
    int loadSensorPlugins(const std::string &xml);
    ///bind the labels to specified sensor plugin. //将来可能要修改xml结构并在解析xml的时候绑定
    int bindSensorNameToLabelNames(const std::string &sensorName, const std::vector<std::string> &labelNameVec);

    int initSensorPlugin(const std::string &sensorName);
    int initSensorPlugins();

    ///after loading the plugin, the flag in the Label is still 0, so before startLabelPlugins(), we should enable it first.
    int enableLabelPlugin(const std::string &labelName);
    int enableLabelPlugins();

    int enableSensorPlugin(const std::string &sensorName);
    int enableSensorPlugins();

    ///start all the
    int startPlugins(); //not implemented
    ///start all the label generator plugins being in 0\2 status
    int startLabelPlugins();
    ///start specified label generator plugins according to the label name stored in the @labelNameVec
    int startLabelPlugins(std::vector<std::string> labelNamesVec); //not implemented

    int startSensorPlugins();

    int stopPlugins();

    int unloadPlugins();
    int unloadLabelPlugins();

    int updateLabelPluginStatus(std::string labelName, int status);
    int updateSensorPluginStatus(std::string sensorName, int status);


    //增加统计信息，当前运行的插件数量、暂停的数量，可用的数量，出错的数量等
    //stat functions
    int getLabelPluginNum();
    int getLabelPluginFileNum();
    int getSensorPluginFileNum();

    unsigned long getLabelNum();

    unsigned long getSensorNum();

    //user api
    int getLatestSensorAndLabelData(std::string sensorName, microsSensorLabelInfo &tmp);

    int run(std::string xmlPath);

};
#endif