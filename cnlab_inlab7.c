    #include <arpa/inet.h>
    #include <netinet/in.h>
    #include <stdio.h>
    #include <string.h>
    #include <sys/socket.h>
    #include <unistd.h>
    #include <pthread.h>
    #include <semaphore.h>
    #include <stdlib.h>
     
    sem_t w;
    int numclient=0;
    pthread_t threads[100],cthread;
    int new_socket[100];
    struct sockaddr_in caddress[100];
    char global[2048]={0};
    char *details[100];
    char *names[100];
    char list[2048]={0};
     
    void* clientrecieve(void* param)
    { 
        int* par=(int*)param;
        int sock=par[0];
        free(par);
        while(1)
        {
            char buffer[1024]={0};
            printf("Enter message");
            fgets(buffer,1024,stdin);
            char buffer2[1024]= "MESG " ;
            strcat(buffer2,buffer);
            if(!strcmp(buffer,"EXIT\n")) 
            {   
                send(sock, buffer, strlen(buffer), 0);
                pthread_exit(NULL);
            }
            else
            {
                send(sock, buffer2, strlen(buffer2), 0);
            }
        }
    }
     
    void* handle(void* param)
    {   
        int* par=(int*)param;
        int i=par[0];
        int index=par[0];
        free(par);
        printf("Client connected %d | %s:%d!\n\n", i+1, inet_ntoa(caddress[i].sin_addr), ntohs(caddress[i].sin_port));
        char buff1[100] = { 0 };
        read(new_socket[i], buff1, 100);
        printf("client %d : %s\n", i+1, buff1);  //helo
        char send1[]="MESG Name?\n";
        send(new_socket[i], send1, strlen(send1), 0);
        char name[100]={0};
        char name2[100]={0};
        read(new_socket[i], name, 100);
        strcpy(name2, name + 21);    //name remove MESG not contains \n
        printf("client %d : %s\n", i+1, name2);
        char write[100];
        strcpy(write,name2);
        strcat(write,"@");
        strcat(write,inet_ntoa(caddress[i].sin_addr));
        strcat(write,":");
        char port[10]={0};
        sprintf(port,"%d",ntohs(caddress[i].sin_port));
        strcat(write,port);
        char write2[100];
        strcpy(write2,write);
        strcat(write,"\n");
        printf("details : %s\n",write);
        names[i]=name2;
        details[i]=write;
        strcat(list,name2);
        strcat(list,"\n"); 
        char ggbro[100]={0};
        read(new_socket[i], ggbro, 100);
        if(!strcmp(ggbro,"LIST"))
        send(new_socket[i], list, strlen(list), 0);
     
        while(1)
        {   
            char buffer[1024]={0};
            char buffer2[1024]={0};
            read(new_socket[i], buffer, 1024);
            strcpy(buffer2, buffer+5);//contain \n
            if(!strcmp(buffer,"EXIT\n"))
            {   
                for(int x=0;x<numclient;x++)
                {
                    send(new_socket[x], buffer, strlen(buffer), 0);
                }
                break;
            }
            else if(buffer[0]=='M'&&buffer[1]=='E'&&buffer[2]=='S'&&buffer[3]=='G')
            {   
                
                char buffer3[1024]={0};
                char message[1024]={0};
                int flag=0;
                int p=0;
                for(int l=0;l<strlen(buffer2);l++)  //seperating message from string
                {
                    if(buffer2[l]==' '&&flag==0) {flag=1; continue;}
                    if(flag==0)
                    {
                        buffer3[l]=buffer2[l];
                        continue;
                    } 
                    else if(flag==1)
                    {
                        message[p]=buffer2[l];
                        p++;
                    } 
                }
                printf("client %d : %s\n", i+1, message);
                int found=0;
                int k;
                for(int i=0;i<numclient;i++)
                {
                    if(!strcmp(buffer3,names[i]))
                    {
                        found=1;
                        k=i;
                    }
                }
                char x[100]="404 not found";
                char x1[100]="201 available";
                //if(!found) send(new_socket[index], x, strlen(x), 0);
                
                    //send(new_socket[i], x1, strlen(x1), 0);
                    char arr[1024]={0};
                    strcat(arr,message);
                    char arr3[100]="MESG ";
                    char arr2[100]={0};
                    strcpy(arr2,name2);
                    strcat(arr2,": ");
                    char arr4[100]={0};
                    strcat(arr4,arr2);
                    strcat(arr4,message);
                    sem_wait(&w);
                    FILE* ff = fopen("clientid1_clientid2.txt", "a");
                    fprintf(ff, "%s", arr4);
                    fclose(ff);
                    sem_post(&w);
                    strcat(arr2,arr);
                    strcat(arr3,arr2);
                    send(new_socket[k], arr3, strlen(arr3), 0);
                    printf("Send to:  %s\n",buffer3);
                
            }
            else
            {
                printf("Invalid Message\n");
                pthread_exit(NULL);
            }
        }
        
    }
     
    int main(int argc, char const* argv[])
    {	
    	const char *ip=argv[1];
        int port=atoi(argv[2]);
    	const char *mode=argv[3];
        sem_init(&w, 0, 1);
     
        if(!strcmp(mode,"c"))
        {
            int sock = 0, valread, client_fd;
            struct sockaddr_in server_address,client_address;
            
            if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
                printf("\n Socket creation error \n");
                return -1;
            }
     
            server_address.sin_family = AF_INET;
            server_address.sin_port = htons(port);
            server_address.sin_addr.s_addr = inet_addr(ip);
     
            if ((client_fd = connect(sock, (struct sockaddr*)&server_address,sizeof(server_address)))< 0)
            {
                printf("could not find the server on %s:%d\n", inet_ntoa(server_address.sin_addr), ntohs(server_address.sin_port));
                return -1;
            }
     
            socklen_t clientaddr_len = sizeof(client_address);
            memset(&client_address, 0, clientaddr_len);
            if (getsockname(sock, (struct sockaddr *)&client_address, &clientaddr_len))
            printf("getsockname error");
            printf("%s:%d is connected to ", inet_ntoa(client_address.sin_addr), ntohs(client_address.sin_port));
            printf("%s:%d\n", inet_ntoa(server_address.sin_addr), ntohs(server_address.sin_port));
     
            char send1[]="HELO\n";
            send(sock, send1, strlen(send1), 0);
            char read1[100] = { 0 };
            valread = read(sock, read1, 100);
            char temp[100]={0};
            strcpy(temp,read1+5);
            printf("server : %s\n", temp);
            char name[100]={0};
            fgets(name, 100, stdin);
            char name1[1024] = "MESG SERVER username:" ;
            strcat(name1,name);
            send(sock, name1, strlen(name1)-1, 0);
            char command[1024]={0};
            printf("Enter the List command");
            gets(command);
            char ggbro1[1024]={0};
            send(sock, command, strlen(command), 0);
            valread=read(sock,ggbro1,1024);
            printf("%s",ggbro1);
            int *res=(int*)malloc(sizeof(int));
            res[0]=sock;
            if (pthread_create(&cthread, NULL, clientrecieve,(void*)res)!= 0)
            {
    			printf("Failed to create thread\n");
            }
     
            while(1)
            {   
                memset(global, 0, sizeof global);
                read(sock, global, 2048);
                if(global[0]=='M'&&global[1]=='E'&&global[2]=='S'&&global[3]=='G'&&global[4]==' ')
                {      
                    char arr[100]={0};
                    strcpy(arr,global+5);
                    printf("\n%s",arr);
                    puts("\nEnter message ");
                }
                else if(global[0]=='E'&&global[1]=='X'&&global[2]=='I'&&global[3]=='T'&&global[4]=='\n')
                {   
                    printf("exiting...\n");
                    break;
                }
            }
            
        }
     
        else if(!strcmp(mode,"s"))
        {
            int server_fd, valread;
            struct sockaddr_in address;
            int opt = 1;
            int addrlen = sizeof(address);
            FILE* ff = fopen("clientid1_clientid2.txt", "w");
            fclose(ff);
     
            if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
                printf("socket failed");
                return -1;
            }
     
            address.sin_family = AF_INET;
            address.sin_addr.s_addr = inet_addr(ip);
            address.sin_port = htons(port);
            
            if (bind(server_fd, (struct sockaddr*)&address,sizeof(address))< 0) 
            {
                printf("bind failed");
                return -1;
            }
     
            if (listen(server_fd, 100) < 0) 
            {
                printf("listen");
                return -1;
            }
            
            int i=-1;
            while(1)
            {   
                i++;
                int *res=(int*)malloc(sizeof(int));
                res[0]=i;
                if ((new_socket[i]= accept(server_fd, (struct sockaddr*)&caddress[i],(socklen_t*)&addrlen))< 0) 
                {
                    printf("accept");
                    return-1;
                }
                numclient++;
                if (pthread_create(&threads[i], NULL, handle,(void*)res)!= 0){
    				printf("Failed to create thread\n");
                }
            }
            shutdown(server_fd, SHUT_RDWR);
        }
     
        else
        {
            printf("Enter valid mode s(server) or c(client)\n");
        }
    	return 0;
    }

