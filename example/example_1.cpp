#include "Parameter.h"

int main(){
	
	// 创建对象，构造函数加载T.yaml
	Parameter paramT("T.yaml");

	// ----------------------------------------------------------------------------------------------------
	// 成员函数1示例: 获取指定文件夹(名称为data)下指定的后缀的文件名
	std::string folderPath = "../";
	std::string extension = ".yaml"; 
	std::vector<std::string> FilesIndata = paramT.getFilesWithExtension(folderPath, extension);
	// 打印所有.yaml后缀文件名
	std::cout << "Found YAML files in data: " << std::endl;
	for (const auto& file : FilesIndata){
		std::cout << file << std::endl;
	}


	// ----------------------------------------------------------------------------------------------------
	// 成员函数2示例: 获取指定文件夹下(名称为data)下所有yaml文件
	std::vector<std::string> yamlFiles = paramT.searchYamlFiles(folderPath);
	// 打印所有.yaml后缀文件名
	std::cout << "Found YAML files in data(using searchYamlFiles()): " << std::endl;
	for (const auto& file_yaml : yamlFiles){
		std::cout << file_yaml << std::endl;
	}


	// ----------------------------------------------------------------------------------------------------
	// 成员函数3示例: 将数组写入文件的函数
	std::string outputYAMLname = "All yaml Files.txt"; 
	paramT.writeToFile(yamlFiles, outputYAMLname);


	// ----------------------------------------------------------------------------------------------------
	// 成员函数4示例: 获取yaml文件的所有节点
	std::vector<std::string> nodes = paramT.getYamlNodes();
	// 让我们把所有节点打印出来
	for (const std::string& node : nodes) {
		std::cout << node << std::endl;
	}


	// ------------------------------------------------------------------------------------------------
	// 成员函数（新增1）: 根据节点的路径，查找对应节点的内容

	// 例子1: 简单路径输出内容
	Parameter paramD("D.yaml");	
	std::string path_1 = "Person.gender[1]";	// 路径示例
	YAML::Node result_1;  // 存储节点的值
	if (paramD.getValueByPath(path_1, result_1)) {
		std::cout << "Value at path '" << path_1 << "': " << result_1 << std::endl;
	} else {
		std::cout << path_1 << "not found. " << std::endl;
	}

	std::string path_2 = "Person.is_student[2]";
	YAML::Node result_2;
	paramD.getValueByPath(path_2, result_2);
	std::cout << "Value at path" << path_2 << ":" << result_2 << std::endl;
    
	/* 简单路径示例, 案例在../data/D.yaml
	std::vector<std::string> paths = {
	"Person.name[0]",       // 应该返回John Doe
	"Person.age.age2",      // 应该返回20
	"Person.gender",        // 应该返回male, female, unknown
	"Person.is_student[1]"  // 应该返回false
	}
	*/
	
	// 例子2: 复杂路径输出内容
    Parameter paramF("F.yaml");
	std::string path_3 = "company.employees[1].skills[0]";   // 复杂路径示例
	YAML::Node result_3;  // 存储节点的值
	if (paramF.getValueByPath(path_3, result_3)){
		std::cout << "employees skills: " << result_3 << std::endl;
	} else {
		std::cout << path_3 << "not found. " << std::endl;
	}

	std::string path_4 = "company.projects[1].lead";
	YAML::Node result_4;
	paramF.getValueByPath(path_4, result_4);
	std::cout << "Value at path" << path_4 << ":" << result_4 << std::endl;


	// ----------------------------------------------------------------------------------------------------------
	// 成员函数（新增2）:采用模板编程，自动获取节点值类型, 简单快捷，且能生成对应的数据类型
	// 测试提取不同类型的节点值
    // 获取第一部电影的评分
	Parameter paramB("B.yaml");
	
	double movieRating;
	
    paramB.fetchValueAtPath("movies[0].rating", movieRating);
    std::cout << "Rating: " << movieRating << std::endl;
    
    // 获取第一部电影的类型
	std::string movieGenres;
    paramB.fetchValueAtPath("movies[0].genres[1]", movieGenres);
    std::cout << "Genres[1]: ";
	
    for (const auto& genre : movieGenres) {
        std::cout << genre << " ";
    }
	
    std::cout << std::endl;
	
    // 检查第一部电影是否在流媒体上可用
	bool isAvailableOnStreaming;	
    
	paramB.fetchValueAtPath("movies[0].availableOnStreaming", isAvailableOnStreaming);
    std::cout << "Available on Streaming: " << std::boolalpha << isAvailableOnStreaming << std::endl;
	
    // 获取第一部电影的标题
	std::string movieTitle;	
    paramB.fetchValueAtPath("movies[0].title", movieTitle);
    std::cout << "Title: " << movieTitle << std::endl;
	
    // 获取第一部电影的上映年份
	int movieYear;
    paramB.fetchValueAtPath("movies[0].year", movieYear);
    std::cout << "Year: " << movieYear << std::endl;
	

	// ---------------------------------------------------------------------------------------------------------
	// 成员函数7：给出节点路径，查找所有子节点并存储；成员函数8：打印所有子节点
	paramD.printChildrenByPath("Person.name");
	paramD.printChildrenByPath("Person.age");
	

	paramF.printChildrenByPath("company");
	paramF.printChildrenByPath("company.projects[1]");
	

	// --------------------------------------------------------------------------------------------------------
	// 成员函数9：给出节点路径，查询所有子节点类型；成员函数10：打印所有子节点类型
	paramD.printNodeTypesAtPath("Person.name[1]");
	paramD.printNodeTypesAtPath("Person.age");
	paramF.printNodeTypesAtPath("company.name");
	paramF.printNodeTypesAtPath("company.employees[1]");
	

	return 0;
}

