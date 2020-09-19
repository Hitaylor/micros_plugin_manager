#ifndef __H_LABEL_GENERATOR_BASE_H__
#define __H_LABEL_GENERATOR_BASE_H__
#include <iostream>
#include <stdio.h>
#include <unistd.h>
//#include <thread>
namespace micros_label_gen
{
  using namespace std;

  /**
 * @brief base class for the label generator plugins written by users and developers
 * 
 */
  class LabelGeneratorBase

  {
  public:
    /// flag used to control the work state of the label generator plugin. 1: generating labels. 0: pause
    int flag;
    ///reserved for future
    void *reservedParam;
    //    std::thread t;
  public:
    /// print interface for plugins to print out some information, such as help
    virtual void print() {}
    //    virtual void startThread(){}
    ///the function to generate labels. each label generator should implement this method to generate corresponding labels
    virtual void genLabel(void *) {}
    ///setter of the member @flag
    virtual void setFlag(int j) {cout<<"LabelGeneratorBase flag = "<<flag<<endl;}
    ///getter of the member @flag
    virtual int getFlag() {}
    ///setter of the member @reservedParam
    virtual void setReservedParam(void *) {}
    ///getter of the member reservedParam
    virtual void *getReservedParam(){};
  };
} // namespace micros_label_gen
#endif
