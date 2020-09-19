#include "../include/sensor.h"
#include <assert.h>

#define _DEBUG 1

int main(int argc, char *argv[])
{
     Sensor *sptr ;
    Sensor *sptr1 = NULL;
    Sensor *sptr2 = NULL;

    /*
       Test the the constructor of the Label with negative labelcontentlength
    */
    cout << "---------------------------Test Begin---------------------------------------" << endl;
    cout << "test1: Test the the default constructor of the Sensor without params" << endl;
    cout << "----------------------------------------------------------------------------" << endl;
    try{
        sptr = new Sensor();
    }
    catch(std::bad_alloc &ex)
    {
        printf("%s\n",ex.what());
    }
    cout<<"here 1"<<endl;
    assert(sptr != NULL);
    assert(sptr);
    assert(!sptr->getSensorContent());
    assert(!sptr->getSensorContentLengthPtr());
    assert(sptr->getSensorName().empty());
    assert(sptr->getMaxSensorContentLength() == 0);
    assert((sptr->getSensorParams()).empty());
    assert((sptr->getSensorPluginClassName()).empty());
    assert((sptr->getSensorPluginPath()).empty());
    cout<<"end of test1-------------------------------------------------------------------"<<endl;
/*
    cout << "Test the the constructor of the Sensor with negative sensorMaxcontentlength" << endl;
    cout << "----------------------------------------------------------------------------" << endl;
    try{
        sptr = new Label("timeLabel", "../resource/libtimelabelgenerator.so", -1);
    }
    catch(std::bad_alloc &ex)
    {
        printf("%s\n",ex.what());
    }
    
    assert(!sptr);
  */
    /*assert(sptr->getLabelContent());
    printf("address: 0x%0x\n",sptr->getLabelContent());
    */
    //cout<<"length="<<sptr->getLabelContentLength() <<endl;
    //else
    //{
    //    cout<<"sptr = NULL!"<<endl;
    //}
    
    /*
       Test the the constructor of the Label with legal labelcontentlength
    */
    cout << "-----------------------------------------------------------------------------" << endl;
    cout << "Test the the constructor of the Label with legal labelcontentlength" << endl;
    cout << "-----------------------------------------------------------------------------" << endl;
    /*
    try{
        sptr1 = new Label("timeLabel", "../resource/libtimelabelgenerator.so", 8);
    }
    catch(std::bad_alloc &ex)
    {
        printf("%s\n",ex.what());
    }
    assert(sptr1->getLabelContent() != NULL);

    cout << "the lable length of the sptr1 is " << sptr1->getLabelContentLength() << endl;
    char *lc = (char *)malloc(sptr1->getLabelContentLength());
    lc = (char *)sptr1->getLabelContent();
    for (int i = 0; i < sptr1->getLabelContentLength(); i++)
    {
        printf("labelContent[%d]=%d\n", i, lc[i]);
    }
*/
    /*
       Test the the constructor of the Label with too large labelcontentlength
    */
    cout << "-----------------------------------------------------------------------------" << endl;
    cout << "Test the the constructor of the Label with too large labelcontentlength" << endl;
    cout << "-----------------------------------------------------------------------------" << endl;
  /*
    try{
        sptr2 = new Label("timeLabel", "../resource/libtimelabelgenerator.so", 2*1024*1024);
    }
    catch(std::bad_alloc &ex)
    {
        printf("%s\n",ex.what());
    }
   // printf("sptr2 labelContent = 0x%0x\n",sptr2->getLabelContent());
   //assert(!sptr2->getLabelContent());
    assert(sptr2 == NULL);
    */
    cout << "------------------------Test finished!---------------------------------------" << endl;
}