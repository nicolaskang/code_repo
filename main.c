#include <stdio.h>
#include <assert.h>
#include <string.h>
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
              p3++;p2++;
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
}
int main(int argc,char* argv[]){
	//printf("%s\n",argv[1]);
	FILE* f = fopen(argv[1],"r");
	char buffer[lengthBuffer];
	FILE* f1 = fopen("test.cu","w");
	if(f == NULL)
	{
		perror("error in opening");
	}
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
			// I think there is more universal format for extraction of local, global, shared varibles.
			//
			printf("%s\n",buffer);
			while(buffer[0]==' '){
				*buffer = *(buffer+1);
			}
			sscanf(buffer,"%[^ (]",headTag);
			//printf("this is headtag %s\n",headTag);
			int flagStruct = 0;
			if(flagStruct==0&&(strcmp(headTag,structureTag)==0||strcmp(headTag,TypedefTag)==0)){
				//printf("this buffer contains of structure tag or typedef %s\n",buffer);	
				//printf("the tag is %s\n",headTag);
				printf("%s\n",buffer);
				printf("first\n");
				if(FindTag(buffer,"{")==1){
					flagStruct = 1;
				}
				fputs(buffer,f1);	
				while(flagStruct==1){
					fgets(buffer,lengthBuffer,f);
					//printf("buffer now is %s\n",buffer);
					if(buffer[0]=='#'){
						//printf("pass #\n");
					}
					else{
						fputs(buffer,f1);
						if(FindTag(buffer,"};")==1){
							flagStruct=0;
							break;
						}
					}
				}					
			}
		}

	}
	
	if(strcmp(headTag,intTag)==0 || strcmp(headTag,floatTag)==0){
            //compare tag and intTag or floatTag, if true, then LocalVaribles are found.
            printf("this buffer contains of float or int %s\n",buffer);
            sscanf(buffer,"%*[^ ] %s %[^;]",name);
            printf("name is %s\n",name);
            if(FindTag(buffer,tagForLocalVar)==1){
            	 printf("this is %s\n and %s\n",headTag,name)                                                                                                                                                                                    fputs(buffer,f1);
                                //                                                                                                                                                                                                                                                 }
                                //                                                                                                                                                                                                                                                                         }
	fclose(f);
	fclose(f1);
	return 0;
	printf("%s\n",local);
}
