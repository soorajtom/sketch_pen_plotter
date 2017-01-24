#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<dos.h>
#include <Windows.h>

void sendout(char*);
int lineno =0;
int main()
{
	char fname[50],command[800]="MonoBitmap.0.9.0 ";
	printf("Enter the filename:");
	scanf("%s",fname);
	strcat(command,fname);
	strcat(command," --file-output");
	system(command);
	//system("echo off");
	char streamout[820]="echo \"", ch;
	int i = 0,hieght = 1, breadth = 0;
	FILE *fp;
	fp=fopen("generatedMonoBitmap.txt","a");
	fprintf(fp,";");
	fclose(fp);
	fp=fopen("generatedMonoBitmap.txt","r");
	if(fp==NULL){
	printf("filevide??");
	exit(254);}
	while((ch = getc(fp))!= '\n')
	{
		breadth++;
	}
	rewind(fp);
	while((ch = getc(fp))!= ';')
	{
		if(ch == '\n')
			hieght++;
	}

	rewind(fp);
	command[1]='\0';
	while((ch = getc(fp))!=EOF)
	{
		if(ch=='\n' || ch==';')
		{
			command[i]=':';
			command[i+1]= '\0';
			sendout(command);
			i=0;
			continue;
		}
		command[i++]=ch;
	}
	printf("\nReady to start printing.....\n");
	system("pause");
	strcpy(command, ";");
	sendout(command);
		
}

void sendout(char * command)
{
	char streamout[820];
	strcpy(streamout,"echo \"");
	strcat(streamout,command);
	strcat(streamout,"\" > COM3");
	//printf("\n%s\n",streamout);
	system(streamout);
	printf("\nline %d | %s",lineno ++,streamout);
	if(lineno % 50 == 0)
	Sleep(5000);
	//system("type COM6");
	//printf("\nbyte in\n");
	//system("pause");
}

