#include <iostream>
#include <string>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#define local "__local__"
#define tagForLocalVar "__cuda_local_var_"
#define tagForGlobal "__global__"
#define tagForSharedMem "__shared__"
#define tagForDevice "__device__"
#define intTag "int"
#define floatTag "float"
#define TypedefTag "typedef"
#define structureTag "struct"
using namespace std;
void split(const string &s, const char* delim, vector<string> & v){
    // to avoid modifying original string
    // first duplicate the original string and return a char pointer then free the memory
    char * dup = strdup(s.c_str());
    char * token = strtok(dup, delim);
    while(token != NULL){
        v.push_back(string(token));
        // the call is treated as a subsequent calls to strtok:
        // the function continues from where it left in previous invocation
        token = strtok(NULL, delim);
    }
    free(dup);
}
class parameter_func{
public:
	string type;
	string name;
	parameter_func(){};
	parameter_func(string type, string name){
		this->type=type;
		this->name = name;
	}
	friend ostream& operator << (ostream& output,parameter_func& a){
		output<<"("<<a.type<<","<<a.name<<")";
		return output;
	}
};
class local_variable{
public:
	parameter_func para;
	string function_source;
	local_variable(){
		function_source = "null";
	};
	local_variable(parameter_func &input){
		para = input;
	}
	bool add_source(string a){
		function_source = a;
		return true;
	}
	friend ostream& operator << (ostream& output,local_variable& a){
		output<<"("<<a.function_source<<")";
		output<<a.para;
		return output;
	}
};
class local_variable_struct{
public:
	string name;
	string type;
	vector<local_variable> local_variable1;
	local_variable_struct(){};
	local_variable_struct(vector<string> &input){
		name = "vals";
		type = "vals*";
		for (vector<string>::iterator it = input.begin() ; it != input.end();)
		{
			string a,b;
			a = *it;
			b = *(it+1);
			parameter_func *tmp1 = new parameter_func(a,b);
			local_variable *tmp = new local_variable(*tmp1);
			local_variable1.push_back(*tmp);
			it = it+2;
		}
	}
	friend ostream& operator << (ostream& output,local_variable_struct& a){
		output<<"("<<a.type<<","<<a.name<<")"<<endl;
		for (vector<local_variable>::iterator it = (a.local_variable1).begin() ; it != (a.local_variable1).end(); it++)
		{
			output<<*it<<endl;
		}
		return output;
	}
};
class funtion_title{
public:
	string scale;
	string return_value;
	string func_name;
	vector<parameter_func> parameters;
	vector<parameter_func> variable;
	funtion_title(){};
	funtion_title(vector<string> &input){
		scale = input.front();
		input.erase(input.begin());
		return_value = input.front();
		input.erase(input.begin());
		func_name = input.front();
		input.erase(input.begin());
		for (vector<string>::iterator it = input.begin() ; it != input.end();)
		{
			string a,b;
			a = *(it);
			b = *(it+1);
			parameter_func *tmp = new parameter_func(a,b);
			parameters.push_back(*tmp);
			it = it+2;
		}
	}

	friend ostream& operator << (ostream& output,funtion_title& a){
		output<<"("<<a.scale<<","<<a.return_value<<","<<a.func_name<<")"<<endl;
		for (vector<parameter_func>::iterator it = (a.parameters).begin() ; it != (a.parameters).end(); it++)
		{
			output<<*it;
		}
		return output;
	}
	string function_form(){
		string func_part = "";
		func_part = scale +" "+return_value+" "+func_name+"(";
		for (vector<parameter_func>::iterator it = (parameters).begin() ; it != (parameters).end(); it++)
		{
			func_part = func_part + (*it).type + " "+(*it).name+", ";
		}
		func_part.erase(func_part.end()-2);
		func_part = func_part +")";
		return func_part;
	}
	string use_form(vector<string> &true_parameters){
		string func_part = "";
		func_part = func_name+"(";
		for (vector<string>::iterator it = (true_parameters).begin() ; it != (true_parameters).end(); it++)
		{
			func_part = func_part +(*it)+",";
		}
		func_part.erase(func_part.end()-1);
		func_part = func_part +")";
		return func_part;
	}
	void add_parameter(string a, string b){
		parameter_func *tmp = new parameter_func(a,b);
		parameters.push_back(*tmp);
	}
};
int main(int argc,char* argv[]){
	// string input_name = argv[1];
	string compiledFile = "test.cu";
	ifstream compiledFile_in (compiledFile.c_str(), ifstream::in);
	string LineFromFile;
	
	vector<string> val_paras;
	int flag =-1;
	while(getline(compiledFile_in,LineFromFile)){
		if(LineFromFile.compare("//!@#val")==0){
			flag++;
			continue;
		}
		if(LineFromFile.compare("};")==0){
			flag=-1;
		}
		if(flag == 0){
			flag++;
			continue;
		}
		if(flag==1){
			split(LineFromFile," ;\t",val_paras);
		}
	}
	local_variable_struct localVar(val_paras);
	cout<<localVar;

	char c = getchar();
	// string headerName = "test.cu";
	string originKernel = "kernel.cu";
	string mainName = "test1.cu";
	// ifstream kernel_gpufile_in (input_name.c_str(), ifstream::in);
	// ofstream mainName_out (headerName.c_str(), ofstream::out);
	ifstream kernel_in (originKernel.c_str(), ifstream::in);
	ofstream mainName_out (mainName.c_str(), ofstream::out);
	string attach = "#include \"test.cu\"";
	mainName_out<<attach.c_str()<<endl;
	vector<funtion_title> function_group;
	while(getline(kernel_in,LineFromFile)){
		size_t posGlobal = LineFromFile.find(tagForGlobal);
		size_t posDevice = LineFromFile.find(tagForDevice);
		if (posGlobal!=string::npos && posGlobal==0){
			vector<string> store_info;
			split(LineFromFile, " (,)",store_info);
			funtion_title *tmp = new funtion_title(store_info);
			function_group.push_back(*tmp);
			(*tmp).add_parameter("vals*","vals");
			LineFromFile.clear();
			LineFromFile = (*tmp).function_form();
		}
		if (posDevice!=string::npos && posDevice==0){
			vector<string> store_info;
			split(LineFromFile, " (,)",store_info);
			funtion_title *tmp = new funtion_title(store_info);
			function_group.push_back(*tmp);
			(*tmp).add_parameter("vals*","vals");
			LineFromFile.clear();
			LineFromFile = (*tmp).function_form();
		}
		if(posGlobal==string::npos && posDevice==string::npos){
			for (vector<funtion_title>::iterator it = (function_group).begin() ; it != (function_group).end(); it++)
			{
				string functionCall = (*it).func_name;
				size_t pos_useFunc = LineFromFile.find(functionCall);
				if (pos_useFunc!=string::npos){
					//pos_useFunc = pos_useFunc+ functionCall.size();
					string formPart = LineFromFile.substr(0,pos_useFunc);
					vector<string> parameters;
					size_t pos_closeParenthesis = LineFromFile.find(")",pos_useFunc);
					size_t pos_NextopenParenthesis = LineFromFile.find("(",pos_useFunc+functionCall.size()+1);
					//cout<<pos_closeParenthesis<<"\t"<<pos_NextopenParenthesis<<endl;
					//cout<<formPart<<endl;
					//cout<<LineFromFile.substr(pos_useFunc,pos_closeParenthesis-pos_useFunc+1)<<endl;
					while(pos_NextopenParenthesis!=string::npos && pos_NextopenParenthesis<pos_closeParenthesis){
						pos_closeParenthesis = LineFromFile.find(")",pos_closeParenthesis+1);
						pos_NextopenParenthesis = LineFromFile.find("(",pos_NextopenParenthesis+1);
					}
					string rearPart= LineFromFile.substr(pos_closeParenthesis+1);
					pos_useFunc = pos_useFunc + functionCall.size();
					split(LineFromFile.substr(pos_useFunc+1,pos_closeParenthesis-pos_useFunc-1),",",parameters);
					parameters.push_back("vals");
					// for (vector<string>::iterator it = (parameters).begin() ; it != (parameters).end(); it++)
					// {
					// 	cout<<*it<<endl;
					// }
					LineFromFile.clear();
					LineFromFile = formPart + (*it).use_form(parameters) + rearPart;
					// cout<<formPart<<(*it).use_form(parameters)<<rearPart<<endl;
					//cin.get();
					//cout<<*it<<endl;
				}
				else
					continue;
			}
		}
		mainName_out<<LineFromFile<<endl; 
	}

	// for (vector<funtion_title>::iterator it = function_group.begin();it != function_group.end(); ++it)
	// {
	// 	(*it).add_parameter("Vals*","vals");
	// 	cout<<(*it).function_form()<<endl;
	// }
	kernel_in.close();
	mainName_out.close();
}
