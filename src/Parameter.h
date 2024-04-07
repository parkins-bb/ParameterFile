#ifndef PARAMETER_H
#define PARAMETER_H

#include <string>
#include <iostream>
#include <filesystem>
#include <vector>
#include <unordered_map>
#include <sstream>
#include <fstream>
#include <regex>
#include <boost/filesystem.hpp>
#include <boost/optional.hpp>
#include "yaml-cpp/yaml.h"

namespace fs = boost::filesystem;

class Parameter {
public:

	// 构造函数:加载一个yaml文件，接受一个yaml文件名作为参数，文件路径默认为../data
    Parameter(const std::string& yamlFilename);

	
	// 打印root节点
	void printRoot() const {
		std::cout << root << std::endl;
	}

	// 获取YAML文件的根节点
    const YAML::Node& getRoot() const {
        return root;
    }

	// 成员函数1
	/**
	@brief 获取指定文件夹下指定后缀的所有文件名
	@param 参数1 folderPath: 文件路径; 参数2 extension: 文件扩展名
	@return 文件名组成的数组
	*/
    std::vector<std::string> getFilesWithExtension(const std::string& folderPath, const std::string& extension);


	// 成员函数2
	/**
	@brief 获取指定文件夹下所有YAML文件的文件名
	@param 参数 folderPath: 文件路径
	@return 所有YAML文件的文件名
	*/
    std::vector<std::string> searchYamlFiles(const std::string& folderPath);


	// 成员函数3：将数组写入文件的函数
	/**
	@brief 将字符串数组写入文件中
	@param 参数1 files: 待写入的字符串数组; 参数2 outputFileName: 文件的文件名
	*/
    void writeToFile(const std::vector<std::string>& files, const std::string& outputFileName);


	// 成员函数4：用于获取并返回yaml文件所有的节点内容
    std::vector<std::string> getYamlNodes();


	/** 成员函数（新增1）： 
	 * @brief 给出路径，返回对应的节点（节点可以是标量，键值对或者序列）
	 * @param 传入节点搜索路径（用一个点分隔）；引用参数存储找到的这个值
	 * @return 如果值为标量，返回“true”来表示成功；值不是标量或没有找到值，函数返回“false”
	 */
	bool getValueByPath(const std::string& path, YAML::Node& outputNode) const;

	
	/** 模板成员函数(新增2)：
	* @brief 模板成员函数用来包装上述按地址查找成员函数，可以自动判定节点值的类型
	* @param 参数1: 路径字符串，参数2:存储值 
	* @return 
	*/
	template<typename T>
    bool fetchValueAtPath(const std::string& path, T& output) const {
        std::istringstream pathStream(path);
        std::string segment;
        YAML::Node currentNode = YAML::Clone(root);

        while (std::getline(pathStream, segment, '.')) {
            if (!segment.empty()) {
                size_t bracketPos = segment.find('[');
                if (bracketPos != std::string::npos) {
                    // Handling sequences
                    std::string key = segment.substr(0, bracketPos);
                    size_t endBracketPos = segment.find(']', bracketPos);
                    int index = std::stoi(segment.substr(bracketPos + 1, endBracketPos - bracketPos - 1));
					if (!currentNode[key] || !currentNode[key][index]) {
                    	std::cerr << "Path not found or invalid index: " << path << std::endl;
                    	return false;
                	}
                    currentNode = currentNode[key][index];
                } else {
                    // Handling maps
					if (!currentNode[segment]) {
                    	std::cerr << "Path not found: " << path << std::endl;
                    	return false;
                	}
                    currentNode = currentNode[segment];
                }
            }
        }

        // Attempt conversion to the requested type
		if (currentNode && currentNode.IsDefined()){
        	try {
            	output = currentNode.as<T>();
            	return true;
        	} catch (const YAML::TypedBadConversion<T>& e) {
            	std::cerr << "Failed to convert the value at path " << path << " to the requested type: " << e.what() << std::endl;
            	return false;
        	} catch (const YAML::BadConversion& e) {
            	std::cerr << "General conversion error for the value at path " << path << ": " << e.what() << std::endl;
            	return false;
        	}
		} else {
			std::cerr << "Node not found or undefined at path: " << path << std::endl;
        	return false;
		}
    }


	/** 成员函数（新增3）：
	@brief 传入节点的路径，查找对应节点下一级的所有子节点，子节点可以是标量，映射和序列
	@param 参数1: 节点的路径，用'.'和[]表示，参数2:引用参数，存储所有找到的子节点
	@return 如果有子节点，则返回为true; 如果无子节点或路径无效，返回为false 
	*/
	bool getChildNodes(const std::string& path, std::vector<std::string>& childNodes) const;


	/**
	@brief 输入节点路径，打印下一级所有子节点
	@param 输入节点路径
	@return 如果节点路径有效或children不为空，返回true
	*/
	bool printChildrenByPath(const std::string& path) const;


	/**
	@brief 给定节点路径，查询所有子节点的类型。
	@param 参数1: 节点路径; 参数2: 容器，存储子节点类型
	@return 如果路径存在，则返回true
	*/
	bool getNodeTypesAtPath(const std::string& path, std::vector<std::string>& nodeTypes) const;
	
	
	/**
	@brief 打印所有子节点类型
	@param 参数: 节点路径
	*/
	void printNodeTypesAtPath(const std::string& path) const;
	void printRootInfo() const;	
	void testGetNodeByPath(const std::string& path) const;
	
	/**
	@brief 给定节点内容，搜索节点的路径。特点是可以从自定义的任一节点开始查找
	@param 参数1：节点内容, 可能是标量、映射的值、序列的一个元素；参数2：引用参数，用于存储节点路径
	@return 如果节点的值存在，返回为true
	@note 可以设定从任一节点开始查找，这么做的好处是：1.可以提高查找效率；2. 可以提高查找准确性
	*/
	template<typename T>
	bool findPathByValue(const YAML::Node& node, const T& valueToFind, std::vector<std::string>& paths, const std::string& currentPath = "") const {
	bool found = false;
    if (!node.IsDefined()) {
        return found;
    }
    
    if (node.IsScalar()) {
        // 尝试将节点值转换为T类型，然后与valueToFind进行比较
        try {
            T value = node.as<T>();
            if (value == valueToFind) {
                paths.push_back(currentPath);
                found = true;
            }
        } catch (const YAML::BadConversion& e) {
            // 如果转换失败，忽略这个节点
        }
    } else if (node.IsSequence()) {
        for (size_t i = 0; i < node.size(); i++) {
            std::string newPath = currentPath.empty() ? ("[" + std::to_string(i) + "]") : (currentPath + "[" + std::to_string(i) + "]");
            if (findPathByValue(node[i], valueToFind, paths, newPath)) {
                found = true;
            }
        }
    } else if (node.IsMap()) {
        for (auto it = node.begin(); it != node.end(); it++) {
            std::string newPath = currentPath.empty() ? it->first.as<std::string>() : (currentPath + "." + it->first.as<std::string>());
            if (findPathByValue(it->second, valueToFind, paths, newPath)) {
                found = true;
            }
        }
    }
    return found;
	}
	

	/**
	@brief 从根节点开始查找所有的路径，如果存在，则存储至引用参数容器path中。
	@param 参数1：要查找的值；参数2：形式参数，容器，存储路径
	@return 如果要查找的值存在，返回为true
	@note 要注意本函数是用来查询: "标量、序列中的元素、键值对的值" 对应的路径，不是用来查询键名对应的路径!!!
	*/
	template<typename T>
	bool findPathByValueInRoot(const T& valueToFind, std::vector<std::string>& paths) const {
		return findPathByValue(root, valueToFind, paths, "");
	}


	/**
	@brief 给定节点内容，打印所有的路径。
	@param 输入要查找的节点的内容（注意是: "标量、序列中的元素、键值对的值"）
	*/
	template<typename T>
	void printAllPathsByValue(const T& valueToFind) const {
		std::vector<std::string> paths;
        if (findPathByValueInRoot(valueToFind, paths)) {
            std::cout << "Found paths for value '" << valueToFind << "':" << std::endl;
            for (const auto& path : paths) {
                std::cout << "- " << path << std::endl;
            }
        } else {
            std::cout << "Value '" << valueToFind << "' not found." << std::endl;
        }
	}
	

	/**
	@brief 根据键名查找这个键名对应的路径，并存储至引用参数中
	@param 参数1 node:当前递归遍历到的节点，参数2 keyToFind:要找到的键名,
	   	   参数3 paths:用于存储找到的路径，参数4 当前路径（用于递归过程中的累积构建）
	@return 如果找得到，返回为true
	@note 较少使用这个函数
    	  注意YAML文件可能包含多个同名的键名, 所以本成员函数可以查找同名键名可能的所有路径
	*/
	bool findAllPathsByKey(const YAML::Node& node, const std::string& keyToFind, std::vector<std::string>& paths, const std::string& currentPath = "") const;
	

	/**
	@brief 从根节点开始查找键名对应的所有的路径，存储至容器paths中
	@params 参数1 keyToFind: 要查找的键名，参数2 paths: 用于存储所有路径的容器
	@return 如果键名的确存在，返回true
	@note 一般使用这个搜索键名 
	*/
	bool findAllPathsByKeyInRoot(const std::string& keyToFind, std::vector<std::string>& paths) const;


	/**
	@brief 打印键名对应的所有路径
	@param 要查询的键名 
	*/
	void printAllPathsByKey(const std::string& keyToFind) const;


	/**
	@brief
	@param
	@return
	*/
	bool addorUpdateNode(const std::string& path, const YAML::Node& value, const std::string& key);

	
	/**
	@brief 函数重载，当接收两个参数时执行修改内容命令
	@param
	@return
	*/
	bool addorUpdateNode(const std::string& path, const YAML::Node& value);


	/**
	@brief
	@param
	*/
	void saveYamlToFile(const std::string& filePath) const;	


	/*
	// 成员函数8:
	// 本函数为了实现如下功能：给出节点的具体路径，增加这个路径下的节点, 需要调用成员函数9保存至新文件
	// 本函数要求输入yaml文件路径，指定要加入节点的路径，新增节点的名称，以及新增节点的内容
	// 其中AddNodeToYaml函数为实现增加功能的函数
	// 函数，增加节点到YAML中指定的路径
	// pathStr为一个字符串相连，表示节点的路径，每个字符串代表路径中的一个节点名称
	// nodeName是新增加节点的名称，newNode是新增加节点的内容
    bool AddNodeToYaml(const std::string& pathStr, const std::string& nodeName, const YAML::Node& newNode);

	// 成员函数9:将当前YAML内容保存到新文件
	// 此函数为了实现多个进程同时修改源文件内容造成写冲突的问题，同时也避免了错误修改造成无法回复源文件的问题
    bool SaveYamlToFile(const std::string& newFilename, const std::string& savePath);

	// 成员函数10: 
	// 本函数实现了修改节点的功能，需要调用成员函数9保存至新文件
    // 输入文件root、节点路径path、新值节点newValue、新键名newKey（可选）如果需要改映射的键名，请提供新键名
	// 如果节点是标量的话，修改标量的内容
	// 如果节点是序列的话，修改序列的内容
	// 如果节点是映射的话，修改值的内容或修改
    bool ModifyNode(const std::string& pathStr, const YAML::Node& newValue, const std::string& newKey = "");

	// 成员函数11: 删除某路径下的节点，需要调用成员函数9保存至新文件
    bool RemoveNode(const std::string& pathStr);
	*/
	// 获取当前操作的YAML文件内容
    YAML::Node GetYamlContent() const;
	
	/**
	@brief 私有成员函数，用于查找子节点
	*/
    YAML::Node getNodeByPath(const YAML::Node& startNode, const std::string& path) const;

	// 析构函数，输出对象删除信息
    ~Parameter();

private:
	// loadYamlFile函数, 加载特定路径的yaml文件
    void loadYamlFile(const std::string& yamlFilePath);

	/**
	@brief 私有成员函数，根据路径查找子节点
	*/
	bool getNodeByPathSegments(const YAML::Node& node, const std::vector<std::string>& segments, size_t index, YAML::Node& outputNode) const;
	
	/**
	@brief 私有成员函数，用于查找子节点
	*/
	/*
    YAML::Node getNodeByPath(const YAML::Node& startNode, const std::string& path) const;
	*/
	/*
	// 实现增加功能
    bool AddNode(YAML::Node& root, const std::vector<std::string>& path, const std::string& nodeName, const YAML::Node& newNode);

	// 此函数是用于分离路径字符串，比如"Person.age"分离为["Person", "age"]
    std::vector<std::string> SplitPath(const std::string& pathStr);
	*/
	// 实际执行修改操作的内部函数
    bool ModifyNodeInternal(YAML::Node& current, const std::vector<std::string>& path, const YAML::Node& newValue, const std::string& newKey);
	
	// 成员变量1
    std::vector<std::string> files;
	// 成员变量2 文件名和YAML内容的映射
    std::unordered_map<std::string, YAML::Node> configs;
	// 成员变量3 保存当前操作的文件名
    std::string currentYamlFilename;
	// 成员变量4 文件的根节点
	YAML::Node root;
};

#endif // PARAMETER_H

