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



/** 成员函数（新增1）： 
 * @brief 给出节点的路径，查找对应的节点的值，并且这个值是一个标量。
 * @param 传入节点搜索路径（用一个点分隔）；引用参数存储找到的这个值
 * @return 如果值为标量，返回“true”来表示成功；值不是标量或没有找到值，函数返回“false”
 */
bool Parameter::getValueByPath(const std::string& path, std::string& outputValue) const{
	std::istringstream iss(path);   // 节点的路径
	std::string segment;  // 路径的一部分，比如aaa.bbb.ccc[2], 取由‘.’分隔的那部分
	const YAML::Node* current = &root;  // 指向当前操作节点的值镇，初始化为指向根节点

	// 按照‘.’分割路径字符串，每次循环处理路径的一部分
	while (std::getline(iss, segment, '.')){
		// 检查路径的当前部分是否表示为一个序列的索引（例如“employee[0]”）
		// 如果是，解析出键名和索引，然后尝试访问对应的节点
		if (segment.find('[') != std::string::npos && segment.find(']') != std::string::npos){
			std::string key = segment.substr(0, segment.find('['));
			int index = std::stoi(segment.substr(segment.find('[') + 1, segment.find(']') - segment.find('[') - 1));
			if (!current->operator[](key)[index]){
				return false;  // 未找到对应的值或者为空
			}
			current = &current->operator[](key)[index];
		}else{
			if(!current->opeator[](segment)){
				return false;   // 未找到对应的值或者为空
			}
			current = &current->operator[](segment);
		}
	}
	
	// 检查最终的节点是不是一个标量，如果是，存储在outputValue中
	if (current->IsScalar()) {
		outputValue = current->as<std::string>();
		return true;
	}else{
		// 如果结果不是标量
		return false;
	}
}


/** 成员函数（新增2）：
@brief 传入节点的路径，查找对应节点下一级的所有子节点，子节点可以是标量，映射和序列
@param 参数1: 节点的路径，用'.'和[]表示，参数2:引用参数，存储所有找到的子节点
@return 如果有子节点，则返回为true; 如果无子节点或路径无效，返回为false 
*/
bool Parameter::getChildrenByPath(const std::string& path, std::vector<std::string>& children) const {
	const YAML::Node* node = getNodeByPath(path);
	if (!node) {
		return false;  // 路径无效
	}

	if (node->IsScalar()){
		// 如果子节点是标量，直接将其值添加到children中
		children.push_back(node->as<std::string>());
	}else if (node->IsSequence()) {
		// 如果子节点是序列，将每个元素的索引添加到children中
		for (size_t i = 0; i < node->size(); i++){
			children.push_back(std::to_string);  // 将索引转换为字符串
		}
	} else if (node->IsMap()) {
		// 如果节点是映射，将每个键的名称添加到children中
		for (YAML::const_iterator it = node->begin(); it != node->end(); it++){
			children.push_back(it->first.as<std::string>());
		}
	}
	
	return true;
}

/**
@brief 私有成员函数，用于查找子节点
*/
const YAML::Node* Parameter::getNodeByPath(const std::string& path) const {
	std::istringstream iss(path);
	std::string segment;
	const YAML::Node* current = &root;

	while (std::getline(iss, segment, '.')) {
		if (segment.find('[') != std::string::npos && segment.find(']') != std::string::npos) {
			std::string key = segment.substr(0, segment.find('['));
			int index = std::stoi(segment.substr(segment.find('[') + 1, segment.find(']') - segment.find('[') - 1));
			current = &(*current)[key][index];
		} else {
			current = &(*current)[segment];
		}
		if (!current) {
			return nullptr;
		}
	}
	return current;
}


/**
@brief 打印下一级所有子节点的函数
@param 输入节点路径
@return 如果节点路径有效或children不为空，返回true
*/
bool Parameter::printChildrenByPath(const std::string& path) const {
	std::vector<std::string> children;
    if (getChildrenByPath(path, children)) {
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


// 成员函数5: 通过路径找到节点，打印当前节点的类型，输出父节点、子节点
Parameter::NodeInfo Parameter::GetNodeInfoByPath(const std::string& internalPath){
	// 分解internalPath为节点路径数组。将路径用“.”分隔开来
	std::vector<std::string> path;
	std::istringstream iss(internalPath);
	std::string item;
	while (std::getline(iss, item, '.')){
		path.push_back(item);
	}

	// 找到指定路径的节点
	YAML::Node targetNode = FindNodeByPath(configs[currentYamlFilename], path);

	// 如果节点有效，获取节点信息
	if (targetNode){
		return ExtractNodeInfo(targetNode);
	} else {
		std::cerr << "Node not found for path: " << internalPath << std::endl;
		return NodeInfo{"Invalid", boost::none, {}};
	}
}


// 成员函数6：给定目标值，返回该值在YAML结构中的路径
std::vector<std::string> Parameter::GetNodePathByValue(const std::string& targetValue) {
	std::vector<std::string> path;
	YAML::Node target = YAML::Load(targetValue); // 从字符串创建YAML节点
	// 使用FindNodePath进行搜索
	if (!FindNodePath(configs[currentYamlFilename], target, path) || path.empty()) {
		// 如果没找到，可以选择返回一个包含特定消息的问题
		return {"Node not found. "};
	}

	return path; // 返回构建的路径信息或未找到的信息
}


// 成员函数7: 此函数为了实现功能：给出一个yaml节点的键名，
// 1. 输出这个键的路径；
// 2. 输出这个键所对应的值，如果节点的值对应是序列的话，也全部输出
// 功能：打印节点值，如果节点是序列，则遍历序列元素
// 修改后的成员函数，返回查找结果
Parameter::FindResult Parameter::FindAndPrintByKey(const std::string& targetKey) {
	FindResult result;
	if (configs.find(currentYamlFilename) != configs.end()) {
		YAML::Node root = configs[currentYamlFilename];
		FindByKey(root, targetKey, "", result);
	}
	return result;
}


// 成员函数8:
// 本函数为了实现如下功能：给出节点的具体路径，增加这个路径下的节点
// 本函数要求输入yaml文件路径，指定要加入节点的路径，新增节点的名称，以及新增节点的内容
// 其中AddNodeToYaml函数为实现增加功能的函数
// 函数，增加节点到YAML中指定的路径
// root为文件的根节点，path为一个字符串相连，表示节点的路径，每个字符串代表路径中的一个节点名称
// nodeName是新增加节点的名称，newNode是新增加节点的内容
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


// 成员函数9:将当前YAML内容保存到新文件
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


// 成员函数9: 
// 本函数实现了修改节点的功能，主体函数为
// bool ModifyNode(YAML::Node& root, const std::vector<std::string>& path, const YAML::Node& newValue, const std::string& newKey = "");
// 输入文件root、节点路径path、新值节点newValue、新键名newKey（可选）如果需要改映射的键名，请提供新键名

// 如果节点是标量的话，修改标量的内容
// 如果节点是序列的话，修改序列的内容
// 如果节点是映射的话，修改值的内容或修改键名
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


// 成员函数10: 删除某路径下的节点，此功能不建议使用，yaml-cpp/yaml.h库删除功能并不完善
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
			YAML::Node fileContent = YAML::LoadFile(filePath.string());
			// 假设我们仅通过文件名作为键，这里去掉路径只保留文件名
			configs[filePath.filename().string()] = fileContent;
			std::cout << "Loaded " << filePath.filename() << std::endl;
		} else {
			std::cerr << "Error: File does not exist or is not a regular file." << std::endl;
		}
	} catch (const YAML::Exception& e) {
		std::cerr << "Error loading YAML file: " << e.what() << std::endl;
	}
}

// 函数FindNodeByPath,通过字符串路径找到节点（而非索引路径）
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

// 提取节点信息
Parameter::NodeInfo Parameter::ExtractNodeInfo(const YAML::Node& node) {
	NodeInfo info;
	info.nodeType = NodeTypeToString(node.Type());

	// 假设父节点信息在此上下文中不可获取，初始化为nullopt
	info.parentNode = boost:none;

	// 提取子节点信息
	if (node.IsMap()) {
		for (const auto& iter : node) {
			info.childNodes.push_back(iter.first.as<std::string>());
		}
	} else if (node.IsSequence()) {
		for (std::size_t i = 0; i < node.size(); ++i) {
			info.childNodes.push_back("Sequence item " + std::to_string(i));
		}
	}
	return info;
}

// 节点类型转字符串
std::string Parameter::NodeTypeToString(YAML::NodeType::value type) {
	switch (type) {
		case YAML::NodeType::Null: return "Null";
		case YAML::NodeType::Scalar: return "Scalar";
		case YAML::NodeType::Sequence: return "Sequence";
		case YAML::NodeType::Map: return "Map";
		default: return "Unknown";
	}
}

// FindNodePath实现递归查找
bool Parameter::FindNodePath(const YAML::Node& node, const YAML::Node& target, std::vector<std::string>& path) {
	if (node.Type() == YAML::NodeType::Scalar && node.Scalar() == target.Scalar()) {
		return true; // 找到匹配的标量节点
	} else if (node.Type() == YAML::NodeType::Sequence) {
		for (size_t i = 0; i < node.size(); ++i) {
			path.push_back("[" + std::to_string(i) + "]");
			if (FindNodePath(node[i], target, path)) {
				return true; // 找到匹配的节点
			}
			path.pop_back(); // 回溯
		}
	} else if (node.Type() == YAML::NodeType::Map) {
		for (const auto& iter : node) {
			path.push_back(iter.first.Scalar());
			if (FindNodePath(iter.second, target, path)) {
				return true; // 找到匹配的节点
			}
			path.pop_back(); // 回溯
		}
	}
	return false; // 未找到匹配的节点
}

// 辅助递归函数，用于查找键
bool Parameter::FindByKey(const YAML::Node& node, const std::string& targetKey, const std::string& path, FindResult& result) {
	if (node.IsMap()) {
		for (const auto& element : node) {
			std::string newKey = element.first.as<std::string>();
			std::string newPath = path.empty() ? newKey : path + "." + newKey;
			if (newKey == targetKey) {
				result.path = newPath; // 存储找到的路径
				if (element.second.IsSequence()) {
					for (const auto& item : element.second) {
						result.values.push_back(item.Scalar());
					}
				} else {
					result.values.push_back(element.second.Scalar()); // 存储值
				}
				return true;
			}
			if (FindByKey(element.second, targetKey, newPath, result)) {
				return true;
			}
		}
	} else if (node.IsSequence()) {
		for (size_t i = 0; i < node.size(); ++i) {
			std::string newPath = path + "[" + std::to_string(i) + "]";
			if (FindByKey(node[i], targetKey, newPath, result)) {
				return true;
			}
		}
	}
	return false;
}

// 此函数是为了实现增加功能
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

// 此函数是用于分离路径字符串，比如"Person.age"分离为["Person", "age"]
std::vector<std::string> Parameter::SplitPath(const std::string& pathStr) {
	std::vector<std::string> path;
	std::stringstream ss(pathStr);
	std::string item;
	while (getline(ss, item, '.')) { // 假设路径中的节点由'.'分隔
		path.push_back(item);
	}
	return path;
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
	

