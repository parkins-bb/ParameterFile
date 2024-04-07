#include "Parameter.h" // 确保你的类定义在这个文件里
#include <iostream>

int main() {
	// ----------------------------------------------------------------------------------------------------------
	// 成员函数，按节点内容搜索节点路径(所有路径)，函数提供了存储方法，案例中直接调用打印函数
	Parameter paramH("H.yaml");
	int valueH_1 = 9;
	paramH.printAllPathsByValue(valueH_1);
	std::string valueH_2 = "Jane Doe";
	paramH.printAllPathsByValue(valueH_2);


	// --------------------------------------------------------------------------------------------------------
	// 成员函数，根据节点键名搜索节点路径(所有路径)，函数提供了存储方法，案例中之间调用打印函数
	Parameter paramF("F.yaml");
	std::string keyF_1 = "name";
	paramF.printAllPathsByKey(keyF_1);
	std::string keyF_2 = "lead";
	paramF.printAllPathsByKey(keyF_2);


	// ----------------------------------------------------------------------------------------------------------
	// 成员函数，更新yaml文件节点(包括增加和修改)，直接存储到新文件夹中
	// 创建对象，加载J.yaml文件
	Parameter paramJ("J.yaml");

	// 更新数据库密码
	paramJ.addorUpdateNode("database.credentials", YAML::Load("{\"password\": \"123456\"}"));
	
	// 向用户列表中添加一个新用户
	YAML::Node newUser = YAML::Load("{\"name\": \"Alex Smith\", \"age\": 28, \"email\": \"alexsmith@gmail.com\"}");
	paramJ.addorUpdateNode("users[2]", newUser);

	paramJ.printRoot();

	// 保存更改到新的YAML文件
	paramJ.saveYamlToFile("new_J.yaml"); 

    return 0;
}

