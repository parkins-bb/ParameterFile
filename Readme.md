
# 1 需求分析
<font size=4 color=black> </font>
## 1.1 功能介绍
<font size=4 color=black> 此代码为了实现功能：从YAML文件中读取配置参数，服务于HSF高性能仿真框架的开发。
- 本容器是基于树形数据结构开发实现的一个Paramater类，由于YAML文件内部节点呈现严格的树状格式，因此树形数据结构的形状将严格遵守YAML文件本身的上下级关系。
- 该类使用的第三方库为yaml-cpp，使用的C++版本为C++17。
- 该类实现了对某个YAML文件的查找、增加、修改、删除的功能，同时也提供了搜索某个文件夹下的所有YAML文件名，以便类生成对象时使用。</font>
## 1.2 YAML文件格式介绍
- <font size=4 color=black> YAML文件基本结构：
    $\qquad$ **标量**：是YAML文件最基本的结构类型，表示单个不可分割的值：
        $\qquad\qquad$① **字符串**：YAML中最常见的标量类型，可以直接书写，对于包含特殊字符或空格的字符串，需要使用引号（单引号'或双引号"）包裹。
        $\qquad\qquad$② **布尔值**：true 或 false，用于表示逻辑真或假。
        $\qquad\qquad$③ **整数**：直接书写数字，例如 123。
        $\qquad\qquad$④ **浮点数**：包括小数点的数字，例如 12.34。
        $\qquad\qquad$⑤ **Null**：表示无值，可以使用 null、Null、NULL 或 ~ 表示。
    $\qquad$**键值对**：YAML文件基于键值对，用冒号和空格分隔键和值。
    $\qquad$**缩进**：使用空格进行缩进来表示层级关系，而且只能使用空格，不能使用制表符（Tab）。
    $\qquad$**列表**：使用短横线（-）开始新的列表项。
    $\qquad$**字典**：多个键值对组成一个字典，键值对之间同样使用缩进来表示所属关系。</font>
- <font size=4 color=black> YAML文件示例：
  
  ```YAML
    # YAML文件示例
    website: 
        title: "我的个人网站"
        owner: 
            name: "ABC"
            contact:
                email: "ABC@163.com"
                phone: "123456789"
        pages: 
            - name: "首页"
              url: "/"
            - name: "博客"
              url: "/blog"
  ```
    **注释**：以#开头的行表示注释，它们不会被处理。
    **字符串**：通常不需要引号，但如果值中包含特殊字符或关键字（如:，{，}等），则需要用单引号或双引号包裹。
    **布尔值、数字和Null**：YAML直接支持布尔值（true/false）、数字以及null，对于null可以使用null、~或者直接留空。</font>
## 1.3 参数文件类型
<font size=4 color=black> 根据调研情况，一般模拟软件的配置参数分为两类，一类为基本配置参数，一类为模拟案例输入参数。</font>
- <font size=4 color=black> **基本配置参数**：基本配置参数主要包含：运行时的系统参数（config_system.yaml）, 求解器信息，拓扑配置信息（config_topo.yaml）, 基本常数信息（constant.yaml）, 网格层拓扑信息（regionTopo.yaml）。</font>
- <font size=4 color=black> **模拟案例参数**：针对特定的模拟示例输入的模拟参数：时间步长控制参数，时间积分，网格参数，控制方程等。
# 2 设计说明
## 2.1 代码架构
![代码框架](img/code_frame_png)
<font size=4 color=black> 其中Parameter类的头文件和源文件都在`./src/`文件夹中，包含`Parameter.h`和`Parameter.cpp`两个文件，用户在使用时需要三步操作：</font>
- <font size=4 color=black>修改`Parameter.cpp`中的路径，将路径指向YAML配置文件所在的文件夹：</font>
  （在当前代码中，将YAML配置文件均放在`./data`目录下）
  ```cpp
     // 构造函数，接受一个yaml文件名作为参数 
    Parameter::Parameter(const std::string& yamlFilename){
        // 初始化currentYamlFilename成员变量
        currentYamlFilename = yamlFilename;
        // 构建完整的文件路径，这里我们设定yaml文件都安放在data文件夹中，而data文件夹的位于当前源文件目录的上一级目录中
        std::string yamlFilePath = "../data/" + yamlFilename;
    
        // 加载yaml文件
        loadYamlFile(yamlFilePath);
    }

  ```
- <font size=4 color=black> 引用`Parameter.h`头文件到当前目录中 </font>
  `#include "Parameter.h"`
- 在`CMakeLists.txt`文件中链接。包含三步：
  $\qquad$① 为`Parameter.cpp`文件链接库，需要链接`yaml-cpp`静态库和`boost`库；
  $\qquad$② 将`Parameter.cpp`链接到主文件中。
  $\qquad$③ 编写CMakeLists.txt文件，之后进行编译工作。
   ```
        mkdir build
        cd build/
        cmake ..
        make
   ```
  `CMakeLists.txt`文件案例在工程目录下`./CMakeLists.txt`，其中主函数入口在工程目录下`./example/`中，在示例文档中中, `boost`库在系统目录中，`yaml-cpp`静态库在工程目录中。
  
## 2.2 成员函数说明
<font size=4 color=black>Parameter类所涉及的公有成员函数主要如下：</font>
- ```cpp
    // 构造函数:加载一个yaml文件，接受一个yaml文件名作为参数，文件路径默认为../data
    Parameter(const std::string& yamlFilename);
```

- ```cpp
    // 成员函数1： 函数用于获取指定文件夹下指定后缀的所有文件名, 接受文件路径和扩展名作为参数
    std::vector<std::string> getFilesWithExtension(const std::string& folderPath, const std::string& extension);
```

- ```cpp
    // 成员函数2：函数用于获取yaml后缀的文件，并输出成字符串数组形式
    std::vector<std::string> searchYamlFiles(const std::string& folderPath);
```

- ```cpp
    // 成员函数3：将数组写入文件的函数
    void writeToFile(const std::vector<std::string>& files, const std::string& outputFileName);
```

- ```cpp
    // 成员函数4：用于获取并返回yaml文件所有的节点内容
    std::vector<std::string> getYamlNodes();
```

- ```cpp
    // 成员函数5: 通过路径找到节点，返回当前节点的类型，父节点、子节点
    // 通过路径搜查的好处是防止搜索到同名节点，保证搜索的准确性，本方法不使用模糊搜索
    NodeInfo GetNodeInfoByPath(const std::string& internalPath);
```

- ```cpp
    // 成员函数6：给定目标值，返回该值在YAML结构中的路径
    std::vector<std::string> GetNodePathByValue(const std::string& targetValue);
```

- ```cpp
    // 成员函数7: 此函数为了实现功能：给出一个yaml节点的键名，
    // 功能：返回节点值，节点路径，如果节点是序列，则遍历序列元素
    FindResult FindAndPrintByKey(const std::string& targetKey);
```

- ```cpp
    // 成员函数8：本函数为了实现如下功能：给出节点的具体路径，增加这个路径下的节点, 需要调用成员函数9保存至新文件
    bool AddNodeToYaml(const std::string& pathStr, const std::string& nodeName, const YAML::Node& newNode);
```

- ```cpp
    // 成员函数9:将当前YAML内容保存到新文件
    bool SaveYamlToFile(const std::string& newFilename, const std::string& savePath);
```

- ```cpp
    // 成员函数10：本函数实现了修改节点的功能，需要调用成员函数9保存至新文件
    bool ModifyNode(const std::string& pathStr, const YAML::Node& newValue, const std::string& newKey = "");
```

- ```cpp
    // 成员函数11: 删除某路径下的节点，需要调用成员函数9保存至新文件
    bool RemoveNode(const std::string& pathStr);
```
- 具体使用方法可参照`Parameter.h`头文件和`./example/example.cpp`案例。
