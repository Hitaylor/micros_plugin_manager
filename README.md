# pluginManager
plugin Manager label

v 0.1
存在的问题：
（1）PluginManager::parseLabelXML()中将xml解析到的内容构建Label类对象，发现使用如下方式无法成功构建Label对象，只有name有值，path为空，length随机值：
    //下面这种赋值出来的结果，打印出来的内容，只有labelName有数值，其他都没有值或者随机的。
    // labelPtr.reset(new Label()); //使用无参的方式构建类的对象
    // labelPtr->setLabelName(name);
    // labelPtr->setLabelPluginPath(path); //用绝对路径
    // labelPtr->setLabelContentLength(length);
    但是使用如下有参的方式构建Label的对象是没有问题的：
    //labelPtr.reset(new Label(name,path,length));