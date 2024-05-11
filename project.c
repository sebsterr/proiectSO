#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <dirent.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/wait.h>

#define BUFFER_SIZE 8

void dirstart();

void alocare(char*** array,int nr) {

  *array = (char**)realloc(*array, nr*10*sizeof(char*));
    if (*array == NULL) {
        perror("Eroare la realocarea memoriei pentru array");
        exit(EXIT_FAILURE);
    }
}

void adaugaSir(char*** array, const char* sir,int pozitie) {
  if(pozitie%10==0){
    alocare(array,(pozitie/10)+1);

  }
  (*array)[pozitie]=strdup(sir);
  if ((*array)[pozitie] == NULL) {
        perror("Eroare la alocarea memoriei pentru șir");
        exit(EXIT_FAILURE);
    }
  strcpy((*array)[pozitie],sir);
}

void sters(int a,int b,char **arr1,char **arr2)
{
  for(int i=0;i<a;i++)
    {
      // printf("%s\n",arr1[i]);
      int ver=0;
      for(int j=0;j<b;j++)
	{
	  if(strcmp(arr1[i],arr2[j])==0)
	    ver=1;

	}
      if(ver==0)
	printf("a fost sters %s\n",arr1[i]);
    }
  for(int i=0;i<a;i++)
    free(arr1[i]);
  for(int j=0;j<b;j++)
    free(arr2[j]);
  free(arr1);
  free(arr2);

}

void scriere(char *fisier,DIR *d,char *director,char *iesire,int stare)
{
  struct dirent *a;
  int log;

  log=open(fisier, O_RDWR | O_CREAT | O_TRUNC , S_IRUSR | S_IWUSR);
  if (log == -1) {
    perror("Eroare la deschiderea fișierului");
    exit(EXIT_FAILURE);

  }
  while ((a= readdir(d))!=NULL)
    {
      if(strcmp(a->d_name,".")==0)
	{
	  continue;
	}
      if(strcmp(a->d_name,"..")==0)
	{
	  continue;
	}
      if(strstr(a->d_name,"snapshot")!=NULL)
	{
	  continue;
	}

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
      char buffer[140];
      int bytes_written = snprintf(buffer, sizeof(buffer), ",%ld,%o,%ld", fileStat.st_size, fileStat.st_mode,fileStat.st_ino);
      write(log, buffer, bytes_written);
      write(log,"\n",1);
    }

  if (stare==0)
    {
      char script[500];
      strcpy(script,"./copiere.sh ");
      strcat(script, fisier);
      strcat(script," ");
      if(strrchr(fisier,'/')!=NULL)
	{

	  strcat(script,1+strrchr(fisier,'/'));
	}
      else
	{

	   strcat(script,fisier);
	}
      strcat(script," ");
      strcat(script,iesire);

      //printf("%s\n",script);
      system(script);
    }
  close(log);
}








void cautare(DIR *d,char* fisier ,char *director,char *folderRau,char *iesire,int stare)
{
  int l=0;
  struct dirent *a;
  char cale[200];
  char** arr1=NULL;
  char** arr2=NULL;
  int poz1=0,poz2=0;
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

      int n=0;
      strcpy(cale,director);
      strcat(cale,"/");
      strcat(cale,a->d_name);
      if(strcmp(fileType,"-director")==0)
	{
	  if(strcmp(a->d_name,".")==0)
	    {

	      n=1;
	      continue;

	    }
	  if(strcmp(a->d_name,"..")==0)
	    {

	      n=1;
	      continue;
	    }
	  dirstart(cale,folderRau,iesire,stare);
	}
      if(strstr(a->d_name,"snapshot")!=NULL)
	{
	  n=1;

	  continue;

	}
      adaugaSir(&arr2,a->d_name,poz2);
      poz2++;

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
      int stop=0;
      while ((bytesRead = read(log, buffer, BUFFER_SIZE-1)) > 0) {
	buffer[bytesRead]='\0';
	stop=0;
	if (strchr(buffer,'\n')==NULL)
	  {

	    strcat(ceva,buffer);

	  }
	else{


	  strcat(ceva,buffer);
	  char *token = strtok(ceva, "-,\n");
	  int i=0;
	  int k=0;
	  int altv=0;
	  while (token != NULL) {

	    if ((l==0)&&(k==0))
	      {

		adaugaSir(&arr1,token,poz1);
		poz1++;
	      }

	    k++;
	    if (strcmp(token, a->d_name) == 0) {
	      n=1;
	      stop=1;
	      token = strtok(NULL, "-,\n");
	      if (strcmp(fileType+1,token))
		{
		  printf("s-a schimbat tipul lui %s\n",a->d_name);

		}
	      char buf[100];
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
		  printf("s-a schimbat dimensiunea lui %s\n",cale);
		}
	      token = strtok(NULL, "-,\n");
	      snprintf(buf, sizeof(buf), "%o",  fileStat.st_mode);
	      if(strcmp(fileType+1,"director")==0)
		altv=1;
	      if ((strcmp(buf,"100000")!=0)&&(altv==0))
		{

		  int pipefd[2];
		  if (pipe(pipefd) == -1) {
		    perror("pipe");
		    exit(EXIT_FAILURE);
		  }
		  pid_t pid = fork();
		  if (pid == -1) {
		    perror("fork");
		    exit(EXIT_FAILURE);
		  }

		  if (pid == 0) {
		    close(pipefd[0]);

		    dup2(pipefd[1], STDOUT_FILENO);
		    close(pipefd[1]);
		    execlp("./verify_for_malicious.sh" , "./verify_for_malicious.sh", cale, NULL);
		    perror("execlp");
		    exit(EXIT_FAILURE);
		  } else {

		    close(pipefd[1]);
		    char line[200];
		    ssize_t bytes_read=read(pipefd[0],line,199);
		    line[bytes_read]='\0';
		    if (bytes_read == -1) {
		      perror("read");
		      exit(EXIT_FAILURE);
		    }
		    if(strcmp("SAFE\n",line)!=0){
		      printf("%s",line);
		      char comanda[400];
		      strcpy(comanda,"mv");
		      strcat(comanda," ");
		      strcat(comanda,cale);
		      strcat(comanda," ");
		      strcat(comanda,folderRau);
		      system(comanda);

		    }
		  }
		   int status;
		  waitpid(pid, &status, 0);
		  if (WIFEXITED(status)) {

		  } else {
		    printf("Procesul copil cu PID-ul %d s-a încheiat anormal\n", pid);
		  }

		}

	      if (strcmp(buf,token))
		{
		  printf("s-a schimbat modul lui %s\n",cale);
		}
	      token = strtok(NULL, "-,\n");

	      snprintf(buf, sizeof(buf), "%ld",  fileStat.st_ino);
	      if (strcmp(buf,token))
		{
		  printf("s-a schimbat indode number-ul lui %s\n",cale);
		}

	    }


	    if(i==4)
	      {
		break;
	      }
	    token = strtok(NULL, "-,\n");
	    i++;
	  }
	  if((stop==1)&&(l!=0))
	    {
	      break;
	    }


	  strcpy(ceva,strchr(buffer,'\n'));


	}
      }
      l++;
      if(n==0)
	{
	  printf("a fost adaugat un %s: %s\n",fileType+1,cale);

	}


      close(log);
    }

  sters(poz1,poz2,arr1,arr2);

}


void dirstart(char* cv,char *folderRau,char* iesire, int stare)
{
  char fisier[100];
  DIR *d;
  d=opendir(cv);
  if(d==NULL)
    {
      printf("nu s a putut deschide folderul\n");
      exit(1);
    }
  strcpy(fisier,cv);
  strcat(fisier,"_snapshot");
  cautare(d,fisier,cv,folderRau,iesire,stare);
  closedir(d);
  d=opendir(cv);
  if(d==NULL)
    {
      printf("nu s a putut deschide folderul\n");
      exit(1);
    }
  scriere(fisier,d,cv,iesire, stare);
  closedir(d);

}




int main(int argc, char* argv[])
{


  pid_t pid;
  int status;
  int stare=0;
  if ((strcmp(argv[1],"-o")==0))
    {
      if ((argc<6)||(strcmp(argv[3],"-s")!=0)||(argc>15))
	{
	  printf("usage error1");
	  exit(-1);
	}

      for(int i=5;i<argc;i++)
	{
	  pid = fork();
	  if (pid == -1) {
	    perror("fork");
	    exit(EXIT_FAILURE);
	  } else if (pid == 0) {
	    dirstart(argv[i],argv[4],argv[2],stare);
	    printf("snapshot for %s was created\n",argv[i]);
	    exit(i + 1);
	  }
	}
      for (int i = 5; i < argc; ++i) {
	pid = wait(&status);
	if (WIFEXITED(status)) {
	  printf("Child PID=%d exited with status=%d\n", pid, WEXITSTATUS(status));
	} else {
	  printf("Child PID=%d exited abnormally\n", pid);
	}
      }
    }
  else if((strcmp(argv[1],"-s")==0)){
    stare=1;
    if ((argc<4)||(argc>13))
      {
	printf("usage error2");
	exit(-1);
      }

    for(int i=3;i<argc;i++)
      {
	pid = fork();
	if (pid == -1) {
	  perror("fork");
	  exit(EXIT_FAILURE);
	} else if (pid == 0) {
	  dirstart(argv[i],argv[2],argv[2],stare);
	  printf("snapshot for %s was created\n",argv[i]);
	  exit(i + 1);
	}
      }
    for (int i = 3; i < argc; ++i) {
      pid = wait(&status);
      if (WIFEXITED(status)) {
	printf("Child PID=%d exited with status=%d\n", pid, WEXITSTATUS(status));
      } else {
	printf("Child PID=%d exited abnormally\n", pid);
      }
    }
  }
  else
    {
      printf("usage error");
      exit(-1);
    }
  return 0;
  }
