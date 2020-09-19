#ifndef _H_LABEL_H
#define _H_LABEL_H
/*****************************************************************************
*  OpenST Basic tool library                                                 *
*  Copyright (C) 2014 Henry.Wen  renhuabest@163.com.                         *
*                                                                            *
*  This file is part of OST.                                                 *
*                                                                            *
*  This program is free software; you can redistribute it and/or modify      *
*  it under the terms of the GNU General Public License version 3 as         *
*  published by the Free Software Foundation.                                *
*                                                                            *
*  You should have received a copy of the GNU General Public License         *
*  along with OST. If not, see <http://www.gnu.org/licenses/>.               *
*                                                                            *
*  Unless required by applicable law or agreed to in writing, software       *
*  distributed under the License is distributed on an "AS IS" BASIS,         *
*  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  *
*  See the License for the specific language governing permissions and       *
*  limitations under the License.                                            *
*                                                                            *
*  @file     Example.h                                                       *
*  @brief    对文件的简述                                                      *
*  Details.                                                                  *
*                                                                            *
*  @author   Henry.Wen                                                       *
*  @email    renhuabest@163.com                                              *
*  @version  1.0.0.1(版本号)                                                  *
*  @date     renhuabest@163.com                                              *
*  @license  GNU General Public License (GPL)                                *
*                                                                            *
*----------------------------------------------------------------------------*
*  Remark         : Description                                              *
*----------------------------------------------------------------------------*
*  Change History :                                                          *
*  <Date>     | <Version> | <Author>       | <Description>                   *
*----------------------------------------------------------------------------*
*  2014/01/24 | 1.0.0.1   | Henry.Wen      | Create file                     *
*----------------------------------------------------------------------------*
*                                                                            *
*****************************************************************************/

#include <iostream>
#include <new>//used for the std::bad_alloc()
#include <string.h>//used for the memset()
#include "constParam.h"//use the LABEL_CONTENT_MAX_LENGTH param in the constructor

using namespace std;

/**
 * @brief Description of the label plugins. Each label plugin correspond to a Label object. A Label object describes the path to the label generator plugin, and the name, content and size of the data generated by the label generator plugin
 * 
 */
class Label
{
    ///name or the label of the data generated by the label generator plugin
    std::string labelName;
    ///path to the label generator plugin
    std::string labelPluginPath;
    ///size of the data generated by the label generator plugin
    int labelContentLength;
    ///content of the data generated by the label generator plugin
    void * labelContent;

public:
    Label();
    Label(std::string aLabelName, std::string aLabelPluginPath, int aLabelContentLength);
    ~Label();
    
    std::string getLabelName() const;
    void setLabelName(std::string aLabelName);
    
    std::string getLabelPluginPath();
    void setLabelPluginPath(std::string aLabelPluginPath);
    
    int getLabelContentLength() const;
    void setLabelContentLength(int aLabelContentLength);
    
    void * getLabelContent() const;
    void setLabelContent(void * aLabelContent);

    /// if using the default constructor (no params), the labelContent will be null, so this function should be called to alloc memory for it.
    int allocMemForLabelContent();


};
#endif