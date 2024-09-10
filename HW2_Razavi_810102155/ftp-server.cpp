/*FTP server*/
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <iostream>
using namespace std;

/*for getting file size using stat()*/
#include<sys/stat.h>

/*for sendfile()*/
#include<sys/sendfile.h>

/*for O_RDONLY*/
#include<fcntl.h>

int main(int argc,char *argv[])
{
  struct sockaddr_in server, client;
  struct stat obj;
  int sock1, sock2;
  char username[100] , password[100],
  buf[100], command[5], filename[20];
  int k, i, size, len, c;
  int filehandle;
  fstream my_file;
  sock1 = socket(AF_INET, SOCK_STREAM, 0);
  if(sock1 == -1)
  {
      printf("Socket creation failed");
      exit(1);
  }
  server.sin_port = htons(9999);
  server.sin_addr.s_addr = 0;
  k = bind(sock1,(struct sockaddr*)&server,sizeof(server));
  if(k == -1)
  {
      printf("Binding error \n");
      exit(1);
  }

  k = listen(sock1,1);
  if(k == -1)
  {
      printf("Listen failed \n");
      exit(1);
  }

  len = sizeof(client);
  sock2 = accept(sock1,(struct sockaddr*)&client,  (socklen_t*)&len);
  i = 1;

  send(sock2 , "Please enter your username : " , 100 , 0);
  recv(sock2 , username , 100 , 0);
  printf("username : %s\n", username);

  send(sock2 , "Please enter your password : " , 100 , 0);
  recv(sock2 , password , 100 , 0);
  printf("password : %s\n", password);

  if(username != "mahdi" || password != "mahdi-pass")
  {
    printf("This is username : %s\n", username);
    printf("This is password : %s\n", password);
     printf("Authentication failed \n" );
//    send(sock1 , "Authentication failed \n" , 100 , 0);
//    exit(1);
    //break;
  }

// FILE * fp;
// char * line = NULL;
// size_t len = 0;
// ssize_t read;
//
// fp = fopen("configuration.txt", "r");
// if (fp == NULL)
//    exit(EXIT_FAILURE);
//
// while (read = getline(&line, &len, fp) != -1)
// {
//    printf("Retrieved line of length %zu:\n", read);
//    printf("%s", line);
// }

  while(1)
  {
      recv(sock2, buf, 100, 0);
      sscanf(buf, "%s", command);
      if(!strcmp(command, "ls"))
	     {
      	  system("ls -l > temps.txt");
      	  i = 0;
      	  stat("temps.txt",&obj);
      	  size = obj.st_size;
      	  send(sock2, &size, sizeof(int),0);
      	  filehandle = open("temps.txt", O_RDONLY);
      	  sendfile(sock2,filehandle,NULL,size);
	     }


      else if(!strcmp(command,"get"))
	     {
      	  sscanf(buf, "%s%s", filename, filename);
      	  stat(filename, &obj);
      	  filehandle = open(filename, O_RDONLY);
      	  size = obj.st_size;
      	  if(filehandle == -1)
      	      size = 0;
      	  send(sock2, &size, sizeof(int), 0);

      	  if(size)
      	     sendfile(sock2, filehandle, NULL, size);
	     }

      else if(!strcmp(command, "put"))
        {
      	  int c = 0, c2 = 0 ,  len;
      	  char *f;
      	  sscanf(buf+strlen(command), "%s", filename);
      	  recv(sock2, &size, sizeof(int), 0);
      	  i = 1;
      	  while(1)
      	  {
      	        filehandle = open(filename, O_CREAT | O_EXCL | O_WRONLY, 0666);
      	        if(filehandle == -1)
      		      {
      		          sprintf(filename + strlen(filename), "%d", i);
      		      }
      	      else
      		      break;
      	   }

      	  f = (char*)malloc(size);
      	  recv(sock2, f, size, 0);
          //c = (int)(my_file << f);
          my_file << f ;
          my_file.close();
          c = 1;
      	  // c = write(filehandle, f, size);
      	  // close(filehandle);
          send(sock2, &c , sizeof(int), 0);
        }


  else if(!strcmp(command, "pwd"))
	{
      // system("echo 257 | pwd |  > temp-pwd.txt");
	    // system("pwd > temp-pwd.txt");
      system("pwd > temp-pwd.txt");
	    i = 0;
      FILE*f = fopen("temp-pwd.txt","r");
      // printf("here here 0\n");

      while(!feof(f))
        buf[i++] = fgetc(f);

      buf[i-1] = '\0';
      fclose(f);
      printf("%s", buf);
      system("pwd");

      send(sock2, buf, 100, 0);
	}

  else if (!strcmp(command , "mk")) {
    system(command);
  }

  else if(!strcmp(command , "rm")){
    system(command);
  }

  else if (!strcmp(command , "mv")) {
    system(command);
  }

  else if(!strcmp(command, "cd"))
  {
    if(chdir(buf+3) == 0)
	         c = 1;
	  else
	         c = 0;

    send(sock2, &c, sizeof(int), 0);
  }


  else if(!strcmp(command, "bye") || !strcmp(command, "quit"))
	{
	  printf("FTP server quitting..\n");
	  i = 1;
	  send(sock2, &i, sizeof(int), 0);
	  exit(0);
	}

    }
  return 0;
}
