#include "sensor.h"

Sensor::Sensor()
{
    sensorName = "";
    sensorPluginPath = "";
    sensorPluginClassName = "";
    sensorParams.clear();
    maxSensorContentLength = 0;
    sensorContentLengthPtr = NULL;
    sensorContent = NULL;
}

Sensor::Sensor(const std::string aSensorName, const std::string aSensorPluginPath, const std::string aSensorPluginClassName, const std::map<std::string, std::string> aSensorParams, const int aMaxSensorContentLengthPtr)
{
 //   std::map<std::string, std::string>::iterator iter;
    unsigned long size = 0;
    std::string paramName;
    std::string paramValue;

    sensorName = aSensorName;
    sensorPluginPath = aSensorPluginPath;
    sensorPluginClassName = aSensorPluginClassName;
    maxSensorContentLength = aMaxSensorContentLengthPtr;

    size = aSensorParams.size();

    if (size > 0)
    {
        for (auto iter = aSensorParams.begin(); iter != aSensorParams.end(); ++iter)
        {
            paramName = iter->first;
            paramValue = iter->second;
            sensorParams.insert(std::make_pair(paramName, paramValue));
        }
        //  iter = NULL;
    }
    else
    {
#if (_DEBUG)
        {
            cout << "[Sensor::Sensor(const std::string, const std::string , const std::string, const std::map<std::string, std::string>, const int)] There is no param for sensor " << sensorName << endl;
        }
#endif
    }

    if (maxSensorContentLength < 1)
    {
#if (_DEBUG)
        {
            cerr << "maxSensorContentLength should be positive number!\n";
        }
#endif
        sensorName = "";
        sensorPluginPath = "";
        sensorPluginClassName = "";
        sensorParams.clear();
        maxSensorContentLength = 0;
        sensorContentLengthPtr = NULL;
        sensorContent = NULL;
#if (_DEBUG)
        {
            cout << "negative maxSensorContentLength and the sensorContent ptr = 0x" << sensorContent << endl;
        }
#endif
        throw std::bad_alloc();
    }
    else if (maxSensorContentLength > SENSOR_CONTENT_MAX_LENGTH)
    {
        //将来的处理方式可能是虽然申请过长，但我们可以按照最大支持的长度给它分配。目前采用严格措施，直接不分配，抛出异常
#if (_DEBUG)
        {
            cerr << "maxSensorContentLength too large, its upper limits is " << SENSOR_CONTENT_MAX_LENGTH << "!\n";
        }
#endif
        sensorName = "";
        sensorPluginPath = "";
        sensorPluginClassName = "";
        sensorParams.clear();
        maxSensorContentLength = 0;
        sensorContentLengthPtr = NULL;
        sensorContent = NULL;
#if (_DEBUG)
        {
            cout << "too large maxSensorContentLength and the sensorContent ptr = 0x%" << sensorContent << endl;
        }
#endif
        throw std::bad_alloc();
    }
    else
    {
        sensorContentLengthPtr = (int32_t *)malloc(sizeof(int32_t));
        sensorContent = malloc(maxSensorContentLength);

        if (sensorContentLengthPtr == NULL || sensorContent == NULL)
        {
            throw std::bad_alloc(); //一旦抛出异常，后面的都不会执行
        }
        *sensorContentLengthPtr = 0; //use the memset()?
        memset(sensorContent, 0, maxSensorContentLength);
#if (_DEBUG)
        {
            cout << "Reasonable maxSensorContentLength and the sensorContent ptr = 0x%" << sensorContent << " filled with 0x00, and initial sensorContentLengthPtr is set to 0!" << endl;
        }
#endif
    }
}

Sensor::~Sensor()
{
    if (sensorContentLengthPtr)
    {
        free(sensorContentLengthPtr);
        sensorContentLengthPtr = NULL;
    }

    if (sensorContent)
    {
        free(sensorContent);
        sensorContent = NULL;
    }
}

std::string Sensor::getSensorName() const
{
    return sensorName;
}

void Sensor::setSensorName(const std::string aSensorName)
{
    sensorName = aSensorName;
}

std::string Sensor::getSensorPluginPath()
{
    return sensorPluginPath;
}
void Sensor::setSensorPluginPath(const std::string aSensorPluginPath)
{
    sensorPluginPath = aSensorPluginPath;
}

std::string Sensor::getSensorPluginClassName()
{
    return sensorPluginClassName;
}
void Sensor::setSensorPluginClassName(const std::string aSensorPluginClassName)
{
    sensorPluginClassName = aSensorPluginClassName;
}

std::map<std::string, std::string> Sensor::getSensorParams()
{
    return sensorParams;
}
//因为不修改aSensorParams的内容，所以这里我们不用引用的方式传参
void Sensor::setSensorParams(const std::map<std::string, std::string> aSensorParams)
{
 //   std::map<std::string, std::string>::iterator iter;
    unsigned long size = 0;
    std::string paramName;
    std::string paramValue;

    size = aSensorParams.size();
    cout<< "size of params imported is "<<size<<endl;
    cout<<"-----------------------------------"<<endl;
    if (size > 0)
    {
        for (auto iter = aSensorParams.begin(); iter != aSensorParams.end(); ++iter)
        {
            paramName = iter->first;
            paramValue = iter->second;
            sensorParams.insert(std::make_pair(paramName, paramValue));
        }
    }
    else
    {
#if (_DEBUG)
        {
            cout << "[Sensor::setSensorParams()] There is no param for sensor: " << sensorName << endl;
        }
#endif
    }
    //iter = NULL;
}

int Sensor::getMaxSensorContentLength() const
{
    return maxSensorContentLength;
}

void Sensor::setMaxSensorContentLength(const int aMaxSensorContentLength)
{
    maxSensorContentLength = aMaxSensorContentLength;
}

int32_t *Sensor::getSensorContentLengthPtr() const
{
    return sensorContentLengthPtr;
}

void Sensor::setSensorContentLengthPtr( int32_t *aSensorContentLengthPtr)
{
    sensorContentLengthPtr = aSensorContentLengthPtr;
}

int32_t Sensor::getSensorContentLength() const
{
    return *sensorContentLengthPtr;
}

void Sensor::setSensorContentLength(const int32_t aSensorContentLength)
{
    *sensorContentLengthPtr = aSensorContentLength;
}

void *Sensor::getSensorContent() const
{
    return sensorContent;
}
void Sensor::setSensorContent(void *aSensorContent)
{
    sensorContent = aSensorContent;
}

int Sensor::allocMemForSensorContentLengthPtr()
{
    if (sensorContentLengthPtr == NULL)
    {
        sensorContentLengthPtr = (int32_t *)malloc(sizeof(int32_t));
    }
    if (sensorContentLengthPtr == NULL)
    {
#if (_DEBUG)
        {
            cout << "[Sensor::allocMemForSensorContentLengthPtr()] alloc memory for sensorContentLengthPtr failed!" << endl;
        }
#endif
        return -1;
    }
    return 0;
}
/// if using the default constructor (no params), the sensorContent will be null, so this function should be called to alloc memory for it.
int Sensor::allocMemForSensorContent()
{
    if (sensorContent == NULL)
    {
        if (maxSensorContentLength < 1)
        {
#if (_DEBUG)
            {
                cerr << "[Sensor::allocMemForSensorContent()] maxSensorContentLength < 1!" << endl;
            }
#endif
            return -1;
        }
        else
        {
            sensorContent = malloc(maxSensorContentLength);
            if (sensorContent == NULL)
            {
#if (_DEBUG)
                {
                    cout << "[Sensor::allocMemForSensorContent()] alloc memory for sensorContent failed!" << endl;
                }
#endif

                return -1;
            }
        }
        return 0;
    }
}
