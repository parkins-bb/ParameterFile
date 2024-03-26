#include "Parameter.h"

int main(){
	Parameter paramA("A.yaml");
	std::string folderPath = "../";
	std::string extension = ".yaml";

	std::vector<std::string> yamlFiles = paramA.getFilesWithExtension(folderPath, extension);
	std::vector<std::string> nodes = paramA.getYamlNodes();

	// Parameter paramB("B.yaml");   B.yaml是个错误示例
	// 输出获取到的文件名
    std::cout << "Found YAML files:" << std::endl;
    for (const auto& file : yamlFiles) {
        std::cout << file << std::endl;
    }
	// 输出所有节点
	std::cout << "This YAML files All Nodes:" << std::endl;	
	for (const auto& node : nodes){
		std::cout << node << std::endl;
	}

	// 给出具体节点路径,  输出节点类型，父节点和子节点
	std::string nodePath = "companies";
	// 调用GetNodeInfoByPath函数获取节点信息
    auto nodeInfo = paramA.GetNodeInfoByPath(nodePath);

    // 输出节点信息
    std::cout << "Node Type: " << nodeInfo.nodeType << std::endl;
    if (nodeInfo.parentNode) { // 检查是否有父节点信息
        std::cout << "Parent Node: " << *nodeInfo.parentNode << std::endl;
    } else {
        std::cout << "Parent Node: None" << std::endl;
    }

    std::cout << "Child Nodes:" << std::endl;
    for (const auto& child : nodeInfo.childNodes) {
        std::cout << " - " << child << std::endl;
    }

	Parameter paramK("K.yaml");
	/*
	// 查找节点对应的路径
	std::string targetValue = "male";
	std::vector<std::string> path = paramD.GetNodePathByValue(targetValue);

	// 检查路径向量的内容并相应地打印信息
    if (!path.empty() && path[0] != "Node not found.") {
        std::cout << "Found node at path: ";
        for (const auto& step : path) {
            std::cout << step << " ";
		}
        std::cout << std::endl;
    } else {
        // 如果向量为空或包含特定的错误消息，则打印未找到节点的信息
        std::cout << "Node not found." << std::endl;
    }

	// 查找键的路径和值
	std::string targetKey = "gender";
	auto keyResult = paramD.FindAndPrintByKey(targetKey);

	// 打印查找结果
	if (!keyResult.path.empty()) {
        std::cout << "Found path: " << keyResult.path << std::endl;
        std::cout << "Values:" << std::endl;
        for (const auto& value : keyResult.values) {
            std::cout << "- " << value << std::endl;
        }
    } else {
        std::cout << "Key not found." << std::endl;
    }
	*/
	// 增加新节点
	std::string addpath = "Person.age";   // 增加节点的路径
	std::string addnodeName = "age4";     // 增加节点的名称
	YAML::Node newNode = YAML::Load("40");           // 新增加节点的内容
	// 注意：外层双引号用于YAML解析，内层双引号表示字符串的一部分

	// 调用函数添加新节点
	if (paramK.AddNodeToYaml(addpath, addnodeName, newNode)) {
        std::cout << "Node added successfully." << std::endl;
	} else {
        std::cout << "Failed to add node." << std::endl;
    }

	// 保存修改后的内容到新文件
	std::string savePath = "../data";   // 指定新文件的保存路径
	std::string newFilename = "K_added.yaml";    // 指定新文件名
    if (paramK.SaveYamlToFile(newFilename, savePath)) {
    	std::cout << "YAML content saved to " << savePath << "/" << newFilename << std::endl;
    } else {
        std::cout << "Failed to save YAML content." << std::endl;
	}
	
	return 0;
}

