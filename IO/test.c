#include<stdio.h>
#include<string.h>

int main()
{
    FILE* fp;
    char readBuff[50] ={0};
    char writeStr[] = "Linux so diffcult";

    fp = fopen("bite" ,"w+");
    if(fp==NULL)
    {
        printf("File open fail\n");
        return 1;
    }

    fwrite(writeStr,strlen(writeStr),1,fp);

    fseek(fp , 0, SEEK_SET);

    fread(readBuff,strlen(writeStr),1,fp);
    printf("%s",readBuff);

    fclose(fp);

    return 0;
}
