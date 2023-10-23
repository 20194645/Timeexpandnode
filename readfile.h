#include<stdio.h>
#include <stdlib.h>
#include<string.h>
int getBestTime(char *line){
 int i;
 int count = 0;
 int k;
 for(i=0;i<strlen(line);i++)
 {
    if(line[i]==' ')
    {
     count++;
     if (count == 6)
     {
      k=i;
     }
    }
    
 }
 int j = 0;
 char besttime[10];
 while (k < strlen(line))
 {
  besttime[j] = line[k];
  k++;
  j++;
  if(line[k]==' ')
  {
   besttime[j]='\0';
   break;
  }
 }
 int a = atoi(besttime);
 return a;
}
int getAmplitude(char *line){
 int i;
 int count = 0;
 int k;
 for(i=0;i<strlen(line);i++)
 {
    if(line[i]==' ')
    {
     count++;
     if (count == 7)
     {
      k=i;
     }
    }
 }
 int j = 0;
 char Amplitude[10];
 while (k < strlen(line))
 {
  Amplitude[j] = line[k];
  k++;
  j++;
  if(line[k]=='\0')
  {
   Amplitude[j]='\0';
   break;
  }
 }
 int a = atoi(Amplitude);
 return a;
}
void getName(char *line,char *name)
{
 int i;
 int count = 0;
 int k;
 for(i=0;i<strlen(line);i++)
 {
    if(line[i]==' ')
    {
     count++;
     if (count == 1)
     {
      k=i+1;
     }
    }
 }
 int j = 0;
 while (k < strlen(line))
 {
  name[j] = line[k];
  k++;
  j++;
  if(line[k]==' ')
  {
   name[j]='\0';
   break;
  }
 }
}
int  getPeriod(char *line)
{
 int i;
 int count = 0;
 int k;
 for(i=0;i<strlen(line);i++)
 {
    if(line[i]==' ')
    {
     count++;
     if (count == 5)
     {
      k=i;
     }
    }
    
 }
 int j = 0;
 char period[10];
 while (k < strlen(line))
 {
  period[j] = line[k];
  k++;
  j++;
  if(line[k]==' ')
  {
   period[j]='\0';
   break;
  }
 }
 int a = atoi(period);
 return a;
}
