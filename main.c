#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <copyfile.h>
#define local "__local__"
#define lengthBuffer 200
#define tagForLocalVar "__cuda_local_var_"
#define tagForGlobal "__global__"
#define tagForSharedMem "__shared__"
#define intTag "int"
#define floatTag "float"
#define TypedefTag "typedef"
#define structureTag "struct"
int FindTag(char* buffer,char* tag){
	char *p1, *p2, *p3;
	int i=0,j=0,flag=0;
	p1 = buffer;
	p2 = tag;
	for(i = 0; i<strlen(buffer); i++)
	{
	if(*p1 == *p2)
	{
		p3 = p1;
		for(j = 0;j<strlen(tag);j++)
		{
			if(*p3 == *p2)
			{
				p3++;
				p2++;
			} 
			else
				break;
			}
			p2 = tag;
			if(j == strlen(tag))
			{
				flag = 1;
				return 1;
			}
		}
		p1++; 
	}
	if(flag==0)
	{
		return 0;
	}
	return 0;
}
int FindTagPos(char* buffer,char* tag){
	char *p1, *p2, *p3;
	int i=0,j=0,flag=0;
	p1 = buffer;
	p2 = tag;
	for(i = 0; i<strlen(buffer); i++)
	{
	if(*p1 == *p2)
	{
		p3 = p1;
		for(j = 0;j<strlen(tag);j++)
		{
			if(*p3 == *p2)
			{
				p3++;
				p2++;
			} 
			else
				break;
			}
			p2 = tag;
			if(j == strlen(tag))
			{
				flag = 1;
				return i;
			}
		}
		p1++; 
	}
	if(flag==0)
	{
		return -1;
	}
	return -1;
}
int main(int argc,char* argv[]){
	//printf("%s\n",argv[1]);
	FILE* f = fopen(argv[1],"r");
	char buffer[lengthBuffer];
	char *headerName = "test.cu";
	char *mainName = "test1.cu";
	char *originKernel = "kernel.cu";
	FILE* f1 = fopen(headerName,"w");
	assert(f!=NULL);
	assert(f1!=NULL);
	while(!feof(f))
	{
		fgets(buffer,lengthBuffer,f);
		if(buffer[0]=='#'){
			//# can help trace back to the place where fault and error is.
			//it may be used later
			continue;
		}
		else{
			char headTag[15],name[50];
			memcpy(headTag,"",sizeof(headTag));
			memcpy(name,"",sizeof(name));
			while(buffer[0]==' '){
				*buffer = *(buffer+1);
			}
			sscanf(buffer,"%[^ (]",headTag);
			//printf("this is headtag %s\n",headTag);
			int flagStruct = 0;
			if(flagStruct==0&&(strcmp(headTag,structureTag)==0||strcmp(headTag,TypedefTag)==0)){
				//printf("this buffer contains of structure tag or typedef %s\n",buffer);	
				//printf("the tag is %s\n",headTag);
				//printf("%s\n",buffer);
				//printf("first\n");
				if(FindTag(buffer,"{")!=0){
					flagStruct = 1;
				}
				fputs(buffer,f1);
				//fputs("//@#$pre\n",f1);
				while(flagStruct==1){
					fgets(buffer,lengthBuffer,f);
					//printf("buffer now is %s\n",buffer);
					if(buffer[0]=='#'){
						//printf("pass #\n");
					}
					else{
						fputs("\t",f1);
						fputs(buffer,f1);
						if(FindTag(buffer,"};")!=0){
							flagStruct=0;
							break;
						}
					}
				}
			}
		}
	}
	//fclose(f);
	//fclose(f1);
	// create struct
	char start_creating[] = "struct vals {\n";
	char end_creating[] = "};\n";
	char markForVal[] ="//!@#val\n";
	fseek(f,0,SEEK_SET);
	fputs(markForVal,f1);
	fputs(start_creating,f1);
	while(!feof(f)){
		fgets(buffer,lengthBuffer,f);
		//printf("buffer is %s\n",buffer);
		if(buffer[0]=='#'){
			continue;
		}
		else{
			//printf("still good\n" );
			char headTag1[15],name1[50];
			memcpy(headTag1,"",sizeof(headTag1));
			memcpy(name1,"",sizeof(name1));
			int space=0,indexforspace=0;
			while(buffer[indexforspace] == ' '){
				space++;
				indexforspace++;
			}
			if(buffer[0]=='('||buffer[0]=='{'||buffer[0]=='}'||buffer[0]==')')
				continue;
			sscanf(buffer+space,"%[^ ] %[^ ]",headTag1,name1); 
			//printf("headtag1 is %s and name is %s\n",headTag1,name1);
			//printf("test %d\n",FindTag(	"__syncthreads();",tagForLocalVar));
			
			if(FindTag(name1,tagForLocalVar)!=0 && FindTag(buffer+space,"return")==0){
				//printf("%s %s\n",headTag1,name1);
				//printf("full!!!! %s\n",buffer+space );
					//compare tag and intTag or floatTag, if true, then LocalVaribles are found.
					//printf("this buffer contains of float or int %s\n",buffer);
				if(name1[0]=='('||name1[0]=='{'||name1[0]=='}'||name1[0]==')')
					continue;
				int pos = FindTagPos(name1,"_non_const_");
				if(pos ==-1)
					continue;
				pos = pos + strlen("_non_const_");
				int pos_space = FindTagPos(buffer+space," ");
				if(pos_space ==-1)
					continue;
				pos_space = pos_space + strlen(" ");
				char *type = malloc(pos_space*sizeof(char));
				memcpy(type,"",pos_space*sizeof(char));
				memcpy(type,buffer+space,pos_space*sizeof(char));
				//printf("%s\n",type);
				char *name_val = malloc((strlen(name1)-pos-1)*sizeof(char));
				memcpy(name_val,"",(strlen(name1)-pos-1)*sizeof(char));
				memcpy(name_val,name1+pos,(strlen(name1)-pos-1)*sizeof(char));
				//printf("%s\n",name_val);
				//printf("pos is %d\n",pos );
				//printf("string is %s\n", name1+pos);
				// strcpy(content,name1+pos);
				// printf("name is %s, content is %s\n",name1+pos,content);
				// printf("print\n");
				// printf("%s\n", buffer+space);
				// printf("%shahha%s\n",type,name_val);
				fputs("\t",f1);
				fputs(type,f1);
				if(type[strlen(type)-1]!=';'){
					//fputs(" ",f1);
					fputs(name_val,f1);
				}
				fputs("\n",f1);
				//free(type);
				//free(name_val);
			}
		}    
	}
	fputs(end_creating,f1);
	char *typedefForVal = "typedef struct vals vals;";
	fputs(typedefForVal,f1);
	fclose(f);
	fclose(f1);

	f1 = fopen(originKernel,"r");
	FILE *f2 = fopen(mainName,"w");
	void *buf;
	char *attach = "#include \"test.cu\"";
	fputs(attach,f2);
	fputs("\n",f2);
	while(!feof(f1)){
		fread(&buf,1,1,f1);
		fwrite(&buf,1,1,f2);
		if((char)buf == ','|| (char)buf == '('){
			fputs("\n",f2);
			fputs("\t",f2);
		}
	}
	fclose(f1);
	fclose(f2);
	return 0;
}
