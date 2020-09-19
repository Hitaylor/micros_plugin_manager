#include <tinyxml2.h>
#include <iostream>
#include <unistd.h>
#include <string>
#define _DEBUG 1
using namespace tinyxml2;
using namespace std;
int main(int argc, char * argv[])
{
     XMLDocument doc;
    int errID = -1;
    std::string xml= "../resources/plugin.xml";
    cout<<"I am here 1!"<<endl;
    errID = doc.LoadFile(xml.c_str());
cout<<"I am here 2!"<<endl;
    if (errID != 0)
    {
#if (_DEBUG)
        {
            cerr << "[pluginManager.cpp-->parseLabelXML()] Load the xml file (" << xml << ") failed! err: ";
            switch (errID)
            {
            case 1:
                cout << "XML_NO_ATTRIBUTE" << endl;
                break;
            case 2:
                cout << "XML_WRONG_ATTRIBUTE_TYPE" << endl;
                break;
            case 3:
                cout << "XML_ERROR_FILE_NOT_FOUND" << endl;
                break;
            case 4:
                cout << "XML_ERROR_FILE_COULD_NOT_BE_OPENED" << endl;
                break;
            case 5:
                cout << "XML_ERROR_FILE_READ_ERROR" << endl;
                break;
            case 6:
                cout << "UNUSED_XML_ERROR_ELEMENT_MISMATCH" << endl;
                break;
            case 7:
                cout << "XML_ERROR_PARSING_ELEMENT" << endl;
                break; // remove at next major version
            case 8:
                cout << "XML_ERROR_PARSING_ATTRIBUTE" << endl;
                break;
            case 9:
                cout << "UNUSED_XML_ERROR_IDENTIFYING_TAG" << endl;
                break; // remove at next major version
            case 10:
                cout << "XML_ERROR_PARSING_TEXT" << endl;
                break;
            case 11:
                cout << "XML_ERROR_PARSING_CDATA" << endl;
                break;
            case 12:
                cout << "XML_ERROR_PARSING_COMMENT" << endl;
                break;
            case 13:

                cout << "XML_ERROR_PARSING_DECLARATION" << endl;
                break;
            case 14:
                cout << "XML_ERROR_PARSING_UNKNOWN" << endl;
                break;
            case 15:
                cout << "XML_ERROR_EMPTY_DOCUMENT" << endl;
                break;
            case 16:
                cout << "XML_ERROR_MISMATCHED_ELEMENT" << endl;
                break;
            case 17:
                cout << "XML_ERROR_PARSING" << endl;
                break;
            case 18:
                cout << "XML_CAN_NOT_CONVERT_TEXT" << endl;
                break;
            case 19:
                cout << "XML_NO_TEXT_NODE" << endl;
                break;
            case 20:
                cout << "XML_ELEMENT_DEPTH_EXCEEDED"<<endl;
                    break;
            default:
                break;
            }
        }
#endif
        doc.PrintError();
        return -1;
    }

    XMLElement *episode_fs = doc.RootElement(); //根标签对，这里是espide_fs
    cout<<"I am here 3!"<<endl;
    XMLElement *labelPlugins = episode_fs->FirstChildElement("labelPlugins");
    XMLElement *labelPlugin = labelPlugins->FirstChildElement("plugin");
    XMLElement *pluginName;
    XMLElement *pluginPath;
    XMLElement *labelLength;
    const char *name;
    const char *path;
    int length;
    while (labelPlugin)
    {
        cout<<"I am here 4!"<<endl;
        pluginName = labelPlugin->FirstChildElement("name");
        name = pluginName->GetText();
        cout<<"I am here 4.1!"<<endl;
        pluginPath = labelPlugin->FirstChildElement("path");
        path = pluginPath->GetText();
        cout<<"I am here 4.2!"<<endl;
        labelLength = labelPlugin->FirstChildElement("length");
        length = labelLength->IntText();
        cout<<"I am here 4.3!"<<endl;
#if (_DEBUG)
        {
            cout << "name:" << name << ", path:" << path << ", length:" << length << endl;
        }
#endif
        labelPlugin = labelPlugin->NextSiblingElement();
        
    }
    cout<<"I am here 5!"<<endl;
    return 0;
}