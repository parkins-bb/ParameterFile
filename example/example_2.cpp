#include "Parameter.h" // 确保你的类定义在这个文件里
#include <iostream>

int main() {
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

