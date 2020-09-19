#include "../include/label.h"
#include <assert.h>

#define _DEBUG 1

int main(int argc, char *argv[])
{
    Label *lp = NULL;
    Label *lp1 = NULL;
    Label *lp2 = NULL;

    /*
       Test the the constructor of the Label with negative labelcontentlength
    */
    cout << "---------------------------Test Begin---------------------------------------" << endl;
    cout << "Test the the constructor of the Label with negative labelcontentlength" << endl;
    cout << "----------------------------------------------------------------------------" << endl;
    try{
        lp = new Label("timeLabel", "../resource/libtimelabelgenerator.so", -1);
    }
    catch(std::bad_alloc &ex)
    {
        printf("%s\n",ex.what());
    }
    
    assert(!lp);
    /*assert(lp->getLabelContent());
    printf("address: 0x%0x\n",lp->getLabelContent());
    */
    //cout<<"length="<<lp->getLabelContentLength() <<endl;
    //else
    //{
    //    cout<<"lp = NULL!"<<endl;
    //}
    
    /*
       Test the the constructor of the Label with legal labelcontentlength
    */
    cout << "-----------------------------------------------------------------------------" << endl;
    cout << "Test the the constructor of the Label with legal labelcontentlength" << endl;
    cout << "-----------------------------------------------------------------------------" << endl;
    
    try{
        lp1 = new Label("timeLabel", "../resource/libtimelabelgenerator.so", 8);
    }
    catch(std::bad_alloc &ex)
    {
        printf("%s\n",ex.what());
    }
    assert(lp1->getLabelContent() != NULL);

    cout << "the lable length of the lp1 is " << lp1->getLabelContentLength() << endl;
    char *lc = (char *)malloc(lp1->getLabelContentLength());
    lc = (char *)lp1->getLabelContent();
    for (int i = 0; i < lp1->getLabelContentLength(); i++)
    {
        printf("labelContent[%d]=%d\n", i, lc[i]);
    }

    /*
       Test the the constructor of the Label with too large labelcontentlength
    */
    cout << "-----------------------------------------------------------------------------" << endl;
    cout << "Test the the constructor of the Label with too large labelcontentlength" << endl;
    cout << "-----------------------------------------------------------------------------" << endl;
    try{
        lp2 = new Label("timeLabel", "../resource/libtimelabelgenerator.so", 2*1024*1024);
    }
    catch(std::bad_alloc &ex)
    {
        printf("%s\n",ex.what());
    }
   // printf("lp2 labelContent = 0x%0x\n",lp2->getLabelContent());
   //assert(!lp2->getLabelContent());
    assert(lp2 == NULL);
    cout << "------------------------Test finished!---------------------------------------" << endl;
}