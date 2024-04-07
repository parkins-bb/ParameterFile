#include "Parameter.h"


// 构造函数，接受一个yaml文件名作为参数
Parameter::Parameter(const std::string& yamlFilename){
	// 初始化currentYamlFilename成员变量
	currentYamlFilename = yamlFilename;
	// 构建完整的文件路径，这里我们设定yaml文件都安放在data文件夹中，而data文件夹的位于当前源文件目录的上一级目录中
	std::string yamlFilePath = "../data/" + yamlFilename;
	
	// 加载yaml文件
	loadYamlFile(yamlFilePath);
}


//成员函数1： 函数用于获取指定文件夹下指定后缀的所有文件名
std::vector<std::string> Parameter::getFilesWithExtension(const std::string& folderPath, const std::string& extension) {
	// 本地变量，用于存储找到的文件名
	std::vector<std::string> files;
	// 将相对路径转换为绝对路径
	fs::path fullPath = fs::canonical(folderPath);
	// 检查文件夹是否存在且是一个目录
	if (!fs::exists(folderPath) || !fs::is_directory(folderPath)) {
		std::cerr << "Error: Folder does not exist or is not a directory." << std::endl;
		return files;
	}

	// 在工程目录下找到data文件夹
	fs::path dataPath = fullPath / "data";
	if (!fs::exists(dataPath) || !fs::is_directory(dataPath)){
		std::cerr << "Error: Data folder does not exist or is not a directory." << std::endl;
		return files;
	}

	// 遍历文件夹内所有文件
	for (const auto& entry : fs::directory_iterator(dataPath)) {

		// 如果是普通文件且后缀匹配
		if (entry.is_regular_file() && entry.path().extension() == extension) {

			// 将文件名添加到数组中
			files.push_back(entry.path().filename().string());
		}
	}

	return files;    // 返回包含文件名的vector
}


// 成员函数2：函数用于获取yaml后缀的文件，并输出
std::vector<std::string> Parameter::searchYamlFiles(const std::string& folderPath){
	return getFilesWithExtension(folderPath, ".yaml");
}


// 成员函数3：将数组写入文件的函数
void Parameter::writeToFile(const std::vector<std::string>& files, const std::string& outputFileName){

	// 打开输出文件,以输出文件名“outputFileName”打开文件
	std::ofstream outFile(outputFileName);

	// 检查文件是否成功打开
	if (!outFile.is_open()){
		std::cerr << "Error: Unable to open output file." << std::endl;
		return;
	}	

	// 遍历文件名并向输出文件中写入内容
	for (const auto& file : files){
		outFile << file << std::endl;
	}

	std::cout << "File name written to " << outputFileName << std::endl;
}


// 下面是用于检索功能的成员函数
// 成员函数4: 用于获取并返回yaml文件所有的节点：
std::vector<std::string> Parameter::getYamlNodes() {
	std::vector<std::string> nodes;
	if (configs.find(currentYamlFilename) != configs.end()) {
		YAML::Node yamlContent = configs[currentYamlFilename];
		for (YAML::const_iterator it = yamlContent.begin(); it != yamlContent.end(); ++it) {
			std::ostringstream stream;
			stream << it->first << ": " << it->second;
			nodes.push_back(stream.str());
		}
	} else {
		std::cerr << "Error: YAML file " << currentYamlFilename << " not loaded." << std::endl;
	}
	return nodes;
}


/** 成员函数5： 
 * @brief 给出节点的路径，查找对应的节点的值。
 * @param 传入节点搜索路径（用一个点分隔）；引用参数存储找到的这个值
 * @return 如果值存在，返回“true”来表示成功；值不是标量或没有找到值，函数返回“false”
 */
bool Parameter::getValueByPath(const std::string& path, YAML::Node& outputNode) const {
    std::istringstream pathStream(path);
    std::string segment;
    std::vector<std::string> segments;

    while (std::getline(pathStream, segment, '.')) {
        if (!segment.empty()) {
            segments.push_back(segment);
        }
    }

    if (segments.empty()) {
		std::cout << "The path is empty" << std::endl;
        return false;
    }

    return getNodeByPathSegments(root, segments, 0, outputNode);
}


bool Parameter::getNodeByPathSegments(const YAML::Node& node, const std::vector<std::string>& segments, size_t index, YAML::Node& outputNode) const {
    if (index >= segments.size()) {
        outputNode = node;
        return true;
    }

    const std::string& segment = segments[index];
    size_t bracketPos = segment.find('[');

    if (bracketPos != std::string::npos) {
        std::string key = segment.substr(0, bracketPos);
        size_t endBracketPos = segment.find(']', bracketPos);
        int arrayIndex = std::stoi(segment.substr(bracketPos + 1, endBracketPos - bracketPos - 1));

        if (node[key] && node[key][arrayIndex]) {
            return getNodeByPathSegments(node[key][arrayIndex], segments, index + 1, outputNode);
        }
    } else {
        if (node[segment]) {
            return getNodeByPathSegments(node[segment], segments, index + 1, outputNode);
        }
    }

    return false;
}


/** 成员函数6:
@brief 对按地址查找的函数进行进一步封装，由于采用了模板函数，所以更安全地在头文件定义
*/


/** 成员函数7：
@brief 传入节点的路径，查找对应节点下一级的所有子节点，子节点可以是标量，映射和序列
@param 参数1: 节点的路径，用'.'和[]表示，参数2:引用参数，存储所有找到的子节点
@return 如果有子节点，则返回为true; 如果无子节点或路径无效，返回为false 
*/
// 给出YAML文件节点的路径，查询这个节点的所有子节点
bool Parameter::getChildNodes(const std::string& path, std::vector<std::string>& childNodes) const {
    // 使用新的getNodeByPath函数来获取指定路径的节点
    YAML::Node targetNode = getNodeByPath(root, path);

    // 检查targetNode是否有效
    if (!targetNode) {
        std::cout << "The specified path does not exist: " << path << std::endl;
        return false;
    }

    // 如果targetNode是映射，遍历所有键
    if (targetNode.IsMap()) {
        for (YAML::const_iterator it = targetNode.begin(); it != targetNode.end(); ++it) {
            childNodes.push_back(it->first.as<std::string>());
        }
    }
    // 如果targetNode是序列，使用索引作为子节点的名称
    else if (targetNode.IsSequence()) {
        for (size_t i = 0; i < targetNode.size(); ++i) {
            childNodes.push_back(std::to_string(i));
        }
    }
    // 如果targetNode既不是映射也不是序列，则它没有子节点
    else {
        return false; // 没有子节点
    }

    return true; // 成功找到子节点
}


/**先注释4.7 10:08
@brief 私有成员函数，用于查找子节点
@param startNode 起始节点，可以是root或其他任意YAML::Node
@param path 节点的路径
@return 找到的YAML::Node。如果路径无效或找不到节点，返回一个空的YAML::Node对象。
*/
/*
YAML::Node Parameter::getNodeByPath(const YAML::Node& startNode, const std::string& path) const {
    std::istringstream iss(path);
    std::string segment;
    YAML::Node current = startNode; // 从提供的节点开始操作
	std::cout << "Starting node for path '" << path << "':\n" << current << std::endl; // 打印起始节点

    while (std::getline(iss, segment, '.')) {
        if (segment.empty()) continue; // 忽略空段

		std::cout << "Processing segment: " << segment << std::endl; // 打印当前处理的路径段

        // 处理数组索引
        size_t bracketPos = segment.find('[');
        if (bracketPos != std::string::npos) {
            std::string key = segment.substr(0, bracketPos);
            size_t endPos = segment.find(']', bracketPos);
            if (endPos == std::string::npos) {
				std::cout << "Invalid index format in path.\n";
                return YAML::Node(); // 无效的索引格式
            }

            int index = std::stoi(segment.substr(bracketPos + 1, endPos - bracketPos - 1));
			std::cout << "Key: " << key << ", Index: " << index << std::endl; // 打印数组键和索引

            if (current[key] && index < static_cast<int>(current[key].size())) {
                current = current[key][index];
            } else {
				std::cout << "Invalid index or key in array segment.\n";
                return YAML::Node(); // 索引无效
            }
        } else {
            // 处理常规键
            if (current[segment]) {
                current = current[segment];
            } else {
				std::cout << "Key '" << segment << "' does not exist.\n";
                return YAML::Node(); // 键不存在
            }
        }
    	std::cout << "Current node after processing '" << segment << "':\n" << current << std::endl; // 打印当前节点的状态
	}

    return current;
}
*/


YAML::Node Parameter::getNodeByPath(const YAML::Node& startNode, const std::string& path) const {
	// 创建一个临时副本，深拷贝startNode，因为startNode一般传入root
	YAML::Node tempNode = YAML::Clone(startNode);  
	YAML::Node currentNode = tempNode;
	std::istringstream pathStream(path);
    std::string segment;

    while (getline(pathStream, segment, '.')) {
        std::smatch match;
        std::regex arrayIndexPattern("(.+)\\[(\\d+)\\]"); // 用于匹配带索引的路径段，如 "students[0]"

        if (std::regex_match(segment, match, arrayIndexPattern)) {
            std::string key = match[1]; // 路径段的键部分，如 "students"
            int index = std::stoi(match[2]); // 索引部分，如 0

            if (currentNode[key] && currentNode[key].IsSequence() && index < currentNode[key].size()) {
                currentNode = currentNode[key][index];
            } else {
                return YAML::Node(); // 如果路径无效或索引超出范围，返回空节点
            }
        } else {
            if (currentNode[segment]) {
                currentNode = currentNode[segment];
            } else {
                return YAML::Node(); // 如果找不到键，返回空节点
            }
        }
    }

    return currentNode;
}


/**
@brief 私有成员函数，用于查找子节点

YAML::Node Parameter::getNodeByPath(const std::string& path) const {
    std::istringstream iss(path);
    std::string segment; 
    YAML::Node current = root; // 使用副本进行操作，保持root不变

    while (std::getline(iss, segment, '.')) {
        if (segment.empty()) continue; // 忽略空段
        
        // 处理数组索引
        size_t bracketPos = segment.find('[');
        if (bracketPos != std::string::npos) {
            std::string key = segment.substr(0, bracketPos);
            size_t endPos = segment.find(']', bracketPos);
            if (endPos == std::string::npos) {
                return YAML::Node(); // 无效的索引格式
            }
            
            int index = std::stoi(segment.substr(bracketPos + 1, endPos - bracketPos - 1));
            if (current[key] && index < static_cast<int>(current[key].size())) {
                current = current[key][index]; // 直接操作current，而不是尝试获取其地址
            } else {
                return YAML::Node(); // 索引无效
            }
        } else {
            // 处理常规键
            if (current[segment]) {
                current = current[segment];
            } else {
                return YAML::Node(); // 键不存在
            }
        }
    }

    return current;
}
*/

/** 成员函数8
@brief 输入节点路径，打印下一级所有子节点
@param 输入节点路径
@return 如果节点路径有效或children不为空，返回true
*/
bool Parameter::printChildrenByPath(const std::string& path) const {
	std::vector<std::string> children;
    if (getChildNodes(path, children)) {
        std::cout << "Children of '" << path << "':" << std::endl;
        for (const auto& child : children) {
            std::cout << "- " << child << std::endl;
        }
        return true;
    } else {
        std::cout << "No children found or path is invalid for '" << path << "'." << std::endl;
        return false;
    }
}


/** 成员函数9
@brief 给定节点路径，查询所有子节点的类型。
@param 参数1: 节点路径; 参数2: 容器，存储子节点类型
@return 如果路径存在，则返回true
*/
/*
bool Parameter::getNodeTypesAtPath(const std::string& path, std::vector<std::string>& nodeTypes) const {
    const YAML::Node node = getNodeByPath(path);
    if (!node || !node.IsDefined()) {
        return false; // 路径不存在或节点未定义
    }
        
    if (node.IsMap()) {
        for (auto it = node.begin(); it != node.end(); ++it) {
            nodeTypes.push_back("Map");
        }
    } else if (node.IsSequence()) {
        for (size_t i = 0; i < node.size(); ++i) {
            nodeTypes.push_back("Sequence");
        }
    } else if (node.IsScalar()) {
        nodeTypes.push_back("Scalar");
    }

    return true;
}
*/

bool Parameter::getNodeTypesAtPath(const std::string& path, std::vector<std::string>& nodeTypes) const {
    const YAML::Node node = getNodeByPath(root, path);
    if (!node || !node.IsDefined()) {
        return false; // 路径不存在或节点未定义
    }   
    
    // 遍历映射的子节点
    if (node.IsMap()) {
        for (auto it = node.begin(); it != node.end(); ++it) {
            if (it->second.IsMap()) {
                nodeTypes.push_back("Map");
            } else if (it->second.IsSequence()) {
                nodeTypes.push_back("Sequence");
            } else if (it->second.IsScalar()) {
                nodeTypes.push_back("Scalar");
            }
        }
    }
    // 遍历序列的子节点
    else if (node.IsSequence()) {
        for (size_t i = 0; i < node.size(); ++i) {
            if (node[i].IsMap()) {
                nodeTypes.push_back("Map");
            } else if (node[i].IsSequence()) {
                nodeTypes.push_back("Sequence");
            } else if (node[i].IsScalar()) {
                nodeTypes.push_back("Scalar");
            }
        }
    }
    // 单个标量节点不包含子节点，但这里可以添加处理逻辑
    else if (node.IsScalar()) {
        nodeTypes.push_back("Scalar");
    }

    return true;
}


/** 成员函数10
@brief 打印所有子节点类型
@param 参数: 节点路径
*/

void Parameter::printNodeTypesAtPath(const std::string& path) const {
    std::vector<std::string> nodeTypes;
    if (getNodeTypesAtPath(path, nodeTypes)) {
        std::cout << "Node types at '" << path << "':" << std::endl;
        for (const auto& type : nodeTypes) {
            std::cout << "- " << type << std::endl;
        }
    } else {
        std::cout << "Path '" << path << "' not found or has no children." << std::endl;
    }
}


// 打印根节点信息的函数
void Parameter::printRootInfo() const{
	if (root.IsNull()) {
        std::cout << "Root is Null." << std::endl;
    } else if (root.IsScalar()) {
        std::cout << "Root is a Scalar: " << root.as<std::string>() << std::endl;
    } else if (root.IsSequence()) {
        std::cout << "Root is a Sequence. Size: " << root.size() << std::endl;
    } else if (root.IsMap()) {
        std::cout << "Root is a Map. Size: " << root.size() << std::endl;
    }

	if (root.IsMap()) {
        for (YAML::const_iterator it = root.begin(); it != root.end(); ++it) {
            std::cout << it->first.as<std::string>() << ": ";
            const YAML::Node& value = it->second;
            if (value.IsScalar()) {
                std::cout << value.as<std::string>();
            } else if (value.IsSequence()) {
                std::cout << "[Sequence]";
            } else if (value.IsMap()) {
                std::cout << "{Map}";
            }
            std::cout << std::endl;
        }
    } else {
        std::cout << "Root is not a Map. Use appropriate method to print its content." << std::endl;
    }
}


// 在Parameter类中添加这个公共成员函数
void Parameter::testGetNodeByPath(const std::string& path) const {
    std::cout << "Before getNodeByPath, root content is: \n";
    // 假设这里有一个简单的方式来打印 root 的简略信息或者是特定的部分信息
    printRootInfo();   // 打印调用前的root信息

	YAML::Node node = getNodeByPath(root, path); // 调用私有成员函数获取节点

	std::cout << "After getNodeByPath, root content is: \n";
    // 同样，再次打印root的信息，检查其是否发生变化
    printRootInfo();   // 打印调用后的root信息
    
	if (!node) {
        std::cout << "Node not found at '" << path << "'" << std::endl;
    } else if (node.IsNull()) {
        std::cout << "Node at '" << path << "' is Null." << std::endl;
    } else if (node.IsScalar()) {
        std::cout << "Node found at '" << path << "' with value: " << node.Scalar() << std::endl;
    } else if (node.IsSequence()) {
        std::cout << "Node found at '" << path << "' is a Sequence." << std::endl;
    } else if (node.IsMap()) {
        std::cout << "Node found at '" << path << "' is a Map." << std::endl;
    }
}

/**
@brief 给定节点内容，搜索节点的路径。特点是可以从自定义的任一节点开始查找
@param 参数1：节点内容, 可能是标量、映射的值、序列的一个元素；参数2：引用参数，用于存储节点路径
@return 如果节点的值存在，返回为true
@note 可以设定从任一节点开始查找，这么做的好处是：1.可以提高查找效率；2. 可以提高查找准确性
*/
bool Parameter::findPathByValue(const YAML::Node& node, const std::string& valueToFind, std::vector<std::string>& paths, const std::string& currentPath) const {
	bool found = false;  // 标记是否找到至少一个匹配的值
	if (!node.IsDefined()) {
		return found; // 节点未定义
	}

	if (node.IsScalar() && node.as<std::string>() == valueToFind) {
		paths.push_back(currentPath);  // 存储当前路径
		found = true;  // 找到匹配的标量值
	} else if (node.IsSequence()) {
		for (size_t i = 0; i < node.size(); i++){
			std::string newPath = currentPath.empty() ? ("[" + std::to_string(i) + "]") : (currentPath +"[" + std::to_string(i) + "]");
			if (findPathByValue(node[i], valueToFind, paths, newPath)){
				found = true;   // 找到匹配的序列元素
			}
		}
	} else if (node.IsMap()) {
		for (auto it = node.begin(); it != node.end(); it++){
			std::string newPath = currentPath.empty() ? it->first.as<std::string>() : (currentPath + "." + it->first.as<std::string>());
			if (findPathByValue(it->second, valueToFind, paths, newPath)) {
				found = true;   // 找到匹配的映射值
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
bool Parameter::findPathByValueInRoot(const std::string& valueToFind, std::vector<std::string>& paths) const{
	return findPathByValue(root, valueToFind, paths, "");
}


/**
@brief 给定节点内容，打印所有的路径。
@param 输入要查找的节点的内容（注意是: "标量、序列中的元素、键值对的值"）
*/
void Parameter::printAllPathsByValue(const std::string& valueToFind) const {
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
bool Parameter::findAllPathsByKey(const YAML::Node& node, const std::string& keyToFind, std::vector<std::string>& paths, const std::string& currentPath) const {
    bool found = false;
    if (!node.IsDefined()) {
        return false;
    }
        
    if (node.IsMap()) {
        for (YAML::const_iterator it = node.begin(); it != node.end(); ++it) {
            std::string newKey = it->first.as<std::string>();
            std::string newPath = currentPath.empty() ? newKey : currentPath + "." + newKey;
            if (newKey == keyToFind) {
                paths.push_back(newPath);
                found = true;
            }
            // 继续递归搜索
            if (findAllPathsByKey(it->second, keyToFind, paths, newPath)) {
                found = true;
            }
        }
    } else if (node.IsSequence()) {
        for (size_t i = 0; i < node.size(); ++i) {
            std::string newPath = currentPath + "[" + std::to_string(i) + "]";
            // 对序列中的每个元素，继续递归搜索
            if (findAllPathsByKey(node[i], keyToFind, paths, newPath)) {
                found = true;
            }
        }
    }
    return found;
}


/**
@brief 从根节点开始查找键名对应的所有的路径，存储至容器paths中
@params 参数1 keyToFind: 要查找的键名，参数2 paths: 用于存储所有路径的容器
@return 如果键名的确存在，返回true
@note 一般使用这个搜索键名 
*/
bool Parameter::findAllPathsByKeyInRoot(const std::string& keyToFind, std::vector<std::string>& paths) const {
    return findAllPathsByKey(root, keyToFind, paths);
}


/**
@brief 打印键名对应的所有路径
@param 要查询的键名 
*/
void Parameter::printAllPathsByKey(const std::string& keyToFind) const{
	std::vector<std::string> paths;
	if(findAllPathsByKeyInRoot(keyToFind, paths)){
		std::cout << "Found paths for key '"<< keyToFind << "':" << std::endl;
		for (const auto& path : paths){
			std::cout << "- " << path << std::endl;
		}
	} else {
		std::cout << "Key '" << keyToFind << "' not found." << std::endl;
	}
}


/**
@brief 
@param
@return
*/
bool Parameter::addorUpdateNode(const std::string& path, const YAML::Node& value, const std::string& key) {
    std::istringstream pathStream(path);
    std::string segment;
    YAML::Node currentNode = YAML::Clone(root); // 假设root是根节点

    while (std::getline(pathStream, segment, '.')) {
        if (segment.empty()) continue;
        
        size_t bracketPos = segment.find('[');
        if (bracketPos != std::string::npos) {
            // 处理序列
            std::string mapKey = segment.substr(0, bracketPos);
            size_t endBracketPos = segment.find(']', bracketPos);
            int index = std::stoi(segment.substr(bracketPos + 1, endBracketPos - bracketPos - 1));
            
            // 确保序列足够长
            while (currentNode[mapKey].size() <= index) {
                currentNode[mapKey].push_back(YAML::Node());
            }
            currentNode = currentNode[mapKey][index];
        } else {
            // 处理映射
            if (!currentNode[segment]) {
                currentNode[segment] = YAML::Node();
            }
            currentNode = currentNode[segment];
        }
    }

    // 设置节点值
    if (!key.empty()) {
        currentNode[key] = value;
    } else {
        currentNode = value;
    }

    return true;
}


/**
@brief 函数重载，当接收两个参数时执行修改内容命令
@param
@return
*/
bool Parameter::addorUpdateNode(const std::string& path, const YAML::Node& value) {
	return addorUpdateNode(path, value, "");
}


/**
@brief 将修改后的新YAML文件保存在“../output/”路径下
@param 参数: 文件名
*/
void Parameter::saveYamlToFile(const std::string& filename) const {
    // 构建完整的文件路径
    std::string outputPath = "../output/" + filename;

    std::ofstream outFile(outputPath);
    if (!outFile.is_open()) {
        std::cerr << "Failed to open file for writing: " << outputPath << std::endl;
        return;
    }

    outFile << root;
    outFile.close();

    if (outFile.good()) {
        std::cout << "Successfully saved YAML content to " << outputPath << std::endl;
    } else {
        std::cerr << "Failed to write YAML content to " << outputPath << std::endl;
    }
}



// 成员函数8:
// 本函数为了实现如下功能：给出节点的具体路径，增加这个路径下的节点
// 本函数要求输入yaml文件路径，指定要加入节点的路径，新增节点的名称，以及新增节点的内容
// 其中AddNodeToYaml函数为实现增加功能的函数
// 函数，增加节点到YAML中指定的路径
// root为文件的根节点，path为一个字符串相连，表示节点的路径，每个字符串代表路径中的一个节点名称
// nodeName是新增加节点的名称，newNode是新增加节点的内容
/*
bool Parameter::AddNodeToYaml(const std::string& pathStr, const std::string& nodeName, const YAML::Node& newNode) {
	if (configs.find(currentYamlFilename) == configs.end()) {
		std::cerr << "YAML file not loaded." << std::endl;
		return false;
	}

	// 将路径字符串转换为向量
	std::vector<std::string> path = SplitPath(pathStr);
	YAML::Node root = configs[currentYamlFilename]; // 使用已加载的YAML内容

	return AddNode(root, path, nodeName, newNode);
}
*/

// 成员函数9:将当前YAML内容保存到新文件
/*
bool Parameter::SaveYamlToFile(const std::string& newFilename, const std::string& savePath) {
	// 检查是否已加载YAML内容
	if (configs.find(currentYamlFilename) == configs.end()) {
		std::cerr << "No YAML content loaded to save." << std::endl;
		return false;
	}

	// 构建新文件的完整路径
	std::string newFilePath = savePath + "/" + newFilename;

	// 获取当前操作的YAML内容
	YAML::Node contentToSave = configs[currentYamlFilename];
	
	// 将YAML内容序列化并写入新文件
	try {
		std::ofstream fout(newFilePath);
		fout << YAML::Dump(contentToSave);
	} catch (const std::exception& e) {
		std::cerr << "Failed to save YAML to " << newFilename << ": " << e.what() << std::endl;
		return false;
	}

	return true;
}
*/

// 成员函数9: 
// 本函数实现了修改节点的功能，主体函数为
// bool ModifyNode(YAML::Node& root, const std::vector<std::string>& path, const YAML::Node& newValue, const std::string& newKey = "");
// 输入文件root、节点路径path、新值节点newValue、新键名newKey（可选）如果需要改映射的键名，请提供新键名

// 如果节点是标量的话，修改标量的内容
// 如果节点是序列的话，修改序列的内容
// 如果节点是映射的话，修改值的内容或修改键名
/*
bool Parameter::ModifyNode(const std::string& pathStr, const YAML::Node& newValue, const std::string& newKey) {
	if (configs.find(currentYamlFilename) == configs.end()) {
		std::cerr << "YAML file not loaded." << std::endl;
		return false;
	}

	// 将路径字符串转换为向量
	std::vector<std::string> path = SplitPath(pathStr);
	YAML::Node& root = configs[currentYamlFilename]; // 使用已加载的YAML内容

	return ModifyNodeInternal(root, path, newValue, newKey);
}
*/

// 成员函数10: 删除某路径下的节点，此功能不建议使用，yaml-cpp/yaml.h库删除功能并不完善
/*
bool Parameter::RemoveNode(const std::string& pathStr) {
	if (configs.find(currentYamlFilename) == configs.end()) {
		std::cerr << "YAML file not loaded." << std::endl;
		return false;
	}

	// 将路径字符串转换为向量
	std::vector<std::string> path = SplitPath(pathStr);
	YAML::Node& root = configs[currentYamlFilename]; // 使用已加载的YAML内容

	if (path.empty()) {
		std::cerr << "Error: Path cannot be empty." << std::endl;
		return false;
	}

	YAML::Node current = root;
	for (size_t i = 0; i < path.size() - 1; ++i) { // Navigate to the parent of the target node
		current = current[path[i]];
		if (!current) {
			std::cerr << "Error: Invalid path at " << path[i] << std::endl;
			return false;
		}
	}

	std::string targetKey = path.back();
	if (current.IsMap() && current[targetKey]) {
		current.remove(targetKey); // Remove the target key from a Map node
		return true;
	} else if (current.IsSequence()) {
		// Assuming targetKey is an index for a Sequence node
		try {
			int index = std::stoi(targetKey);
			if (index >= 0 && index < static_cast<int>(current.size())) {
				current.remove(index); // Remove the indexed element from a Sequence node
				return true;
			}
		} catch(const std::exception& e) {
			std::cerr << "Error: Invalid index - " << e.what() << std::endl;
		}
	}

	std::cerr << "Error: The target node for removal is not found or the path is incorrect." << std::endl;
	return false;
}
*/

// 获取当前操作的YAML文件内容
YAML::Node Parameter::GetYamlContent() const {
	if (configs.find(currentYamlFilename) != configs.end()) {
		return configs.at(currentYamlFilename);
	} else {
		return YAML::Node(); // 返回一个空的YAML::Node对象
	}
}

Parameter::~Parameter(){
	std::cout << "Parameter is deleted." << std::endl;	
}



// loadYamlFile函数,加载特定路径的yaml文件
void Parameter::loadYamlFile(const std::string& yamlFilePath) {
	fs::path filePath = fs::current_path() / yamlFilePath;
	try {
		if(fs::exists(filePath) && fs::is_regular_file(filePath)) {
			// 加载YAML文件到root成员变量
			root = YAML::LoadFile(filePath.string());
			if (root.IsNull()) {
                std::cerr << "Warning: The file '" << filePath.filename() << "' is empty or does not exist." << std::endl;
            } else {
                std::cout << "Loaded '" << filePath.filename() << "'" << std::endl;
            }
			// 假设我们仅通过文件名作为键，这里去掉路径只保留文件名
			configs[filePath.filename().string()] = root;
			std::cout << "Loaded " << filePath.filename() << std::endl;
		} else {
			std::cerr << "Error: File does not exist or is not a regular file." << std::endl;
		}
	} catch (const YAML::BadFile& e) {
		// 特定的异常处理（无法找到或打开文件）
		std::cerr << "Error: Unable to open file '" << filePath.filename() << "'. Exception message: " << e.what() << std::endl;
	} catch (const YAML::ParserException& e) {
		// 解析异常
		std::cerr << "Error: Parser error at " << e.mark.line + 1 << ":" << e.mark.column + 1 << ". Message: " << e.msg << std::endl;
	} catch (const YAML::Exception& e) {
		// 处理YAML库抛出的其他所有异常
		std::cerr << "Error: An error occurred while processing '" << filePath.filename() << "'. Exception message: " << e.what() << std::endl;
	} catch (const std::exception& e) {
		// 处理所有未被前面捕获的标准异常
		std::cerr << "Error: An unexpected error occurred. Exception message: " << e.what() << std::endl;
	}

}


// 函数FindNodeByPath,通过字符串路径找到节点（而非索引路径）
/*
YAML::Node Parameter::FindNodeByPath(const YAML::Node& root, const std::vector<std::string>& path) {
	YAML::Node current = root;
	for (const auto& key : path) {
		if (!current || current.Type() != YAML::NodeType::Map) {
			std::cerr << "Invalid path or node type at key: " << key << std::endl;
			return YAML::Node(); // 返回一个无效节点
		}
		current = current[key];
	}
	return current;
}
*/

// 此函数是为了实现增加功能
/*
bool Parameter::AddNode(YAML::Node& root, const std::vector<std::string>& path, const std::string& nodeName, const YAML::Node& newNode) {
	YAML::Node current = root;
	for (size_t i = 0; i < path.size(); ++i) {
		// 检查是否到达路径的最后一个节点
		if (i == path.size() - 1) {
			// 如果是最后一个节点，则在此处添加新节点
			// 这里需要判断current是否是Map类型，因为我们想要添加键值对
			if (current.Type() != YAML::NodeType::Map) {
				std::cerr << "Error: Trying to add a node to a non-map type." << std::endl;
				return false;
			}
			current[nodeName] = newNode; // 添加新节点
			return true;
		} else {
			// 如果不是最后一个节点，就继续沿路径向下
			// 这里同样检查current是否是Map类型，因为我们需要通过键来访问子节点
			if (current.Type() != YAML::NodeType::Map) {
				std::cerr << "Error: Intermediate path component is not a map." << std::endl;
				return false;
			}
			// 检查下一个路径节点是否存在，如果不存在则创建一个新的Map节点
			if (!current[path[i]]) {
				current[path[i]] = YAML::Node(YAML::NodeType::Map);
			}
			current = current[path[i]];
		}
	}
	return false; // 如果由于某种原因未能添加节点，则返回false
}


// 实际执行修改操作的内部函数
bool Parameter::ModifyNodeInternal(YAML::Node& current, const std::vector<std::string>& path, const YAML::Node& newValue, const std::string& newKey) {
	for (size_t i = 0; i < path.size(); ++i) {
		if (i == path.size() - 1) {
			if (current.Type() == YAML::NodeType::Map) {
				if (!newKey.empty() && newKey != path[i]) {
					current[newKey] = current[path[i]]; // 修改映射的键名
					current.remove(path[i]);
				} else {
					current[path[i]] = newValue; // 修改映射的值内容
				}
				return true;
			} else if (current.Type() == YAML::NodeType::Sequence) {
				int index = std::stoi(path[i]);
				if (index >= 0 && index < static_cast<int>(current.size())) {
					current[index] = newValue; // 修改序列中的元素
					return true;
				}
				return false;
			} else if (current.Type() == YAML::NodeType::Scalar) {
				current = newValue; // 直接修改标量的内容
				return true;
			}
		} else {
			current = current[path[i]];
			if (!current) {
				std::cerr << "Invalid path: " << path[i] << std::endl;
				return false;
			}
		}
	}
	return false;
}
*/	

