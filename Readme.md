
# 1 需求分析
<font size=4 color=black> </font>
## 1.1 功能介绍
<font size=4 color=black> &emsp;&emsp; 此代码为了实现功能：从YAML文件中读取配置参数，服务于HSF高性能仿真框架的开发。
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
<font size=4 color=black> &emsp;&emsp;根据调研情况，一般模拟软件的配置参数分为两类，一类为基本配置参数，一类为模拟案例输入参数。</font>
- <font size=4 color=black> **基本配置参数**：基本配置参数主要包含：运行时的系统参数（config_system.yaml）, 求解器信息，拓扑配置信息（config_topo.yaml）, 基本常数信息（constant.yaml）, 网格层拓扑信息（regionTopo.yaml）。</font>
- <font size=4 color=black> **模拟案例参数**：针对特定的模拟示例输入的模拟参数：时间步长控制参数，时间积分，网格参数，控制方程等。
# 2 设计说明
## 2.1 代码架构
![代码框架](img/code_frame_png)
<font size=4 color=black> &emsp;&emsp;其中Parameter类的头文件和源文件都在`./src/`文件夹中，包含`Parameter.h`和`Parameter.cpp`两个文件，用户在使用时需要三步操作：</font>
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
  $\qquad$③ 指定需要编译的`main`函数，分别为`./example/`目录下的`example_1.cpp`和`example_2.cpp`
  $\qquad$③ 编写CMakeLists.txt文件，之后进行编译工作。
   ```
        mkdir build
        cd build/
        cmake ..
        make
   ```
  `CMakeLists.txt`文件案例在工程目录下`./CMakeLists.txt`，其中主函数入口在工程目录下`./example/`中，有两个`example.cpp`案例，记得在CMakeLists.txt中指定你想要编译的函数。
  在本代码中, `boost`库头文件在`include`目录下，静态库文件在`lib`目录下，`yaml-cpp`静态库在工程目录中。
  
## 2.2 成员函数说明
<font size=4 color=black>&emsp;&emsp;Parameter类所涉及的公有成员函数主要如下：</font>

- ```cpp
  /**
 	 * @brief 构造函数，加载一个yaml文件,文件路径默认为../data
 	 * @param 参数为yaml文件名
 	 */
    Parameter(const std::string& yamlFilename);

 
  /** 成员函数1:
	 * @brief 获取对应路径文件夹下所有指定后缀的文件名
	 * @param 参数1: 相对路径，参数2: 后缀名
	 * @return 文件名字符串数组
	 * @note 注意是相对路径
	 */
    std::vector<std::string> getFilesWithExtension(const std::string& folderPath, const std::string& extension);

 
  /** 成员函数2:
	 * @brief 获取指定路径下所有YAML文件
	 * @param 文件相对路径
	 * @return 文件名字符串数组
	 */
	std::vector<std::string> searchYamlFiles(const std::string& folderPath);


  /** 成员函数3:
	 * @brief 将字符串数组写入文件
	 * @param 参数1: 字符串数组，参数2: 输出文件名
	 */
    void writeToFile(const std::vector<std::string>& files, const std::string& outputFileName);


- 下面是检索功能的函数
- ```cpp
  /** 成员函数4:
	 * @brief 获取并返回yaml文件的所有节点
	 * @param 
	 * @return 所有节点的字符串数组
	 */
	 std::vector<std::string> getYamlNodes();


	/** 成员函数5： 
	 * @brief 给出路径，返回对应的节点（节点可以是标量，键值对或者序列）
	 * @param 传入节点搜索路径（用一个点分隔）；引用参数存储找到的这个值
	 * @return 如果值为标量，返回“true”来表示成功；值不是标量或没有找到值，函数返回“false”
	 */
	bool getValueByPath(const std::string& path, YAML::Node& outputNode) const;

	
	/** 成员函数6: 模板成员函数
	* @brief 模板成员函数用来包装上述按地址查找成员函数，可以自动判定节点值的类型
	* @param 参数1: 路径字符串，参数2:存储值 
	* @return 
	*/
	template<typename T>
    bool fetchValueAtPath(const std::string& path, T& output) const;

  /** 成员函数7
	 * @brief 传入节点的路径，查找对应节点下一级的所有子节点，子节点可以是标量，映射和序列
	 * @param 参数1: 节点的路径，用'.'和[]表示，参数2:引用参数，存储所有找到的子节点
	 * @return 如果有子节点，则返回为true; 如果无子节点或路径无效，返回为false 
	 */
	bool getChildNodes(const std::string& path, std::vector<std::string>& childNodes) const;


	/** 成员函数8
	 * @brief 输入节点路径，打印下一级所有子节点
	 * @param 输入节点路径
	 * @return 如果节点路径有效或children不为空，返回true
	 */
	bool printChildrenByPath(const std::string& path) const;


	/** 成员函数9
	 * @brief 给定节点路径，查询所有子节点的类型。
	 * @param 参数1: 节点路径; 参数2: 容器，存储子节点类型
	 * @return 如果路径存在，则返回true
	 */
	bool getNodeTypesAtPath(const std::string& path, std::vector<std::string>& nodeTypes) const;
	
	
	/** 成员函数10
	 * @brief 打印所有子节点类型
	 * @param 参数: 节点路径
	 */
	void printNodeTypesAtPath(const std::string& path) const;
	void printRootInfo() const;	
	void testGetNodeByPath(const std::string& path) const;

- 下面是搜索节点路径的成员函数
- ```cpp
  /** 成员函数11: 模板函数
	 * @brief 给定节点内容，搜索节点的路径。特点是可以从自定义的任一节点开始查找
	 * @param 参数1：节点内容, 可能是标量、映射的值、序列的一个元素；参数2：引用参数，用于存储节点路径
	 * @return 如果节点的值存在，返回为true
	 * @note 可以设定从任一节点开始查找，这么做的好处是：1.可以提高查找效率；2. 可以提高查找准确性
	 */
	template<typename T>
	bool findPathByValue(const YAML::Node& node, const T& valueToFind, std::vector<std::string>& paths, const std::string& currentPath = "") const;


  /** 成员函数12: 模板函数
	 * @brief 从根节点开始查找所有的路径，如果存在，则存储至引用参数容器path中。
	 * @param 参数1：要查找的值；参数2：形式参数，容器，存储路径
	 * @return 如果要查找的值存在，返回为true
	 * @note 要注意本函数是用来查询: "标量、序列中的元素、键值对的值" 对应的路径，不是用来查询键名对应的路径!!!
	 */
	template<typename T>
	bool findPathByValueInRoot(const T& valueToFind, std::vector<std::string>& paths) const;


  /** 成员函数13: 模板函数
	 * @brief 给定节点内容，打印所有的路径。
	 * @param 输入要查找的节点的内容（注意是: "标量、序列中的元素、键值对的值"）
	 */
	template<typename T>
	void printAllPathsByValue(const T& valueToFind) const;


  /** 成员函数14
	 * @brief 根据键名查找这个键名对应的路径，并存储至引用参数中
	 * @param 参数1 node:当前递归遍历到的节点，参数2 keyToFind:要找到的键名,
	   	   参数3 paths:用于存储找到的路径，参数4 当前路径（用于递归过程中的累积构建）
	 * @return 如果找得到，返回为true
	 * @note 较少使用这个函数
    	  注意YAML文件可能包含多个同名的键名, 所以本成员函数可以查找同名键名可能的所有路径
	 */
	bool findAllPathsByKey(const YAML::Node& node, const std::string& keyToFind, std::vector<std::string>& paths, const std::string& currentPath = "") const;
	

	/** 成员函数15
	 * @brief 从根节点开始查找键名对应的所有的路径，存储至容器paths中
	 * @params 参数1 keyToFind: 要查找的键名，参数2 paths: 用于存储所有路径的容器
	 * @return 如果键名的确存在，返回true
	 * @note 一般使用这个搜索键名 
	 */
	bool findAllPathsByKeyInRoot(const std::string& keyToFind, std::vector<std::string>& paths) const;


	/** 成员函数16
	 * @brief 打印键名对应的所有路径
	 * @param 要查询的键名 
	 */
	void printAllPathsByKey(const std::string& keyToFind) const;

- 下面是执行修改功能的成员函数，但是远不如直接在YAML文件中修改便捷
- ```cpp
  /** 成员函数16
	 * @brief 打印键名对应的所有路径
	 * @param 要查询的键名 
	 */
	void printAllPathsByKey(const std::string& keyToFind) const;


	/** 成员函数17
	 * @brief 增加或修改节点 
	 * @param path: 节点路径； value: 节点的新值；key: 节点的键
	 * @return 路径正确，返回true
	 */
	bool addorUpdateNode(const std::string& path, const YAML::Node& value, const std::string& key);

	
	/** 成员函数18
	@brief 函数重载，当接收两个参数时执行修改内容命令
	@param path: 节点路径；value: 节点新值
	@return
	*/
	bool addorUpdateNode(const std::string& path, const YAML::Node& value);


	/** 成员函数19
	@brief 将修改后的新YAML文件保存在“../output/”路径下
	@param 参数:文件名
	*/
	void saveYamlToFile(const std::string& filePath) const;

- 析构函数
- ```cpp
  // 析构函数，输出对象删除信息
    ~Parameter();
- 具体使用方法可参照`Parameter.h`头文件和`./example/example.cpp`案例。

# 3 YAML格式说明
## 3.1 简介
<font size=4 color=black> YAML是一种通用的数据串行化格式，其基本语法格式如下：
 - 大小写敏感
 - 使用缩进代表层级关系
 - 缩进时不允许使用Tab键，只允许使用空格
 - 缩进的空格数不重要，但是相同层级的节点左侧必须对齐</font>
## 3.2 节点类型
<font size=4 color=red>节点</font>是YAML文件数据结构的最基本单位，同时任何一个节点的“路径”均可由其父节点表示。在YAML文件中，节点可由如下几种数据类型表示:

 - **标量(Scalars)**：单个的、不可分割的值，包含如下数据类型：
   - 字符串 `std::string`:
   - 布尔值 `bool`
   - 整型 `int`
   - 浮点数 `float`
   - `Null`
   - 时间 `iso8601: 2024-3-27t16:30:30-18:00`
   - 日期 `date: 2024-3-27`

 - **映射(Mappings)**：一个键值对的集合，映射通过“**键: 值**”的形式来表示，其中键和值之间用冒号“**:**”分隔开来，对于比较严格的格式，要求在键和值之间用冒号“**:**”后加上一个空格。
    - 键和值之间通过冒号和空格组成一个映射
      ```yaml
      animal: dog
      ```
    - 一个映射的值也可以是映射
      ```yaml
      animal:
        pet: dog
        fish: goldfish
        bird: sparrow  
      ```
      yaml也允许另一种写法：
      ```yaml
      animal: {pet: dog, fish: goldfish, bird: sparrow}
      ```
 - **序列(Sequences)**: 一个<font size=4 color=red>有序</font>的集合，类似于编程语言中的数组或列表。在YAML中，序列通常通过在每个条目前加“-”字符来表示，条目之间保持缩进一致。对于比较严格的格式，要求在“-”和元素之间加上一个空格。
    - 一组连词线开头的元素组成一个数组：
      ```yaml
      - dog
      - cat
      - fish
      - bird
      ```
      表明的数组格式为`['dog', 'cat', 'fish', 'bird']`
    - 数组的子成员也可以是数组：
      ```yaml
      - 
        - dog
        - cat
        - fish
      - 
        - apple
        - banana
        - melon
      -
        - 1
        - 2
        - 3
      ```
      表明的数组格式为`[['dog', 'cat', 'fish'], ['apple', 'banana', 'melon'], [1, 2, 3]]`

 - **复合类型(Complex Types)**: 由标量、序列、映射通过嵌套组合而成的复杂数据结构。
    ```yaml
       languages:
         - Ruby
         - Perl
         - Python 
       websites:
         YAML: yaml.org 
         Ruby: ruby-lang.org 
         Python: python.org 
         Perl: use.perl.org 
    ```

以下是一个YAML格式的文件的示例，可以对比以下几种类型的节点：
```yaml
# 标量
name: John Doe

# 序列
hobbies:
  - Reading
  - Cycling
  - Painting

# 映射
address:
  street: 123 Main St
  city: Anytown
  state: CA

# 复合类型
contacts:
  - name: Jane Doe
    relation: Sister
    phone: 555-1234
  - name: Mike Smith
    relation: Friend
    phone: 555-5678
```

**注意**：当前YAML格式文件并不支持复数(complex number)这类高级的数据类型，目前仅可以通过字符串的形式来存储复数。

## 3.3 YAML标量基本数据类型
- 字符串表示方法
  - **无引号**
    直接书写字符串，不需要任何引号，适用于<font size=4 color=red>没有特殊字符或空格</font>的简单字符串。
    ```yaml
    ABCDEF
    ```
  - **使用双引号可以包含特殊字符串，包括字符串内部含空格和特殊字符（如转义字符）**
    ```yaml
    address: "123 Street, Goodtown, Goodcity.\n"
    ```
  - **单引号**
    使用单引号包含的字符只会按照字符本身字面值处理，特殊字符不会被转义。
    ```yaml
    word: 'This is a nice day.'
    ```
  - **块标记：‘>’和‘|’**
    - 折叠模式（>）：连续的非空行会被转换为一个空格分隔的单行字符串，保留单独的空行。适用于格式化文本。
      ```yaml
      folded_style: >
        This text has multiple lines,
        but this will be converted
        to a single line with spaces
        where the line breaks were.
      ```
    - 字面模式（|）：保留字符串中的换行符和空行，适合保持格式的内容，如代码或预格式化文本。
      ```yaml
      literal_style: |
        Line 1
        Line 2

        Line 4
      ```
  - **显示标记**
    - 使用!!str来明确表示字符串类型。
      ```yaml
      explicit_string: !!str 12345
      ```
- 布尔值表示方法
  真值：`true True TRUE y Y yes Yes YES on On ON`
  假值：`false False FALSE n N no No NO off Off OFF`
  为保证文件的意图更清晰，目前更推荐使用true和false, 避免使用yes/no等变体。
## 3.4 YAML节点路径表示办法
  本项目基于yaml-cpp库完成，需要依据yaml-cpp库中的内容表示节点路径
- **访问映射（Maps）**
  假设映射结构：
  ```yaml
  person:
    name: John Doe
    age: 30
  ```
  如果我要访问name的值(John Doe)，可以如下操作：
  ```cpp
  // 假设构造函数已经加载了对应的yaml文件, config为加载好对应的文件
  std::string name = config["person"]["name"].as<std::string>();
  ```
  在本代码中，将上述路径简化为：`person.name`
- **访问序列（Sequences）**
  ①假设序列结构：
  ```yaml
  colors:
     - red
     - green
     - blue
  ```
  如果我要访问color的第二个颜色（green）, 可以如下操作：
  ```cpp
  // 假设构造函数已经加载了对应的yaml文件, config为加载好对应的文件
  std::string name = config["colors"][1].as<std::string>(); // 注意序列从0编号
  ```
  在本代码中，将路径简化为：`colors[1]`
  ②假设序列结构：
  ```yaml
  employees:
    - name: John
      role: Engineer
    - name: Jane
      role: Manager
  ```
  如果我要访问John的职业(Engineer)，可以进行如下操作：
  ```cpp
  // 假设构造函数已经加载了对应的yaml文件, config为加载好对应的文件
  std::string John_name = config["employees"][0]["role"].as<std::string>(); // 注意序列从0编号
  /* 序列的第0个元素为：
  name: John
  role: Engineer
  */
  ```
  在本代码中，将路径简化为：`employees[0].role`
- **访问嵌套结构**
  假设嵌套结构：
  ```yaml
  database:
    tables:
      - name: users
        fields: ["id", "name", "email"]
  ```
  如果我要访问uers这个表的第二个字段名称（想输入name），可以如下操作：
  ```cpp
  // 假设构造函数已经加载了对应的yaml文件, config为加载好对应的文件
  std::string fieldName = config["database"]["tables"][0]["fields"][1].as<std::string>();
  ```
  实际字符串路径：database.tables[0].fileds[1]
  节点路径练习：
  ```yaml
  school:
    name: "Example High School"
    location: "Example City"
    departments:
      - name: "Mathematics"
        head: "John Doe"
        courses:
          - id: 101
            name: "Algebra"
            students:
              - name: "Jane Smith"
                id: "JS101"
              - name: "Emily Davis"
                id: "ED202"
      - name: "Science"
        head: "Jane Roe"
        courses:
          - id: 201
            name: "Biology"
            students:
              - name: "Brian Clark"   
                id: "BC303"          // 读这个
              - name: "Alex Johnson"
                id: "AJ404"
  ```
  如果我要访问‘Biology’课程下的第一个学生‘Brian Clark’的‘id’的值，可以如下操作：
  ```cpp
   std::string studentId = config["school"]["departments"][1]["courses"][0]["students"][0][id].as<std::string>();
  ```
  字符串路径：school.departments[1].courses[0].students[0].id
