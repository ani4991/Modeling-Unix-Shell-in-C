#include <unistd.h>     // getpid(), getcwd()
#include <sys/types.h>  // type definitions, e.g., pid_t
#include <sys/wait.h>   // wait()
#include <stdbool.h>
#include<stdio.h>
#include <string.h>
#include<stdlib.h>
#include<fcntl.h>
#define MAX_LINE_LENGTH 1024


char *find_path(char * argv)               // finding the right executable
{  
   
   char * buffer = getenv("PATH");
   char * directory = strtok(buffer, ":");
   int pathsize=0;
   char *pathname=(char *)malloc(20);
   
   while(directory != NULL)
    { 
      pathsize = strlen(directory) + strlen(argv);
      pathsize = pathsize + 1;	  
	  strcpy(pathname, directory);
      strcat(pathname,"/");
      strcat(pathname,argv);
	  pathname[pathsize] = '\0';
	  if (access(pathname, X_OK) == 0)
		  break;

      directory = strtok(NULL, ":"); 
       
    }
    return pathname;
       
}


pid_t pid;
pid_t child_pids[1000];
int child_no=0;
char **bg_cmd = NULL; 
int status;
  
  
 
int main()
{ 
   bg_cmd = (char **)malloc(256);
   
  
  while(1)                                  // while(1) program start point
  {	  
	  printf("\nMyShell:/$ ");
	  char user_command[1024];
	  int i=0,r=0;
	  char c;
	  int interactive=1;
	  char **parsed = NULL;
	  char **cmd = NULL;      
	  int strings_num = 100;
	  char **bg_args = NULL;
	  int nbn =1;
	  int ctr =0;
	  int h=0;
	  int y;
	  char b[100];

	  while((c=getchar()) != '\n')            //getchar accepting user commands
		{
			user_command[i]=c;
			i=i+1;
		}
		//printf("%d",i);
		user_command[i] = '\0';
		
		
	   char* p = strtok(user_command, " ");
	   char* tmp = p;
	   int j=0; int pos =0;
	   cmd = (char **)malloc(1024);
	  
	  
	  while (tmp != NULL)                  // tokenizing user commands into array of strings(cmd[0] ,cmd[1]....)
	   {
		   cmd[j] = tmp; 
			j=j+1;
			tmp = strtok(NULL, " ");
	   }
	   
	   int u=0,q=0;
	   bg_args = (char **)malloc(256);
	   
	   while(u<j)                        // generating bg args .....string array without "bg"
		  {   	
			  if(strcmp(cmd[u],"bg")!=0)
			    {   
				  bg_args[q] = cmd[u];
				  q++; 
			    }
			 u++;
		  }	
		  
	   if(strcmp(cmd[0], "cd")==0)               // implementing the "cd" command
		 {   
			if(strcmp(cmd[1],"/")!=0)
			  {   
				char *cwd = getcwd(b,sizeof(b));
				char *d = strcat(cwd,"/");
				char *dest = strcat(d, cmd[1]);
				printf("%s",dest);
			    chdir(dest);
			  }	
				
		    else
			 { 
				chdir(cmd[1]);
			 } 
			
		 }
		 
		if(strcmp(cmd[0],"bg")==0)          // changing flag for bg processes
	     {   
		   interactive =0;	   
	     } 
		
		if(strcmp(cmd[0], "exit")==0)     //implementing exit command                   
		 {
			  return 0;
		 }
		 
		char *btr_in[2] = {"exit", "processes"};

		 for(int d =0;d<2;d++)
		 {
			 if(strcmp(cmd[0],btr_in[d])==0)
			 {
				 nbn = 0;
			 }
		 }
		 
	  if(nbn)
     	{	  
	      if((pid = fork())==0)                           // fork  call                 
	        {  
				 int fd0,fd1,in=0,out=0;
				 char *input=(char *)malloc(64);
				 char *output=(char *)malloc(64);
                 parsed = (char **)malloc(256);
				 while(bg_args[h]!='\0')                      // I/O redirection args change
				   {    
					   if(strcmp(bg_args[h],"<")==0)
					   {        
						
						bg_args[h]="";
						strcpy(input,bg_args[h+1]);
						parsed[h+1] = input;
						in=2; 
						
					    }               

					    if(strcmp(bg_args[h],">")==0)
					   {       
						
						bg_args[h] = NULL;
						strcpy(output,bg_args[h+1]);
						parsed[h+1] = output;
						out=2;
					
					    } 
					parsed[h] = bg_args[h];
					h=h+1;
					
				    }
				
				
			      if(in)                                             // file reading and writing
				  {   
					int fd0 = open(input, O_RDONLY, 0);
					if (fd0 < 0) {
						perror("Couldn't open input file");
						exit(0);
					}	
					close(0);
					dup(fd0);
					close(fd0);
				  }

				  if (out)
				  {

					int fd1 = open(output, O_WRONLY | O_CREAT, 0644);
					if (fd1  < 0) {
						perror("Couldn't open the output file");
						exit(0);
					}           
					close(1);
					dup2(fd1,1);
					close(fd1);
				  }
				 
				
				char * path = find_path(parsed[0]);                      // execv implementation
				fflush(stdout);
				int s = execv(path, parsed);
				
				   
			      if(s == -1)
			     {    
				    perror("Error: ");
				    _exit(1);
			      }	
	        }
	  
	        else if (pid < 0)
		   {  
		     perror("error");
		   }
	    } 
	  
	    if(interactive)                       // checking if interactive
	    {
		  while (wait(0) != pid);
 	    }
	    else                                  // saving child pid
	    {
            char *store=(char *)malloc(50); 
			int x=1;
		    while(x<=j-1)
		    {   
			   strcat(store,cmd[x]);
			   strcat(store, " ");
				x++;
		    }
		   bg_cmd[child_no] = (char *)malloc(256);
		   bg_cmd[child_no] = store;
		   child_pids[child_no++] = pid;
		   
	    }
		
	    if(strcmp(cmd[0],"processes")==0)      // processes command
	    { 
		
			if(child_no==0)
			  {
				  printf("\nno background process running currently");
			  }
			  else{
				   
				   for(y=0;y<child_no;y++)
				    { 
					 
					 //if(waitpid(child_pids[y], &status, WNOHANG)==0)
					   
					 //{	 
					
					  printf("\npid=%d",child_pids[y]);
					  printf("\nbg_cmd=%s",bg_cmd[y]);
					 //}	
				  
				    }
			      }  
	    }
			  
    }
  
  return 0;
}


  