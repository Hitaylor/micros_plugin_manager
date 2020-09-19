#include "pluginManager.h"

using namespace tinyxml2;

PluginManager::PluginManager()
{

    //labelGeneratorClassLoaderPtr;
    //labelGeneratorClassLoaderPtr;
    labelNum = 0;
    labelPluginNum = 0;
    labelPluginFileNum = 0;
    sensorPluginFileNum = 0;
    sensorNum = 0;
    //labelVec
}

PluginManager::~PluginManager()
{
    //pop back all the vec and free them

    //确认是否stop掉所有的插件
    //确认是否吊用过unload，如果没有，则调用unloadLabelPlugins();
    //labelGeneratorPathSet里面对象的销毁与释放
    //遍历sensorNameToFd关闭文件
    for (auto iter = sensorNameToFd.begin(); iter != sensorNameToFd.end(); ++iter)
    {
        if (iter->second > 0) //这个是废话，后面可能会改成file*替换fd，因为根据fd实际拿不到更多有用的东西，根据file*可以判断是否已经关闭，这个在子线程中也需要判断。
            close(iter->second);
    }
}

/**
 * @brief  parse the @xml file to the description of the label plugins, and generate correspoding Label objects and push them into the labelObjVec.
 * 
 * @param xml the config file including the description of the label generator plugins.
 * @return int the result of the parsing proccess.
 */
int PluginManager::parseLabelXML(const std::string &xml)
{
    XMLDocument doc;
    XMLElement *rootElem; //根标签对，这里是espide_fs

    XMLElement *labelPluginsElem;
    XMLElement *labelPluginElem;
    XMLElement *pluginClassNameElem;
    XMLElement *pluginNameElem;
    XMLElement *pluginPathElem;
    XMLElement *labelLengthElem;
    const char *className;
    const char *name;
    const char *path;
    int length;

    boost::shared_ptr<Label> labelPtr;
    std::pair<string, boost::shared_ptr<Label>> nameAndLabelPair;
    bool exists;
    int errID = -1;
    int ret = -1;
    //std::string xml= "../resources/plugin.xml";
    // cout<<"I am here 1!"<<endl;
    errID = doc.LoadFile(xml.c_str());
    //cout<<"I am here 2!"<<endl;
    if (errID != 0)
    {
#if (_DEBUG)
        {
            cerr << "[PluginManager::parseLabelPlugin()] Load the xml file (" << xml << ") failed! err(" << errID << "): ";
            switch (errID)
            {
            case 1:
                cerr << "XML_NO_ATTRIBUTE" << endl;
                break;
            case 2:
                cerr << "XML_WRONG_ATTRIBUTE_TYPE" << endl;
                break;
            case 3:
                cerr << "XML_ERROR_FILE_NOT_FOUND" << endl;
                break;
            case 4:
                cerr << "XML_ERROR_FILE_COULD_NOT_BE_OPENED" << endl;
                break;
            case 5:
                cerr << "XML_ERROR_FILE_READ_ERROR" << endl;
                break;
            case 6:
                cerr << "UNUSED_XML_ERROR_ELEMENT_MISMATCH" << endl;
                break;
            case 7:
                cerr << "XML_ERROR_PARSING_ELEMENT" << endl;
                break; // remove at next major version
            case 8:
                cerr << "XML_ERROR_PARSING_ATTRIBUTE" << endl;
                break;
            case 9:
                cerr << "UNUSED_XML_ERROR_IDENTIFYING_TAG" << endl;
                break; // remove at next major version
            case 10:
                cerr << "XML_ERROR_PARSING_TEXT" << endl;
                break;
            case 11:
                cerr << "XML_ERROR_PARSING_CDATA" << endl;
                break;
            case 12:
                cerr << "XML_ERROR_PARSING_COMMENT" << endl;
                break;
            case 13:
                cerr << "XML_ERROR_PARSING_DECLARATION" << endl;
                break;
            case 14:
                cerr << "XML_ERROR_PARSING_UNKNOWN" << endl;
                break;
            case 15:
                cerr << "XML_ERROR_EMPTY_DOCUMENT" << endl;
                break;
            case 16:
                cerr << "XML_ERROR_MISMATCHED_ELEMENT" << endl;
                break;
            case 17:
                cerr << "XML_ERROR_PARSING" << endl;
                break;
            case 18:
                cerr << "XML_CAN_NOT_CONVERT_TEXT" << endl;
                break;
            case 19:
                cerr << "XML_NO_TEXT_NODE" << endl;
                break;
            case 20:
                cerr << "XML_ELEMENT_DEPTH_EXCEEDED" << endl;
                break;
            default:
                break;
            }
        }
#endif
        doc.PrintError();
        return -1;
    }

    rootElem = doc.RootElement(); //根标签对，这里是espide_fs
    if (!rootElem)
    {
#if (_DEBUG)
        {
            cerr << "[PluginManager::parseLabelPlugin()] Can not get a root element during parsing the xml file:" << xml << ", please check it!" << endl;
        }
#endif
        return -1;
    }
    //cout<<"I am here 3!"<<endl;
    labelPluginsElem = rootElem->FirstChildElement("labelPlugins");
    if (!labelPluginsElem)
    {
#if (_DEBUG)
        {
            cerr << "[PluginManager::parseLabelPlugin()] Can not get the child element \"labelPlugins\" under the root domain during parsing the xml file:" << xml << endl;
        }
#endif
        return -1;
    }

    labelPluginElem = labelPluginsElem->FirstChildElement("plugin");
    if (!labelPluginElem)
    {
#if (_DEBUG)
        {
            cerr << "[PluginManager::parseLabelPlugin()] Can not get the first child element \"plugin\" under the domain \"labelPlugins\" during parsing the xml file:" << xml << ", please check it!" << endl;
        }
#endif
        return -1;
    }

    while (labelPluginElem)
    {
        // cout<<"I am here 4!"<<endl;
        /* if (!labelPluginElem)
        {
#if (_DEBUG)
            {
                cerr << "Can not get the first child element \"plugin\" under the domain \"labelPlugins\" during parsing the xml file:" << xml << ", please check it!" << endl;
            }
#endif
            return -1;
        }
        */
        pluginClassNameElem = labelPluginElem->FirstChildElement("className");
        if (!pluginClassNameElem)
        {
#if (_DEBUG)
            {
                cerr << "[PluginManager::parseLabelPlugin()] Can not get the first child element \"className\" under the domain \"plugin\" during parsing the xml file:" << xml << ", please check it!" << endl;
            }
#endif
            return -1;
        }
        className = pluginClassNameElem->GetText();

        pluginNameElem = labelPluginElem->FirstChildElement("name");
        if (!pluginNameElem)
        {
#if (_DEBUG)
            {
                cerr << "[PluginManager::parseLabelPlugin()] Can not get the first child element \"name\" under the domain \"plugin\" during parsing the xml file:" << xml << ", please check it!" << endl;
            }
#endif
            return -1;
        }
        name = pluginNameElem->GetText();

        //cout<<"I am here 4.1!"<<endl;
        pluginPathElem = labelPluginElem->FirstChildElement("path");
        if (!pluginPathElem)
        {
#if (_DEBUG)
            {
                cerr << "[PluginManager::parseLabelPlugin()] Can not get the first child element \"path\" under the domain \"plugin\" during parsing the xml file:" << xml << ", please check it!" << endl;
            }
#endif
            return -1;
        }
        path = pluginPathElem->GetText();
        boost::filesystem::path tmpPath(path);
        //if the tmpPath is a absolute path, use it, else translate it to absolute path
        if (tmpPath.is_relative())
        {
#if (_DEBUG)
            {
                cout << "the path " << path << " is a relative path, its abolute path is ";
            }
#endif
            tmpPath = boost::filesystem::detail::system_complete(tmpPath);
            path = tmpPath.c_str();
#if (_DEBUG)
            {
                cout << path << endl;
            }
#endif
        }

        //cout<<"I am here 4.2!"<<endl;
        labelLengthElem = labelPluginElem->FirstChildElement("length");
        if (!labelLengthElem)
        {
#if (_DEBUG)
            {
                cerr << "[PluginManager::parseLabelPlugin()] Can not get the first child element \"length\" under the domain \"plugin\" during parsing the xml file:" << xml << ", please check it!" << endl;
            }
#endif
            return -1;
        }
        length = labelLengthElem->IntText();
        //cout<<"I am here 4.3!"<<endl;
        //validity checking
        if (!((className && *className != '\0') && (name && *name != '\0') && (path && *path != '\0') && length != 0))
        {
#if (_DEBUG)
            {
                cerr << "[PluginManager::parseLabelPlugin()] Plugin parse failed! className\name\path\length domain may be null or empty! please check it!" << endl;
                cout << "className:" << className << ", name:" << name << ", path:" << path << ", length:" << length << endl;
            }
#endif
            //continue;
            return -1;
        }

        //label uniqueness check based on the Label.name
        exists = nameToLabelPtrMap.count(name);
        if (!exists)
        {
            //put the absolute path into set for the subsequent plugin loading process
            labelGeneratorPathSet.insert(path); //去重

            //construct a @Label object and push it into the @labelVec.
            //labelPtr.reset(new Label(name,path,length));//use this reset to replace the following partial code
            //下面这种赋值出来的结果，打印出来的内容，只有labelName有数值，其他都没有值或者随机的。
            // labelPtr.reset(new Label()); //补充
            // labelPtr->setLabelName(name);
            // labelPtr->setLabelPluginPath(path); //用绝对路径
            // labelPtr->setLabelContentLength(length);
            cout << "--------------Label element parsed from the xml are :-------------" << endl;
            cout << "name = " << name << ", path = " << path << ", length = " << length << endl;

            labelPtr.reset(new Label(name, path, length));
            cout << "-------------[PluginManager::parseLabelXML()] After setting the labelPtr, members of the corresponding label object are:---------------" << endl;
            cout << "labelName = " << labelPtr->getLabelName() << ", label plugin path =" << labelPtr->getLabelPluginPath() << ", length =" << labelPtr->getLabelContentLength() << ", content =" << labelPtr->getLabelContent() << "." << endl;

            //             ret = labelPtr->allocMemForLabelContent();
            //             if (ret < 0)
            //             {
            // #if (_DEBUG)
            //                 {
            //                     cerr << "[PluginManager::parseLabelXML()] Memory alloc failed for the labelContent member of the labelPtr object!" << endl;
            //                 }
            // #endif
            //                 return -1;
            //             }

            //put the label object ptr into label object ptr vector
            labelObjVec.push_back(labelPtr);

            //put the label object ptr into name to label object ptr map
            nameAndLabelPair = std::make_pair(name, labelPtr);
            nameToLabelPtrMap.insert(nameAndLabelPair);
            //initialize the status of the label generator plugin corresponding to the label
            nameToLabelGeneratorStatusMap.insert(std::make_pair(name, -1)); //plugin has not binded
            //map the className to the label name
            classNameToLabelNameMap.insert(std::make_pair(className, name));
        }
        else
        {
            cout << "the label " << name << " has binded to another generator plugin, skip!" << endl;
            //return -1;//we also can take this case seriously by return -1 to declear the load plugin process failed
        }

        // switch to the next plugin domain
        labelPluginElem = labelPluginElem->NextSiblingElement();
    }

    labelPluginFileNum = labelPluginFileNum + labelGeneratorPathSet.size();
    labelNum = labelNum + labelObjVec.size();

#if (_DEBUG)
    {
        cout << "There are " << labelPluginFileNum << " plugin file, and " << labelNum << " labels now!" << endl;
        //        cout<<"I am here 5!"<<endl;
    }
#endif

    labelPtr.reset();

    return 0;
    //return doc.ErrorID();
}

/**
 * @brief  parse the @xml file to the description of the sensor plugins, and generate correspoding @Sensor objects and push them into the @nameToSensorPtrMap.
 * 
 * @param xml the config file including the description of the label generator plugins.
 * @return int the result of the parsing proccess.
 */
int PluginManager::parseSensorXML(const std::string &xml)
{
    XMLDocument doc;
    XMLElement *rootElem; //根标签对，这里是espide_fs

    XMLElement *sensorPluginsElem;
    XMLElement *sensorPluginElem;
    XMLElement *pluginClassNameElem;
    XMLElement *pluginNameElem;
    XMLElement *pluginPathElem;
    XMLElement *sensorMaxLengthElem;
    XMLElement *sensorParamsElem;
    XMLElement *sensorParamElem;
    const char *className;
    const char *name;
    const char *path;
    int32_t length;
    std::map<std::string, std::string> paramsMap;
    const char *paramName;
    const char *paramValue;

    boost::shared_ptr<Sensor> sensorPtr;
    std::pair<string, boost::shared_ptr<Sensor>> nameAndSensorPair;
    bool exists;
    int errID = -1;
    int ret = -1;
    //std::string xml= "../resources/plugin.xml";
    // cout<<"I am here 1!"<<endl;
    errID = doc.LoadFile(xml.c_str());
    //cout<<"I am here 2!"<<endl;
    if (errID != 0)
    {
#if (_DEBUG)
        {
            cerr << "[PluginManager::parseSensorPlugin()] Load the xml file (" << xml << ") failed! err(" << errID << "): ";
            switch (errID)
            {
            case 1:
                cerr << "XML_NO_ATTRIBUTE" << endl;
                break;
            case 2:
                cerr << "XML_WRONG_ATTRIBUTE_TYPE" << endl;
                break;
            case 3:
                cerr << "XML_ERROR_FILE_NOT_FOUND" << endl;
                break;
            case 4:
                cerr << "XML_ERROR_FILE_COULD_NOT_BE_OPENED" << endl;
                break;
            case 5:
                cerr << "XML_ERROR_FILE_READ_ERROR" << endl;
                break;
            case 6:
                cerr << "UNUSED_XML_ERROR_ELEMENT_MISMATCH" << endl;
                break;
            case 7:
                cerr << "XML_ERROR_PARSING_ELEMENT" << endl;
                break; // remove at next major version
            case 8:
                cerr << "XML_ERROR_PARSING_ATTRIBUTE" << endl;
                break;
            case 9:
                cerr << "UNUSED_XML_ERROR_IDENTIFYING_TAG" << endl;
                break; // remove at next major version
            case 10:
                cerr << "XML_ERROR_PARSING_TEXT" << endl;
                break;
            case 11:
                cerr << "XML_ERROR_PARSING_CDATA" << endl;
                break;
            case 12:
                cerr << "XML_ERROR_PARSING_COMMENT" << endl;
                break;
            case 13:
                cerr << "XML_ERROR_PARSING_DECLARATION" << endl;
                break;
            case 14:
                cerr << "XML_ERROR_PARSING_UNKNOWN" << endl;
                break;
            case 15:
                cerr << "XML_ERROR_EMPTY_DOCUMENT" << endl;
                break;
            case 16:
                cerr << "XML_ERROR_MISMATCHED_ELEMENT" << endl;
                break;
            case 17:
                cerr << "XML_ERROR_PARSING" << endl;
                break;
            case 18:
                cerr << "XML_CAN_NOT_CONVERT_TEXT" << endl;
                break;
            case 19:
                cerr << "XML_NO_TEXT_NODE" << endl;
                break;
            case 20:
                cerr << "XML_ELEMENT_DEPTH_EXCEEDED" << endl;
                break;
            default:
                break;
            }
        }
#endif
        doc.PrintError();
        return -1;
    }

    rootElem = doc.RootElement(); //根标签对，这里是espide_fs
    if (!rootElem)
    {
#if (_DEBUG)
        {
            cerr << "[PluginManager::parseSensorPlugin()] Can not get a root element during parsing the xml file:" << xml << ", please check it!" << endl;
        }
#endif
        return -1;
    }
    //cout<<"I am here 3!"<<endl;
    sensorPluginsElem = rootElem->FirstChildElement("sensorPlugins");
    if (!sensorPluginsElem)
    {
#if (_DEBUG)
        {
            cerr << "[PluginManager::parseSensorPlugin()] Can not get the child element \"sensorPlugins\" under the root domain during parsing the xml file:" << xml << endl;
        }
#endif
        return -1;
    }

    sensorPluginElem = sensorPluginsElem->FirstChildElement("plugin");
    if (!sensorPluginElem)
    {
#if (_DEBUG)
        {
            cerr << "[PluginManager::parseSensorPlugin()] Can not get the first child element \"plugin\" under the domain \"sensorPlugins\" during parsing the xml file:" << xml << ", please check it!" << endl;
        }
#endif
        return -1;
    }

    while (sensorPluginElem)
    {
        // cout<<"I am here 4!"<<endl;
        pluginClassNameElem = sensorPluginElem->FirstChildElement("className");
        if (!pluginClassNameElem)
        {
#if (_DEBUG)
            {
                cerr << "[PluginManager::parseSensorPlugin()] Can not get the first child element \"className\" under the domain \"plugin\" during parsing the xml file:" << xml << ", please check it!" << endl;
            }
#endif
            return -1;
        }
        className = pluginClassNameElem->GetText();

        pluginNameElem = sensorPluginElem->FirstChildElement("name");
        if (!pluginNameElem)
        {
#if (_DEBUG)
            {
                cerr << "[PluginManager::parseSensorPlugin()] Can not get the first child element \"name\" under the domain \"plugin\" during parsing the xml file:" << xml << ", please check it!" << endl;
            }
#endif
            return -1;
        }
        name = pluginNameElem->GetText();

        //cout<<"I am here 4.1!"<<endl;
        pluginPathElem = sensorPluginElem->FirstChildElement("path");
        if (!pluginPathElem)
        {
#if (_DEBUG)
            {
                cerr << "[PluginManager::parseSensorPlugin()] Can not get the first child element \"path\" under the domain \"plugin\" during parsing the xml file:" << xml << ", please check it!" << endl;
            }
#endif
            return -1;
        }
        path = pluginPathElem->GetText();
        boost::filesystem::path tmpPath(path);
        //if the tmpPath is a absolute path, use it, else translate it to absolute path
        if (tmpPath.is_relative())
        {
#if (_DEBUG)
            {
                cout << "the path " << path << " is a relative path, its abolute path is ";
            }
#endif
            tmpPath = boost::filesystem::detail::system_complete(tmpPath);
            path = tmpPath.c_str();
#if (_DEBUG)
            {
                cout << path << endl;
            }
#endif
        }

        //cout<<"I am here 4.2!"<<endl;
        sensorMaxLengthElem = sensorPluginElem->FirstChildElement("maxContentLength");
        if (!sensorMaxLengthElem)
        {
#if (_DEBUG)
            {
                cerr << "[PluginManager::parseSensorPlugin()] Can not get the first child element \"maxContentLength\" under the domain \"plugin\" during parsing the xml file:" << xml << ", please check it!" << endl;
            }
#endif
            return -1;
        }
        length = sensorMaxLengthElem->IntText();
        //cout<<"I am here 4.3!"<<endl;
        //validity checking
        if (!((className && *className != '\0') && (name && *name != '\0') && (path && *path != '\0') && length != 0))
        {
#if (_DEBUG)
            {
                cerr << "[PluginManager::parseSensorPlugin()] Plugin parse failed! className\name\path\maxContentLength domain may be null or empty! please check it!" << endl;
                cout << "className:" << className << ", name:" << name << ", path:" << path << ", maxContentLength:" << length << endl;
            }
#endif
            //continue;
            return -1;
        }
        //parse the params
        sensorParamsElem = sensorPluginElem->FirstChildElement("params");
        if (sensorParamsElem) //if there are some params
        {
            cout << "yyyyyyyyyyyyyyyyy" << endl;
            sensorParamElem = sensorParamsElem->FirstChildElement("param");
            cout << "paramsMap size = " << paramsMap.size() << endl;
            while (sensorParamElem)
            {
                paramName = sensorParamElem->Attribute("name");
                paramValue = sensorParamElem->Attribute("value");
                cout << "pram name =" << paramName << ", value =" << paramValue << endl;
                if (paramName == NULL || (*paramName) == '\0')
                {
                    cerr << "[PluginManager::parseSensorXML()] there is a param with empty name attribute!" << endl;

                    return -1;
                }
                else
                {
                    paramsMap.insert(std::make_pair(paramName, paramValue));
                    cout << "after insert, paramsMap size = " << paramsMap.size() << endl;
                }

                sensorParamElem = sensorParamElem->NextSiblingElement();
            }
        }
        else
        {
            paramsMap.clear();
        }

        cout << "*********************after parse the sensor xml, paramMap size= " << paramsMap.size() << endl;
        //sensor uniqueness check based on the Sensor.name
        exists = nameToSensorPtrMap.count(name);
        if (!exists)
        {
            //put the absolute path into set for the subsequent plugin loading process
            sensorPluginPathSet.insert(path); //去重

            //construct a @Label object and push it into the @sensorVec.
            //sensorPtr.reset(new Sensor(name,path,length ...));//use this reset to replace the following partial code
            sensorPtr.reset(new Sensor()); //补充
            sensorPtr->setSensorName(name);
            sensorPtr->setSensorPluginPath(path); //用绝对路径
            sensorPtr->setMaxSensorContentLength(length);
            if (!paramsMap.empty())
            {
                sensorPtr->setSensorParams(paramsMap);
            }
            cout << "max length = " << sensorPtr->getMaxSensorContentLength() << ". " << endl;
            cout << "param map size = " << sensorPtr->getSensorParams().size() << ". " << endl;

            ret = sensorPtr->allocMemForSensorContent();
            if (ret < 0)
            {
#if (_DEBUG)
                {
                    cerr << "[PluginManager::parseLabelXML()] Memory alloc failed for the sensorContent member of the sensorPtr object!" << endl;
                }
#endif
                //sensorPtr是否需要销毁？
                sensorPtr.reset();
                return -1;
            }
            ret = sensorPtr->allocMemForSensorContentLengthPtr();
            if (ret < 0)
            {
#if (_DEBUG)
                {
                    cerr << "[PluginManager::parseLabelXML()] Memory alloc failed for the sensorContentLengthPtr member of the sensorPtr object!" << endl;
                }
#endif
                sensorPtr.reset();
                return -1;
            }

            //put the sensor object ptr into sensor object ptr vector
            sensorPtrVec.push_back(sensorPtr);

            //put the sensor object ptr into name to sensor object ptr map
            nameAndSensorPair = std::make_pair(name, sensorPtr);
            nameToSensorPtrMap.insert(nameAndSensorPair);
            //initialize the status of the sensor generator plugin corresponding to the sensor
            nameToSensorPluginStatusMap.insert(std::make_pair(name, -1)); //plugin has not binded
            //map the className to the sensor name
            classNameToSensorNameMap.insert(std::make_pair(className, name));
            //在 sensorNameToLabelNamesMap中为该新的sensor创建一个空的pair，防止某个传感器不配置任何标签。
            std::vector<std::string> emptyLabelNameVec;
            bindSensorNameToLabelNames(name, emptyLabelNameVec);
        }
        else
        {
            cout << "the sensor " << name << " has binded to another sensor plugin, skip!" << endl;
            //return -1;//we also can take this case seriously by return -1 to declear the load plugin process failed
        }

        // switch to the next plugin domain
        sensorPluginElem = sensorPluginElem->NextSiblingElement();
    }

    sensorPluginFileNum = sensorPluginFileNum + sensorPluginPathSet.size();
    sensorNum = sensorNum + sensorPtrVec.size();

#if (_DEBUG)
    {
        cout << "There are " << sensorPluginFileNum << " plugin file, and " << sensorNum << " sensors now!" << endl;
        //        cout<<"I am here 5!"<<endl;
    }
#endif

    sensorPtr.reset();

    return 0;
    //return doc.ErrorID();
}
/**
 * @brief 解析sensor和label的配对关系，将解析得到的配对放到sensorNameToLabelNameMap中（该map在解析sensorPluginXml中已经添加，只不过value为空）
 * 
 * @param xml 
 * @return int 
 */
int PluginManager::parseSensorLabelMappingXML(const std::string &xml)
{
    XMLDocument doc;
    XMLElement *rootElem; //根标签对，这里是espide_fs

    XMLElement *sensorBindLabelsElem;
    XMLElement *sensorBindLabelElem;
    XMLElement *sensorNameElem;
    XMLElement *labelNamesElem;
    XMLElement *labelNameElem;

    const char *sensorName;
    const char *labelName;

    std::list<std::string> labelNameList;

    bool exists;
    int errID = -1;
    int ret = -1;
    //std::string xml= "../resources/plugin.xml";
    // cout<<"I am here 1!"<<endl;
    errID = doc.LoadFile(xml.c_str());
    //cout<<"I am here 2!"<<endl;
    if (errID != 0)
    {
        cerr << "[PluginManager::parseSensorLabelMappingXML()] xml file load or parse failed, please check it!" << endl;
        return -1;
    }

    rootElem = doc.RootElement(); //根标签对，这里是espide_fs
    if (!rootElem)
    {
#if (_DEBUG)
        {
            cerr << "[PluginManager::parseSensorLabelMappingXML()] Can not get a root element during parsing the xml file:" << xml << ", please check it!" << endl;
        }
#endif
        return -1;
    }
    //cout<<"I am here 3!"<<endl;
    sensorBindLabelsElem = rootElem->FirstChildElement("sensorBindLabels");
    if (!sensorBindLabelsElem)
    {
#if (_DEBUG)
        {
            cerr << "[PluginManager::parseSensorLabelMappingXML()] Can not get the child element \"sensorBindLabels\" under the root domain during parsing the xml file:" << xml << endl;
        }
#endif
        return -1;
    }

    sensorBindLabelElem = sensorBindLabelsElem->FirstChildElement("sensorBindLabel");
    if (!sensorBindLabelElem)
    {
#if (_DEBUG)
        {
            cerr << "[PluginManager::parseSensorLabelMappingXML()] Can not get the first child element \"sensorBindLabel\" under the domain \"sensorBindLabels\" during parsing the xml file:" << xml << ", please check it!" << endl;
        }
#endif
        return -1;
    }

    while (sensorBindLabelElem)
    {
        labelNameList.clear(); //防止前面的又在后面使用。

        sensorNameElem = sensorBindLabelElem->FirstChildElement("sensorName");
        if (!sensorNameElem)
        {
#if (_DEBUG)
            {
                cerr << "[PluginManager::parseSensorLabelMappingXML()] Can not get the first child element \"sensorName\" under the domain \"sensorBindLabel\" during parsing the xml file:" << xml << ", please check it!" << endl;
            }
#endif
            return -1;
        }
        sensorName = sensorNameElem->GetText();

        labelNamesElem = sensorBindLabelElem->FirstChildElement("labelNames");
        if (labelNamesElem)
        {
            labelNameElem = labelNamesElem->FirstChildElement("labelName");
            //从sensorNameToLabelNameMap中获取该sensor已经配对的标签，默认情况下应该是空的。
            if (sensorNameToLabelNamesMap.count(sensorName) == 0) //这个不应该成立，只要解析过sensorPluginXml后，每个sensorName都至少对应一个空的labelName列表
            {
                cerr << "[PluginManager::parseSensorLabelMappingXML()] " << sensorName << " not exist in the sensorNameToLabelNamesMap, however, there should be at least a item with empty labelNameVec in it!" << endl;
                return -1;
            }
            labelNameList = sensorNameToLabelNamesMap[sensorName];

            while (labelNameElem)
            {
                labelName = labelNameElem->GetText();
                if ((*labelName) != '\0')
                {
                    labelNameList.push_back(labelName);
                }

                labelNameElem = labelNameElem->NextSiblingElement(); //we have a mistake here, just used labelNameElems->NextSiblingElement()
            }
        }
        else //没有标签与传感器对应
        {
            labelNameList.clear();
        }
        //labelNameList可能是空的，我们是支持这种方式的，也就是只有传感器数据，没有标签。
        if (sensorNameToLabelNamesMap.count(sensorName)) //update the map if the sensorName already exist
        {
            sensorNameToLabelNamesMap[sensorName] = labelNameList;
        }
        else //insert a new item into the map if the sensorName not exist
        {
            sensorNameToLabelNamesMap.insert(std::make_pair(sensorName, labelNameList));
        }

        sensorBindLabelElem = sensorBindLabelElem->NextSiblingElement();
    }

    return 0;
}

int PluginManager::loadLabelPlugins(const std::string &xmlPath)
{
    int ret;
    std::set<string>::iterator iter;
    std::vector<std::string> classes; //className (stored in the classes[]) is used as a bridge to connect plugin object to labelName;
    std::string labelName = "";       //

    ret = parseLabelXML(xmlPath);
    if (ret != 0)
    {
#if (_DEBUG)
        {
            cerr << "[PluginManager::loadLabelPlugin()] xml " << xmlPath << " parsing failed!" << endl;
        }
#endif
        return -1;
    }

    //遍历labelGeneratorPathSet，进行加载
#if (_DEBUG)
    {
        cout << "--------------Begin to load the label generator plugins-------------- " << endl;
    }
#endif
    if (labelPluginFileNum < 1)
    {
#if (_DEBUG)
        {
            cerr << "[PluginManager::loadLabelPlugin()] There is no label generator plugin in the plugin manager framework now!" << endl;
        }
#endif
        return -1;
    }

    for (iter = labelGeneratorPathSet.begin(); iter != labelGeneratorPathSet.end(); ++iter)
    {
        labelGeneratorClassLoaderPtr = new class_loader::ClassLoader(*iter); //多次使用该加载器
        classes = labelGeneratorClassLoaderPtr->getAvailableClasses<micros_label_gen::LabelGeneratorBase>();
        printf("there are %d classes in the plugin file %s\n", classes.size(), xmlPath.c_str());
        for (unsigned int c = 0; c < classes.size(); ++c)
        {
            cout << "class[" << c << "] = " << classes.at(c) << endl;
            if (classNameToLabelNameMap.count(classes[c]) == 0) // the class Name has not appeared in the classNameToLabelNameMap.
            {
#if (_DEBUG)
                {
                    cerr << "[PluginManager::loadLabelPlugin()] The className not appeare in the classNameToLabelNameMap, however it should appear!" << endl;
                }
#endif
                return -1; // here, we takes a strict strategy
            }
            labelName = classNameToLabelNameMap[classes[c]]; //不允许一个类对 几个标签，所以可以用这个方法，不用find(key)，其返回的是迭代器，还要便利迭代器
            if (labelName.empty() || labelName == "")
            {
#if (_DEBUG)
                {
                    cerr << "[PluginManager::loadLabelPlugin()] The labelName get failed from the classNameToLabelNameMap, it must be a non-empty value!" << endl;
                }
#endif
                return -1;
            }
            boost::shared_ptr<micros_label_gen::LabelGeneratorBase> plugin = labelGeneratorClassLoaderPtr->createInstance<micros_label_gen::LabelGeneratorBase>(classes[c]);
            //labelGeneratorObjVec.push_back(plugin); //should remove
            //map the label name to the label generator plugin
            nameTolabelGeneratorPtrMap.insert(std::make_pair(labelName, plugin));
            //update the nameToPluginStatus
            if (nameToLabelGeneratorStatusMap.count(labelName) == 0)
            {
#if (_DEBUG)
                {
                    cerr << "[PluginManager::loadLabelPlugin()] The labelName should appear in the nameToLabelGeneratorStatusMap, however it not!" << endl;
                }
#endif
                return -1;
            }
            nameToLabelGeneratorStatusMap[labelName] = 0;

            plugin->print();

            //std::thread t(std::mem_fn(&Parent::createThread),std::this_thread);
            //std::thread t(&Parent::createThread, plugin);
            //获取名字，然后查找map获得Label对象，得到labelConent的值，作为第三个参数。

            /* string labelName = classes.at(c);//这个是标签名吗？是不是类名啊
            boost::shared_ptr<Label> labelPtr;
            if (nameToLabelPtrMap.count(labelName))
            {
                labelPtr = nameToLabelPtrMap[labelName];
            }
            else
            {
#if (_DEBUG)
                {
                    cerr << "can not find a Label obj with the name " << labelName << ", which should exists in the ..." << endl;
                }
#endif
                continue;
            }
            //启动插件
            std::thread t(&LabelGeneratorBase::genLabel, this, labelPtr->getLabelContent()); //验证数据是否能到这里
            threadVec.push_back(t);
            t.detach();
            sleep(10);
            //plugin->setI(1);
            //printf("i=%d\n",plugin->getI());
*/
            // plugin->createThread();
            // plugin->startThread();
        }
    }
#if (_DEBUG)
    {
        cout << "[PluginManager::loadLabelPlugin()] label generator plugins configed in the " << xmlPath << " file load successfully!" << endl;
    }
#endif

    return 0;
}
/**
 * @brief 
 * 
 * @param xmlPath 
 * @return int 
 */
int PluginManager::loadSensorPlugins(const std::string &xmlPath)
{
    int ret;
    std::set<string>::iterator iter;
    std::vector<std::string> classNameVec; //className (stored in the classes[]) is used as a bridge to connect plugin object to labelName;
    std::string sensorName = "";

    ret = parseSensorXML(xmlPath);
    if (ret != 0)
    {
#if (_DEBUG)
        {
            cerr << "[PluginManager::loadSensorPlugin()] xml " << xmlPath << " parsing failed!" << endl;
        }
#endif
        return -1;
    }

    //遍历sensorPluginPathSet，进行加载
#if (_DEBUG)
    {
        cout << "--------------Begin to load the sensor plugins-------------- " << endl;
    }
#endif
    if (sensorPluginFileNum < 1)
    {
#if (_DEBUG)
        {
            cerr << "[PluginManager::loadSensorPlugin()] There is no sensor plugin in the plugin manager framework now!" << endl;
        }
#endif
        return -1;
    }

    for (iter = sensorPluginPathSet.begin(); iter != sensorPluginPathSet.end(); ++iter)
    {
        sensorPluginClassLoaderPtr = new class_loader::ClassLoader(*iter); //多次使用该加载器
        classNameVec = sensorPluginClassLoaderPtr->getAvailableClasses<micros_sensor_plugin::SensorPluginBase>();
        cout << "[PluginManager::loadSensorPlugin()] There are " << classNameVec.size() << " classes in the plugin file " << xmlPath.c_str() << endl;
        for (unsigned int c = 0; c < classNameVec.size(); ++c)
        {
            cout << "sensorPlugin class[" << c << "] = " << classNameVec.at(c) << endl;
            if (classNameToSensorNameMap.count(classNameVec[c]) == 0) // the class Name has not appeared in the classNameToLabelNameMap.
            {
#if (_DEBUG)
                {
                    cerr << "[PluginManager::loadSensoePlugin()] The className not appear in the classNameToSensorNameMap, however it should appear!" << endl;
                }
#endif
                return -1; // here, we takes a strict strategy
            }
            sensorName = classNameToSensorNameMap[classNameVec[c]]; //不允许一个类对 几个标签，所以可以用这个方法，不用find(key)，其返回的是迭代器，还要便利迭代器
            if (sensorName.empty() || sensorName == "")
            {
#if (_DEBUG)
                {
                    cerr << "[PluginManager::loadSensoePlugin()] The sensorName get failed from the classNameToSensorNameMap, it must be a non-empty value!" << endl;
                }
#endif
                return -1;
            }

            boost::shared_ptr<micros_sensor_plugin::SensorPluginBase> plugin = sensorPluginClassLoaderPtr->createInstance<micros_sensor_plugin::SensorPluginBase>(classNameVec[c]);
            //labelGeneratorObjVec.push_back(plugin); //should remove
            //map the label name to the label generator plugin
            nameToSensorPluginPtrMap.insert(std::make_pair(sensorName, plugin));
            //update the nameToPluginStatus
            if (nameToSensorPluginStatusMap.count(sensorName) == 0)
            {
#if (_DEBUG)
                {
                    cerr << "[PluginManager::loadSensorPlugin()] The sensorName should appear in the nameToSensorPluginStatusMap, however it not!" << endl;
                }
#endif
                return -1;
            }
            nameToSensorPluginStatusMap[sensorName] = 0;

            plugin->print();
        }
    }
#if (_DEBUG)
    {
        cout << "[PluginManager::loadSensorPlugin()] Sensor plugins configured in the " << xmlPath << " file load successfully!" << endl;
    }
#endif

    return 0;
}

/**
 * @brief 
 * 
 * @param sensorName 
 * @param labelNameVec  允许为空，这样该传感器数据不需要标签
 * @return int 
 */
int PluginManager::bindSensorNameToLabelNames(const std::string &sensorName, const std::vector<std::string> &labelNameVec)
{
    //  std::vector<std::string>::iterator labelVecIter;
    std::list<std::string> labelNameList;
    unsigned long inSize = 0;
    unsigned long outSize = 0;

    if (labelNameVec.size() < 1)
    {
#if (_DEBUG)
        {
            cout << "[PluginManager::bindSensorNameToLabelNames()] the param labelNameVec is empty, and the sensor " << sensorName << " will stored into file without label information!" << endl;
        }
#endif
    }
    else
    {
        for (auto labelVecIter = labelNameVec.begin(); labelVecIter != labelNameVec.end(); ++labelVecIter)
        {
            labelNameList.push_back(*labelVecIter);
        }
    }

    sensorNameToLabelNamesMap.insert(std::make_pair(sensorName, labelNameList));

    //测试写进去的和输入的一样多
    inSize = labelNameVec.size();
    outSize = sensorNameToLabelNamesMap[sensorName].size();
    if (inSize != outSize)
    {
#if (_DEBUG)
        {
            cerr << "[PluginManager::bindSensorNameToLabelNames()] Insert new pair failed!" << endl;
        }
        return -1;
    }
#endif
    return 0;
}

int PluginManager::initSensorPlugin(const std::string &sensorName)
{

    boost::shared_ptr<Sensor> sensorPtr;
    std::map<std::string, std::string> paramsMap;
    boost::shared_ptr<SensorPluginBase> sensorPluginPtr;

    int ret;
    //find the sensor object corresponding to the sensorName
    if (nameToSensorPtrMap.count(sensorName))
    {
        sensorPtr = nameToSensorPtrMap[sensorName];
        if (sensorPtr == NULL)
        {
            cerr << "[PluginManager::initSensorPlugin()] sensorPtr get failed from nameToSensorPtrMap for the sensor: " << sensorName << endl;
            return -1;
        }
    }
    else
    {
        cerr << "[PluginManager::initSensorPlugin()] There is no sensor names: " << sensorName << " in the nameToSensorPtrMap!" << endl;
        return -1;
    }
    paramsMap = sensorPtr->getSensorParams();
    if (paramsMap.size() == 0)
    {
        cout << "[PluginManager::initSensorPlugin()] There is no param for sensor: " << sensorName << endl;
    }

    //find the sensorPlugin and call its init()
    if (nameToSensorPluginPtrMap.count(sensorName))
    {
        sensorPluginPtr = nameToSensorPluginPtrMap[sensorName];
        if (sensorPluginPtr == NULL)
        {
            cerr << "[PluginManager::initSensorPlugin()] There is no sensorPluginPtr in nameToSensorPluginPtrMap for sensor: " << sensorName << endl;
            return -1;
        }
    }
    else
    {
        cerr << "[PluginManager::initSensorPlugin()] sensor:" << sensorName << " not exist in nameToSensorPluginPtrMap!" << endl;
        return -1; //这种情况应当与前面的区分，这种可能是输入错误的参数，不应该返回-1
    }

    ret = sensorPluginPtr->init(paramsMap);
    if (ret != 0)
        return ret;

    return 0;
}
/**
 * @brief init all sensor plugins whose status is 0
 * 
 * @return int 
 */
int PluginManager::initSensorPlugins()
{
    std::map<std::string, int>::iterator iter;
    int ret;
    //iterate the nameToSensorPluginStatusMap to find whose status is 0
    for (iter = nameToSensorPluginStatusMap.begin(); iter != nameToSensorPluginStatusMap.end(); ++iter)
    {
        if (iter->second == 0)
        {
            ret = initSensorPlugin(iter->first);
            if (ret != 0)
            {
                return ret;
            }
            else
            {
                continue;
            }
        }
    }
    return 0;
}

/**
 * @brief enable the label generating function of the label generators by setting the flag member of the xxxLabelGenerator object
 * because the xxxLabelGenerator is implemented by users, we can not guarantee the initial flag member value.
 * @param labelName 
 * @return int 
 */
int PluginManager::enableLabelPlugin(const std::string &labelName)
{
    //get the status of the label generator plugin
    if (nameToLabelGeneratorStatusMap.count(labelName) == 0 || nameTolabelGeneratorPtrMap.count(labelName) == 0)
    {
#if (_DEBUG)
        {
            cerr << "[PluginManager::enableLabelPlugin()] The key " << labelName << " dos not appeared in the nameToLabelGeneratorStatusMap or nameToLabelGeneratorObjMap. Please check it!" << endl;
        }
#endif
        return -1;
    }
    //set the flag member of the corresponding label object to 1.
    if (nameTolabelGeneratorPtrMap[labelName]->getFlag() != 0)
    {
        cout << "[PluginManager::enableLabelPlugin()] Label generating function already enabled for the label generator of label: " << labelName << "." << endl;
    }
    else
    {
        nameTolabelGeneratorPtrMap[labelName]->setFlag(1);
    }

    return 0;
}

int PluginManager::enableLabelPlugins()
{
    if (nameTolabelGeneratorPtrMap.size() == 0)
    {
#if (_DEBUG)
        {

            cerr << "[PluginManager::enableLabelPlugins()] There is no LabelGenerator object now!" << endl;
        }
#endif
        return -1;
    }

    std::map<std::string, boost::shared_ptr<micros_label_gen::LabelGeneratorBase>>::iterator iter;
    std::string labelName;
    boost::shared_ptr<micros_label_gen::LabelGeneratorBase> labelGenPtr;
    for (iter = nameTolabelGeneratorPtrMap.begin(); iter != nameTolabelGeneratorPtrMap.end(); ++iter)
    {
        labelName = iter->first;
        labelGenPtr = iter->second;
        if (labelGenPtr->getFlag() == 0)
        {
            labelGenPtr->setFlag(1);
        }
        else
        {
            cout << "[PluginManager::enableLabelPlugins()] Label generating function already enabled for the label generator of label: " << labelName << "." << endl;
        }
    }
    return 0;
}
/**
 * @brief Enabel the sensorPlugin to be able to generate data if contorled by the flag by set the flag member to be 1 for the sensor plugin whose flag equals 0 corresponding to the sensorName.
 * 
 * @param sensorName 
 * @return int 
 */
int PluginManager::enableSensorPlugin(const std::string &sensorName)
{
    boost::shared_ptr<SensorPluginBase> sensorPluginPtr;

    if (nameToSensorPluginPtrMap.count(sensorName))
    {
        sensorPluginPtr = nameToSensorPluginPtrMap[sensorName];
        if (sensorPluginPtr == NULL)
        {
            cerr << "[PluginManager::enableSensorPlugin()] sensorPluginPtr get failed from nameToSensorPluginPtrMap for sensor: " << sensorName << endl;
            return -1;
        }
        sensorPluginPtr->setFlag(1);
    }
    else
    {
        cerr << "[PluginManager::enableSensorPlugin()] There is no sensor plugin object ptr in the nameToSensorPluginPtrMap for sensor: " << sensorName << endl;
        return -1;
    }

    return 0;
}

int PluginManager::enableSensorPlugins()
{
    boost::shared_ptr<SensorPluginBase> sensorPluginPtr;
    std::string sensorName;
    //iterate the
    //对于这种遍历的行为，如果中间失败了，则会返回-1，但是之前的设置已经成功了，这种怎么办？是否需要在返回之前撤销之前的操作？这个是一个共性的问题，需要好好设计一下逻辑
    for (auto iter = nameToSensorPluginStatusMap.begin(); iter != nameToSensorPluginStatusMap.end(); ++iter) //we have a mistake here, just use the nameToLabelGeneratorStatusMap
    {
        cout << "----------------" << endl;
        if (iter->second == 0)
        {
            sensorName = iter->first;
            if (nameToSensorPluginPtrMap.count(sensorName))
            {
                sensorPluginPtr = nameToSensorPluginPtrMap[sensorName];
                if (sensorPluginPtr == NULL)
                {
                    cerr << "[PluginManager::enableSensorPlugins()] sensorPluginPtr get failed from nameToSensorPluginPtrMap for sensor: " << sensorName << endl;
                    return -1;
                }
                cout << "+++++++++++++++++++" << endl;
                cout << "[aFlag] =" << sensorPluginPtr->getFlag() << endl;
                int tmp_flag = 1;
                sensorPluginPtr->setFlag(tmp_flag);
                sensorPluginPtr->print();
                cout << "[aFlag]after=" << sensorPluginPtr->getFlag() << endl;
                cout << "[PluginManager::enableSensorPlugins()] set the flag = 1 for the sensor: " << sensorName << endl;
            }
            else
            {
                cerr << "[PluginManager::enableSensorPlugin()s] There is no sensor plugin object ptr in the nameToSensorPluginPtrMap for sensor: " << sensorName << endl;
                return -1;
            }
        }
    }
    return 0;
}

/**
 * @brief load all the plugins described in the @param xmlPath and set up the mapping relationship between them, including the label generator plugins and sensor plugins.
 * @note the call sequence must b loadLabelPlugins() earlier than loadSensorPlugins , loadSensorPlugins earlier than parseSensorLabelMappingXML
 * @param xmlPath 
 * @return int 0 load successfully, -1 failed.
 */
int PluginManager::loadPlugins(const std::string &xmlPath)
{
    int ret;
    ret = loadLabelPlugins(xmlPath);
    if (ret != 0)
    {
        cerr << "[PluginManager::loadPlugins()] Load label generator plugins failed, please check it!" << endl;
        return -1;
    }
    ret = loadSensorPlugins(xmlPath);
    if (ret != 0)
    {
        cerr << "[PluginManager::loadPlugins()] Load sensor plugins failed, please check it!" << endl;
        return -1;
    }

    ret = parseSensorLabelMappingXML(xmlPath);
    if (ret != 0)
    {
        cerr << "[PluginManager::loadPlugins()] Setting up mapping relationship between sensor and labelNames failed, please check it!" << endl;
        return -1;
    }
    return 0;
}
/**
 * @brief enabel all the label plugins. Set the flag member of the corresponding Label object to 1.
 * 
 * @return int 
 */
/*
int PluginManager::enableLabelPlugins()
{
    std::map<std::string, int>::iterator iter;
    std::string labelName;
    int status = -1;
    if (nameToLabelGeneratorStatusMap.size() == 0)
    {
        cerr << "[PluginManager::enableLabelPlugins()] nameToLabelGeneratorStatusMap is empty, you can not enable any label plugin!" << endl;
        return -1;
    }
    for (iter = nameToLabelGeneratorStatusMap.begin(); iter != nameToLabelGeneratorStatusMap.end(); ++iter)
    {
        labelName = iter->first;
        status = iter->second;
        if (status == 0 || status == 2)
        {
            if (nameTolabelGeneratorPtrMap.count(labelName) == 0)
            {
                cerr << "[PluginManager::enableLabelPlugins()] Label object corresponding to " << labelName << " not exist in the nameTolabelGeneratorPtrMap!" << endl;
                return -1;
            }
            nameTolabelGeneratorPtrMap[labelName]->setFlag(1);
        }
    }
    return 0;
}
*/
/**
 * @brief 根据插件状态，启动当前状态为0和2的插件
 * @Todo 需要增肌的部分是启动失败的时候，flag、status等状态和开关变量的重置
 * @return int 
 */
int PluginManager::startLabelPlugins()
{
    // std：:map<std::string, boost::shared_ptr<micros_label_gen::LabelGeneratorBase> >::iterator iter;
    std::map<std::string, int>::iterator iter;
    std::string labelName;
    //iterate the  @nameToLabelGeneratorStatusMap, and start the plugin whose status equal to 0 or 2
    if (nameToLabelGeneratorStatusMap.empty())
    {
#if (_DEBUG)
        {
            cerr << "[PluginManager::startLabelPlugins()] There is no entity in nameToLabelGeneratorStatusMap!" << endl;
        }
#endif
        return -1;
    }
    for (iter = nameToLabelGeneratorStatusMap.begin(); iter != nameToLabelGeneratorStatusMap.end(); ++iter)
    {
        labelName = iter->first;
        //对于状态为0的，需要新启动一个线程，但是对于状态为2的，则不需要重新启动线程了。
        if (iter->second == 0)
        {
            if (nameTolabelGeneratorPtrMap.count(labelName) == 0)
            {
#if (_DEBUG)
                {
                    cerr << "[PluginManager::startLabelPlugins()] There is no plugin correspond to the label " << labelName << " in the nameTolabelGeneratorPtrMap, which should exist!" << endl;
                }
#endif
                return -1;
            }

            boost::shared_ptr<micros_label_gen::LabelGeneratorBase> plugin = nameTolabelGeneratorPtrMap[labelName];
            if (plugin == NULL)
            {
#if (_DEBUG)
                {
                    cerr << "[PluginManager::startLabelPlugins()] The plugin get from nameTolabelGeneratorPtrMap is NULL or the object it pointing to is NULL! " << labelName << " in the nameTolabelGeneratorPtrMap, which should exist!" << endl;
                }
#endif
                return -1;
            }
            if (nameToLabelPtrMap.count(labelName) == 0)
            {
#if (_DEBUG)
                {
                    cerr << "[PluginManager::startLabelPlugins()] There is no Label object correspond to the label name" << labelName << " in the nameToLabelPtrMap, which should exist!" << endl;
                }
#endif
                return -1;
            }
            boost::shared_ptr<Label> labelPtr = nameToLabelPtrMap[labelName];
            if (!(labelPtr != NULL))
            {
#if (_DEBUG)
                {
                    cerr << "[PluginManager::startLabelPlugins()] The Label object ptr get from nameToLabelPtrMap is NULL or the object it pointing to is NULL! " << labelName << " in the nameTolabelGeneratorPtrMap, which should exist!" << endl;
                }
#endif
                return -1;
            }

            // std::thread t(&micros_label_gen::LabelGeneratorBase::genLabel, this, labelPtr->getLabelContent());
            std::thread t(&micros_label_gen::LabelGeneratorBase::genLabel, plugin, labelPtr->getLabelContent());
            /* if(t.joinable())
                {
                    #if (_DEBUG)
                {
                    cerr << "[PluginManager::startLabelPlugins()] Label generator plugin thread for "<< labelName <<" create faild! "<< endl;
                }
#endif
                    return -1;
                }*/

            //update the status of the plugin, 将来抽成函数updatexxx(labelName, status)
            if (updateLabelPluginStatus(labelName, runningStatus) != 0)
            {
                cerr << "[PluginManager::startLabelPlugins()] Status update failed for the plugin " << labelName << endl;
                return -1;
            }
            // nameToLabelGeneratorThreadMap.insert(std::make_pair(labelName, t));
            if (t.joinable())
            {
                t.detach();
            }
            else
            {
                cerr << "[PluginManager::startLabelPlugins()] The new thread for " << labelName << " is not joinable, but it should be joinable! " << endl;
                return -1; //先采用最严厉的措施，后面我们优化，允许部分没有启动成功，但是要告诉用户哪些启动了，哪些没有启动。
            }

            cout << "[PluginManager::startLabelPlugins()] Create a new thread for the label generator plugin " << labelName << " successfully!" << endl;
        }
        else if (iter->second == 2) //之前已经启动的线程，只不过现在暂停了，这一部分怎么 处理呢？
        {
            //Todo 如果thread不支持pause操作，则我们可以将flag设置为0，这样不生成标签，当做暂停状态，如果要恢复，则将flag重新设置为1，即可。
            //如果这么做，就参考上面的enable函数的操作。这里要考虑已经detach()的线程，我们是否还可以控制！
        }
    }
    return 0;
}
/**
 * @brief start all the sensor plugins whose status is 0 or 2. 
 * 
 * @return int 
 */
int PluginManager::startSensorPlugins()
{
    std::string sensorName;
    boost::shared_ptr<SensorPluginBase> sensorPluginPtr;
    //Iterate the nameToSensorPluginStatusMap to get the status
    for (auto iter = nameToSensorPluginStatusMap.begin(); iter != nameToSensorPluginStatusMap.end(); ++iter)
    {
        if (iter->second == 0) //create a new thread with the SensorPluginBase::dataProcess to fetch sensor data, read label values and write them to file
        {
            std::string fileName = "../resources/";
            sensorName = iter->first;
            fileName = fileName.append(sensorName).append(".dat"); //写文件，这个要从其他地方传进来，或者作为本函数的参数

            if (nameToSensorPluginPtrMap.count(sensorName) == 0)
            {
#if (_DEBUG)
                {
                    cerr << "[PluginManager::startSensorPlugins()] There is no plugin correspond to the sensor: " << sensorName << " in the nameToSensorPluginPtrMap, which should exist!" << endl;
                }
#endif
                return -1;
            }

            sensorPluginPtr = nameToSensorPluginPtrMap[sensorName];
            if (!(sensorPluginPtr != NULL))
            {
#if (_DEBUG)
                {
                    cerr << "[PluginManager::startSensorPlugins()] The Sensor Plugin ptr get from nameToSensorPluginPtrMap is NULL or the object it pointing to is NULL! " << sensorName << " in the nameToSensorPluginPtrMap, which should exist!" << endl;
                }
#endif
                return -1;
            }

            int fd = open(fileName.c_str(), O_CREAT | O_APPEND | O_RDWR, 0644);
            cout << "[PluginManager::startSensorPlugins()] fd = " << fd << endl;
            if (fd < 0)
            {
                cerr << "[PluginManager::startSensorPlugins()] File" << fileName << " open failed!" << endl;
                return -1;
            }
            sensorNameToFd.insert(std::make_pair(sensorName, fd));

            //获取与sensor对应的标签的对象集合
            std::list<boost::shared_ptr<Label>> labelPtrList;
            if (sensorNameToLabelNamesMap.count(sensorName) == 0)
            {
                cerr << "[PluginManager::startSensorPlugins()] sensor: " << sensorName << " not exist in sensorNameToLabelNamesMap" << endl;
                return -1;
            }
            std::list<std::string> labelNameList = sensorNameToLabelNamesMap[sensorName];
            if (labelNameList.size() != 0) //需要标签
            {
                for (auto iter2 = labelNameList.begin(); iter2 != labelNameList.end(); ++iter2)
                {
                    if (nameToLabelPtrMap.count(*iter2) == 0)
                    {
                        cerr << "[PluginManager::startSensorPlugins()] label object ptr not exist in nameToLabelPtrMap for " << *iter2 << endl;
                        return -1;
                    }
                    boost::shared_ptr<Label> labelPtr = nameToLabelPtrMap[*iter2];
                    labelPtrList.push_back(labelPtr);
                }
            }
            else //不需要标签，labelNameList为空
            {
            }

            cout << "-------[PluginManager::startSensorPlugins()] Params used by sensonPluginPtr for start new thread for each sensor plugin are as following:---------------" << endl;
            cout << "(1) fd = " << fd << " for sensor " << sensorName << "." << endl;
            cout << "(2) labelPtrList.size() = " << labelPtrList.size() << " for sensor " << sensorName << "." << endl;
            for (auto listIter = labelPtrList.begin(); listIter != labelPtrList.end(); ++listIter)
            {
                cout << "(3) labelPtr for " << (*listIter)->getLabelName() << ", label generator plugin path = "
                     << (*listIter)->getLabelPluginPath() << ", label length = " << (*listIter)->getLabelContentLength()
                     << ", labelContent address = " << (*listIter)->getLabelContent() << "." << endl;
            }
            cout << "(4) sensorPluginPtr = " << sensorPluginPtr << "." << endl;

            //start thread
            std::thread t(&SensorPluginBase::dataProcess, sensorPluginPtr, fd, labelPtrList);
            cout << "[PluginManager::startSensorPlugins()] start a new thread for the sensor: " << sensorName << endl;
            if (updateSensorPluginStatus(sensorName, runningStatus) != 0)
            {
                cerr << "[PluginManager::startSensorPlugins()] Status update failed for the plugin " << sensorName << endl;
                return -1;
            }
            // nameToLabelGeneratorThreadMap.insert(std::make_pair(labelName, t));
            if (t.joinable())
            {
                t.detach();
            }
            else
            {
                cerr << "[PluginManager::startSensorPlugins()] The new thread for " << sensorName << " is not joinable, but it should be joinable! " << endl;
                return -1; //先采用最严厉的措施，后面我们优化，允许部分没有启动成功，但是要告诉用户哪些启动了，哪些没有启动。
            }

            cout << "[PluginManager::startSensorPlugins()] Create a new thread for the sensor plugin " << sensorName << " successfully!" << endl;
        }
        else if (iter->second == 2) //just set the flag member of the sensor plugin object
        {
            //Todo...
        }
    }
    cout << "[PluginManager::startSensorPlugins()] All the sensor plugins whose status are 0 or 2 have started!" << endl;

    return 0;
}

int PluginManager::updateLabelPluginStatus(std::string labelName, int status)
{
    if (nameToLabelGeneratorStatusMap.count(labelName) == 0)
    {
#if (_DEBUG)
        {
            cerr << "[PluginManager::updateLabelPluginStatus（std()] The labelName should appear in the nameToLabelGeneratorStatusMap, however it not!" << endl;
        }
#endif
        return -1;
    }
    nameToLabelGeneratorStatusMap[labelName] = status;
    return 0;
}

int PluginManager::updateSensorPluginStatus(std::string sensorName, int status)
{
    if (nameToSensorPluginStatusMap.count(sensorName) == 0)
    {
#if (_DEBUG)
        {
            cerr << "[PluginManager::updateLabelPluginStatus（std()] The labelName should appear in the nameToSensorPluginStatusMap, however it not!" << endl;
        }
#endif
        return -1;
    }
    nameToSensorPluginStatusMap[sensorName] = status;
    return 0;
}

int PluginManager::unloadLabelPlugins()
{
    return 0;
}

int PluginManager::getLatestSensorAndLabelData(std::string sensorName, microsSensorLabelInfo &tmp)
{
    // microsSensorLabelInfo tmp;
    if (nameToSensorPluginPtrMap.count(sensorName))
    {
        int ret = -1;
        ret = nameToSensorPluginPtrMap[sensorName]->getSensorLabelDataCache(tmp);
        return 0;
    }
    else
    {
        cerr << "[PluginManager::getLatestSensorAndLabelData()] Can not find item with key =" << sensorName << " in the nameToSensorPluginPtrMap. return NULL!" << endl;
        return -1;
    }
}

//////////////////////The following are stat functions///////////////////////
int PluginManager::getLabelPluginNum()
{
    return labelPluginNum;
}
int PluginManager::getLabelPluginFileNum()
{
    return labelPluginFileNum;
}
int PluginManager::getSensorPluginFileNum()
{
    return sensorPluginFileNum;
}

unsigned long PluginManager::getLabelNum()
{
    return labelNum;
}

unsigned long PluginManager::getSensorNum()
{
    return sensorNum;
}

int PluginManager::run(std::string xmlPath)
{
    int ret = -1;
    cout << "1. ------------begin to run-------------!" << endl;

    ret = loadPlugins(xmlPath);
    if (ret == 0)
    {
        cout << "2. Load Plugins successfully!" << endl;
    }
    else
    {
        return -1;
    }

    ret = initSensorPlugins();

    if (ret == 0)
    {
        cout << "3. init Sensor Plugins successfully!" << endl;
    }
    else
    {
        return -1;
    }

    ret = enableSensorPlugins();
    if (ret == 0)
    {
        cout << "4. Sensor plugins are enabled now! " << endl;
    }
    else
    {
        return -1;
    }

    ret = enableLabelPlugins();
    if (ret == 0)
    {
        cout << "5. Label plugins are enabled now! " << endl;
    }
    else
    {
        return -1;
    }

    ret = startLabelPlugins();
    //这个执行过程中会在调用label插件的时候出错，因为sensor配置了label，但是找不到label的对象（因为没有解析labelXml）
    if (ret == 0)
    {
        cout << "6. Label plugins start successfully! " << endl;
    }
    else
    {
        cout << "6. Start the label plugins failed!" << endl;
        return -1;
    }

    ret = startSensorPlugins();
    //这个执行过程中会在调用label插件的时候出错，因为sensor配置了label，但是找不到label的对象（因为没有解析labelXml）
    if (ret == 0)
    {
        cout << "7. Sensor plugins start successfully! " << endl;
    }
    else
    {
        cout << "7. Start the sensor plugins failed!" << endl;
        return -1;
    }
    return 0;
}