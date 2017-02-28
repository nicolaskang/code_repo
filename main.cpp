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
#define local_struct "vals"
#define local_struct_type "vals*"

const std::string structFileName  = "test.cu";
const std::string kernelFileName ="test1.cu";
const std::string mainFileName ="test2.cu";
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
		name = local_struct;
		type = local_struct_type;
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
	bool is_local_var(string name){
		for (vector<local_variable>::iterator it = (local_variable1).begin() ; it != (local_variable1).end(); it++)
		{
			std::vector<string> tmp_name;
			split(name,"-.",tmp_name);
			string vari_name = tmp_name.front();
			if(vari_name.compare((*it).para.name)==0){
				return true;
			}
		}
		return false;
	}
	string CreateStore(string name){
		string replacement =  "!@#$^&*(";
		if(!is_local_var(name))
			return "null";
		else{
			string return_tmp = this->name+replacement+"."+name+"="+name+";";
			return return_tmp;
		}
	}
	string CreateRestore(string name){
		string replacement =  "!@#$^&*(";
		if(!is_local_var(name))
			return "null";
		else{
			string return_tmp = name+"="+this->name+replacement+"."+name+";";
			return return_tmp;
		}

	}
};
class funtion_title{
public:
	string scale;
	string return_value;
	string func_name;
	vector<parameter_func> parameters;
	//vector<parameter_func> variable;
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
string gotoLine(ifstream &file, int numOfLine){
	int pos = file.tellg();
	file.seekg(0,ios::beg);
	string tmp;
	int lineRN=0;
	while(getline(file,tmp)){
		lineRN++;
		if(lineRN==numOfLine){
			break;
		}
	}
	file.seekg(pos,ios::beg);
	return tmp;
}
int main(int argc,char* argv[]){
	if(argc == 1){
		cerr<<"please input your gpu file"<<endl;
	}
	string input_name = argv[1];
	//cout<<input_name<<endl;
	string compiledFile = structFileName;
	ifstream input_in (input_name.c_str(),ifstream::in);
	ofstream header_out (compiledFile.c_str(),ofstream::out);
	string LineFromFile;
	while(getline(input_in,LineFromFile)){
		//cout<<"ok"<<endl;
		if(LineFromFile[0] == '#'){
			continue;
		}
		else{
			//cout<<"ok1"<<endl;
			std::vector<string> tmp;
			tmp.clear();
			split(LineFromFile," ",tmp);
			bool structureFlag = false;
			// for (vector<string>::iterator it = (tmp).begin() ; it != (tmp).end(); it++)
			// {
			// 	cout<<*it<<endl;
			// }
			string last_string = tmp[tmp.size()-1];
			//cout<<last_string<<endl;
			if(tmp.front() == structureTag || tmp.front() == TypedefTag){
				header_out<<LineFromFile<<endl;
				// string last_string = tmp[tmp.size()];
				// cout<<last_string<<endl;
				if(last_string[last_string.size()-1] == ';'){
					continue;
				}
				else if(last_string[last_string.size()-1] == '{'){
					//cout<<"ok2"<<endl;
					structureFlag = true;
					while(structureFlag == true){
						getline(input_in,LineFromFile);
						if(LineFromFile[0] == '#'){
							continue;
						}	
						header_out<<'\t'<<LineFromFile<<endl;
						int pos = LineFromFile.find("};");
						if(pos != string::npos){
							structureFlag = false;
							break;
						}
					}
				}
			}
		}
	}
	input_in.close();
	header_out.close();
	//finish all struct writing.

	input_in.open(input_name.c_str(),ifstream::in);
	header_out.open(compiledFile.c_str(),ofstream::app);
	LineFromFile.clear();
	header_out<<"//!@$^"<<endl;
	header_out<<"struct vals {"<<endl;
	vector<string> val_paras;
	while(getline(input_in,LineFromFile)){
		//cout<<LineFromFile<<endl;
		if(LineFromFile[0] == '#'){
			continue;
		}
		else{
			std::vector<string> tmp;
			tmp.clear();
			split(LineFromFile," ",tmp);
			if(tmp.size()<2 || tmp[0].compare("return")==0 || tmp[0].compare("if")==0){
				continue;
			}
			else{
				string str_tmp = tmp[1];
				int pos = str_tmp.find(tagForLocalVar);
				int pos1 = str_tmp.find('(');
				std::vector<string> split_underline;
				split(str_tmp,"_",split_underline);
				string var_name = split_underline[split_underline.size()-1];
				var_name.pop_back();
				if(pos != string::npos&&pos1==string::npos){
					header_out<<'\t'<<tmp[0]<<" "<<var_name<<';'<<endl;
					val_paras.push_back(tmp[0]);
					val_paras.push_back(var_name);
				}
			}
		}
	}
	header_out<<"};"<<endl;
	header_out<<"typedef struct "<<local_struct<<" "<<local_struct<<";"<<endl;
	local_variable_struct localVar(val_paras);
	//cout<<localVar<<endl;
	input_in.close();
	header_out.close();
	//char d = getchar();

	//vector<string> val_paras;
	
	//local_variable_struct stores all variables we should look for.
	// when these variables are being updated, these lcao

	//
	//first step: find equal mark in gpu file
	// the reason why i DON NOT find it in kernel file is because
	// we do not know whether it is an local variable operation.
	ifstream gpuFile_in (input_name.c_str(), ifstream::in);
	LineFromFile.clear();
	// LineFromFile =gotoLine(gpuFile_in,10,lineCounter);
	std::vector<int> PositionToInsertStore;
	std::vector<string> ContentForStore;
	int lineNum=0;
	while(getline(gpuFile_in,LineFromFile)){
		lineNum++;
		vector<string> tmp;
		split(LineFromFile," ",tmp);
		// is it with local variable tag or is it contained in our local variable struct?
		std::vector<string> tmp_name;
		split(tmp.front(),"()",tmp_name);
		string vari_name = tmp_name.front();
		if(vari_name.find(tagForLocalVar)==0|| localVar.is_local_var(vari_name) == true){
			//cout<<vari_name<<endl;
			//cout<<"this is "<<lineNum<<endl;
			string numSignLine = gotoLine(gpuFile_in,lineNum-1);
			std::vector<string> tmpForSplit;
			split(numSignLine," ",tmpForSplit);
			tmpForSplit.erase(tmpForSplit.begin());
			//cout<<tmpForSplit.front()<<endl;
			//string WaitForConvert = tmpForSplit.front();
			std::vector<string> split_underline;
			split(vari_name,"_",split_underline);
			string var_name = split_underline[split_underline.size()-1];

			int numConverted = stoi(tmpForSplit.front());
			string insertForstore = localVar.CreateStore(var_name);
			//cout<<"variable "<<var_name<<endl;
			//cout<<"number to insert "<<numConverted<<endl;
			//cout<<"what to insert "<<insertForstore<<endl;
			PositionToInsertStore.push_back(numConverted);
			ContentForStore.push_back(insertForstore);
			//char c = getchar();
		}
		else
			continue;
	}


	//char c = getchar();
	// string headerName = "test.cu";
	string originKernel = argv[2];
	string mainName = kernelFileName;
	int lineCounter=0;
	ifstream kernel_in (originKernel.c_str(), ifstream::in);
	ofstream mainName_out (mainName.c_str(), ofstream::out);
	string attach = "#include \"" + structFileName +'\"';lineCounter++;
	mainName_out<<attach.c_str()<<endl;
	vector<funtion_title> function_group;
	int LineCounterForinsertion = 0;
	bool flagForWriting = false;
	bool flagForPostfuncWriting = false;
	string insertForPostfuncWriting;
	while(getline(kernel_in,LineFromFile)){
		// cout<<LineFromFile<<endl;
		if(flagForWriting){
			mainName_out<<'\t'<<ContentForStore.front()<<endl;
			PositionToInsertStore.erase(PositionToInsertStore.begin());
			ContentForStore.erase(ContentForStore.begin());
			flagForWriting = false;
		}
		if(flagForPostfuncWriting){
			mainName_out<<'\t'<<insertForPostfuncWriting<<endl;
			flagForPostfuncWriting = false;
		}
		LineCounterForinsertion++;

		size_t posGlobal = LineFromFile.find(tagForGlobal);
		size_t posDevice = LineFromFile.find(tagForDevice);
		if (posGlobal!=string::npos && posGlobal==0){
			vector<string> store_info;
			split(LineFromFile, " (,)",store_info);
			funtion_title *tmp = new funtion_title(store_info);
			function_group.push_back(*tmp);
			(*tmp).add_parameter(local_struct_type,local_struct);
			LineFromFile.clear();
			LineFromFile = (*tmp).function_form();
		}
		if (posDevice!=string::npos && posDevice==0){
			vector<string> store_info;
			split(LineFromFile, " (,)",store_info);
			funtion_title *tmp = new funtion_title(store_info);
			function_group.push_back(*tmp);
			(*tmp).add_parameter(local_struct_type,local_struct);
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
					// if these parameters is called by reference or pointer, it may be likely the parameters would be changed inside the function.
					// so we should store these variables after function finishes.
					//	first, we should know whether the variable is called by reference or pointer, 
					// the best way is to check the type.
					// if the type contains of asterisk or AND sign, then the variable is called by r or p.
					for(std::vector<string>::iterator it1 = parameters.begin();it1!=parameters.end();it1++){
						string var_name = *it1;
						vector<parameter_func> tmp =(*it).parameters;
						parameter_func tmp_forfindAstnAnd = tmp.front();
						if(!((tmp_forfindAstnAnd.type).find('*')==string::npos && (tmp_forfindAstnAnd.type).find('&')==string::npos) && localVar.is_local_var(var_name)==true){
							insertForPostfuncWriting = localVar.CreateStore(var_name);
							flagForPostfuncWriting = true;
						}
					}
					parameters.push_back(local_struct);
					LineFromFile.clear();
					LineFromFile = formPart + (*it).use_form(parameters) + rearPart;
				}
				else
					continue;
			}
		}
		if(LineCounterForinsertion == PositionToInsertStore.front()){
			//cout<<"change before "<<PositionToInsertStore.front()<<endl;
			if(LineFromFile[LineFromFile.size()-1]==';' || LineFromFile == ""){
				//cout<<"do not need any modification"<<endl;
				//cout<<"the line is "<<LineFromFile<<endl;
				//cout<<"the last is "<<LineFromFile[LineFromFile.size()-1]<<endl;
				flagForWriting = true;
			}
			else{
				PositionToInsertStore[0]++;
				//cout<<"change after "<<PositionToInsertStore.front()<<endl;
				//cout<<"the line is "<<LineFromFile<<endl;
				//cout<<"the last is "<<LineFromFile[LineFromFile.size()-1]<<endl;
			}
			//char c =getchar();
		}
		mainName_out<<LineFromFile<<endl;
		LineFromFile.clear();
	}
	//char a= getchar();
	kernel_in.close();
	mainName_out.close();
	PositionToInsertStore.clear();
	ContentForStore.clear();
	

	string mainFileWhichCallKernel = argv[3];
	string NewMainFile = mainFileName;
	ifstream mainFileCallKernel_in (mainFileWhichCallKernel.c_str(),ios::in);
	
	LineFromFile.clear();
	lineCounter = 0;
	std::vector<int> numLineTocreateVals;
	std::vector<string> InstructionCreateVals;
	std::vector<string> callkernel;
	int numOfValtoDefine = 0;
	while(getline(mainFileCallKernel_in,LineFromFile)){
		//cout<<LineFromFile<<endl;
		lineCounter++;
		if(LineFromFile.find("<<<")!=string::npos && LineFromFile.find(">>>")!=string::npos){
			std::vector<string> tmpForSplit;
			split(LineFromFile," \t\n<>,();",tmpForSplit);
			// for (vector<string>::iterator it = (tmpForSplit).begin(); it != (tmpForSplit).end(); it++)
			// {
			// 	cout<<*it<<endl;
			// }
			// char c = getchar();
			string name = tmpForSplit.front();
			tmpForSplit.erase(tmpForSplit.begin());
			string grid = tmpForSplit.front();
			tmpForSplit.erase(tmpForSplit.begin());
			string thread = tmpForSplit.front();
			tmpForSplit.erase(tmpForSplit.begin());
			//create vals
			std::string waittoPush;
			string vals_name_tocreate = local_struct + std::to_string(numOfValtoDefine);
			numLineTocreateVals.push_back(lineCounter - 1);
			waittoPush = std::string(local_struct_type) + " " + vals_name_tocreate+";";
			InstructionCreateVals.push_back(waittoPush);
			waittoPush.clear();
			numOfValtoDefine++;
			//malloc memory
			numLineTocreateVals.push_back(lineCounter - 1);
			waittoPush = "cudaMalloc((void**)&"+vals_name_tocreate+
				", "+grid+".x"+"*"+grid+".y"+"*"+grid+".z"+"*"+thread+".x"+"*"+thread+".y"+"*"+thread+".z"+"*"+"sizeof("+
				local_struct+"));";
			InstructionCreateVals.push_back(waittoPush);
			waittoPush.clear();

			//numLineTocreateVals.push_back(lineCounter);
			string callkernel_wait_push = name + " <<<"+grid+" ,"+thread+" >>>"+" (";
			for(vector<string>::iterator it = tmpForSplit.begin();it!=tmpForSplit.end();it++){
				callkernel_wait_push = callkernel_wait_push + *it + ',';
			}
			//callkernel.pop_back();
			callkernel_wait_push = callkernel_wait_push +vals_name_tocreate;
			callkernel_wait_push = callkernel_wait_push+");";
			callkernel.push_back(callkernel_wait_push);
			//InstructionCreateVals.push_back(callkernel);

			numLineTocreateVals.push_back(lineCounter+1);
			waittoPush = "cudaFree("+vals_name_tocreate+");";
			InstructionCreateVals.push_back(waittoPush);
			waittoPush.clear();
		}
		//NewMainFile_out<<LineFromFile<<endl;
		//gotoLine(mainFileCallKernel_in,lineCounter);
	}
	mainFileCallKernel_in.close();
	//NewMainFile_out.close();
	// for(vector<string>::iterator it = InstructionCreateVals.begin();it!=InstructionCreateVals.end();it++){
	// 	cout<<*it<<endl;
	// }
	// for(vector<int>::iterator it = numLineTocreateVals.begin();it!=numLineTocreateVals.end();it++){
	// 	cout<<*it<<endl;
	// }

	//char dad = getchar();
	mainFileCallKernel_in.open(mainFileWhichCallKernel.c_str(),ios::in);
	ofstream NewMainFile_out (NewMainFile.c_str(),ios::out);
	// NewMainFile_out = open(NewMainFile.c_str(),ios::out);
	NewMainFile_out<<"#include \""<<structFileName<<"\";"<<endl;
	flagForWriting = false;
	lineCounter=0;
	LineFromFile.clear();
	while(getline(mainFileCallKernel_in,LineFromFile)){
		lineCounter++;	

		while(lineCounter == numLineTocreateVals.front() && numLineTocreateVals.size()!=0){
			//cout<<"change before "<<PositionToInsertStore.front()<<endl;
			if(LineFromFile[LineFromFile.size()-1]==';' || LineFromFile == ""){
				//cout<<"do not need any modification"<<endl;
				//cout<<"the line is "<<LineFromFile<<endl;
				//cout<<"the last is "<<LineFromFile[LineFromFile.size()-1]<<endl;
				flagForWriting = true;
			}
			else{
				for(int i=0;i<numLineTocreateVals.size();i++)
					numLineTocreateVals[i]++;
				//cout<<"change after "<<PositionToInsertStore.front()<<endl;
				//cout<<"the line is "<<LineFromFile<<endl;
				//cout<<"the last is "<<LineFromFile[LineFromFile.size()-1]<<endl;
			}
			if(flagForWriting == true){
				NewMainFile_out<<'\t'<<InstructionCreateVals.front()<<endl;
				numLineTocreateVals.erase(numLineTocreateVals.begin());
				InstructionCreateVals.erase(InstructionCreateVals.begin());
				flagForWriting = false;
			}
			//char c =getchar();
		}
		if(LineFromFile.find("<<<")!=string::npos){
			NewMainFile_out<<'\t'<<callkernel.front()<<endl;
			callkernel.erase(callkernel.begin());
		}else{
			NewMainFile_out<<LineFromFile<<endl;
		}
	}
	InstructionCreateVals.clear();
	numLineTocreateVals.clear();
	NewMainFile_out.close();
	mainFileCallKernel_in.close();
	return 1;
}
