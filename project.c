#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <dirent.h>
#include <stdlib.h>
#include <fcntl.h>

#define BUFFER_SIZE 8

void scriere(char *fisier,DIR *d,char *director)
{
  struct dirent *a;
  int log;
  log=open(fisier, O_RDWR | O_CREAT | O_TRUNC , S_IRUSR | S_IWUSR);
  while ((a= readdir(d))!=NULL)
    {
      struct stat fileStat;
      const char *fileType;
      switch (a->d_type) {
      case DT_REG:
	fileType = "-fisier obisnuit";
	break;
      case DT_DIR:
	fileType = "-director";
	break;
      default:
	fileType = "-alt tip de fisier";
	break;
      }
      char cale[200];
      strcpy(cale,director);
      strcat(cale,"/");
      strcat(cale,a->d_name);
      if (stat(cale, &fileStat) < 0) {
        printf("Eroare la obținerea informațiilor despre fișierul %s\n", a->d_name);
        
    }
      write(log,a->d_name ,strlen(a->d_name));
      write(log,fileType,strlen(fileType));
      char buffer[100]; 
      int bytes_written = snprintf(buffer, sizeof(buffer), ",%ld,%o", fileStat.st_size, fileStat.st_mode);
      write(log, buffer, bytes_written);
      
      write(log,"\n",1);
    }
  close(log);
}








void cautare(DIR *d,char* fisier ,char *director)
{
  struct dirent *a;
  
  while ((a= readdir(d))!=NULL)
    {
      const char *fileType;
      switch (a->d_type) {
      case DT_REG:
	fileType = "-fisier obisnuit";
	break;
      case DT_DIR:
	fileType = "-director";
	break;
      default:
	fileType = "-alt tip de fisier";
	break;
      }
     
      char buffer[BUFFER_SIZE];
      char ceva[1000];
      strcpy(ceva,"");
      int log;
       log=open(fisier, O_RDWR | O_CREAT , S_IRUSR | S_IWUSR);
      if (log == -1) {
	perror("Eroare la deschiderea fișierului");
	exit(EXIT_FAILURE);
      }
      ssize_t bytesRead=0;
    
      int n=0;
      while ((bytesRead = read(log, buffer, BUFFER_SIZE-1)) > 0) {
    	
	buffer[bytesRead]='\0';
	if (strchr(buffer,'\n')==NULL)
	  {
	    
	    strcat(ceva,buffer);
	    
	  }
	else{
	  
	  strcat(ceva,buffer);
	  char *token = strtok(ceva, "-,\n");
	  int i=0;

	  while (token != NULL) {
	    
	    if (strcmp(token, a->d_name) == 0) {
	      n=1;
		      
	      token = strtok(NULL, "-,\n");
	      
	      if (strcmp(fileType+1,token))
		{
		  printf("s-a schimbat tipul\n");
		}
	      char buf[100];
	      char cale[200];
	      struct stat fileStat;
	      strcpy(cale,director);
	      strcat(cale,"/");
	      strcat(cale,a->d_name);
	      if (stat(cale, &fileStat) < 0) {
		printf("Eroare la obținerea informațiilor despre fișierul %s\n", a->d_name);
		
	      }
	      token = strtok(NULL, "-,\n");
	      snprintf(buf, sizeof(buf), "%ld", fileStat.st_size);
	      if (strcmp(buf,token))
		{
		   printf("s-a schimbat dimensiunea\n");
		}
	      token = strtok(NULL, "-,\n");
	      snprintf(buf, sizeof(buf), "%o",  fileStat.st_mode);
	      if (strcmp(buf,token))
		{
		   printf("s-a schimbat modul\n");
		}
	    }
	    
	    
	   
	    if(i==3)
	      {
		break;
	      }
	    token = strtok(NULL, "-,\n");
	    i++;
	  }
	 
	    
	  strcpy(ceva,strchr(buffer,'\n'));
	  //printf("%s\n",strchr(buffer,'\n')+1);
	}
      }
        if(n==0)
	      printf("a fost adaugat fisierul %s\n",a->d_name);
	    
      //printf("asa\n");
      close(log);
      
    }
}


/* daca nu afiseaza nimic inseamna ca nu s-a produs nicio schimbare,
   programul inca nu afiseaza daca un fisier a fost sters*/



int main(int argc, char* argv[])
{
  
  char fisier[100];
  if ((argc<2)||(argc>10))
    {
      printf("usage error");
      exit(-1);
    }
  for(int i=1;i<argc;i++)
    {
      DIR *d;
 
      d=opendir(argv[i]);
      if(d==NULL)
	{
	  printf("nu s a putut deschide folderul");
	}
      strcpy(fisier,argv[i]); 
      strcat(fisier,"_log");
      
      
      cautare(d,fisier,argv[i]);
      
      
      closedir(d);
      d=opendir(argv[i]);
      
      
      scriere(fisier,d,argv[i]);
      
      
      closedir(d);
  
    }
  return 0;
}
