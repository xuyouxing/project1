#include "order_goods.h"
#define MAXLINE 1024 
#define OPEN_MAX 100
#define LISTENQ 20
#define SERV_PORT 5000
#define INFTIM 1000

void setnonblocking(int sock)
{
	int opts;
	opts = fcntl(sock,F_GETFL);
	if(opts < 0){
		perror("fcntl(sock,GETFL)");
		exit(1);
	}
	opts = opts | O_NONBLOCK;
	if(fcntl(sock,F_SETFL,opts) < 0){
		perror("fcntl(sock,SETFL,opts)");
		exit(1);
	}
}

void client_regist(char name[],char code[],int sockfd)
{
	MYSQL db,*pdb;
	char sql[1024];
	printf("in sign up\n");
	mysql_init(&db);
	pdb = mysql_real_connect(&db,"localhost","root","123","xyx",0,NULL,0);
	if(pdb == NULL){
		printf("mysql connect failed\n");
	}
	else{
		printf("mysql connect success\n");
	}
	memset(sql,0,1024);		
	sprintf(sql,"insert into register values(\'%s\',\'%s\');",name,code);
					
	if(mysql_query(&db,sql)){
		printf("insert into failed\n");
		return;
	}
	printf("in input mysql\n");
	char buff[32] = "register ok";
	write(sockfd,buff,sizeof(buff));
	mysql_close(&db);
}

void client_load(char name[],char code[],int sockfd)
{
	MYSQL db,*pdb;
	char sql[1024];
	char buf[32];
	MYSQL_RES *ptr;
	MYSQL_ROW result;
	MYSQL_FIELD *field;
	int ret,row,column,i,j;
	mysql_init(&db);
	pdb = mysql_real_connect(&db,"localhost","root","123","xyx",0,NULL,0);
	if(pdb == NULL){
		printf("connect mysql error\n");
		return;
	}
	sprintf(sql,"select * from register where name = \'%s\';",name);
	ret = mysql_query(&db,sql);
	ptr = mysql_store_result(&db);//获取查询结果，ptr指向存储结果的一段内存
	row = mysql_num_rows(ptr);
	if(row <= 0){
		memset(buf,0,32);
		strcpy(buf,"your name is error");
		write(sockfd,buf,strlen(buf));
		return;
	}
	result = mysql_fetch_row(ptr);
	if(strcmp(result[1],code) != 0){
		memset(buf,0,32);
		strcpy(buf,"your code is error");
		write(sockfd,buf,strlen(buf));
		return;
	}
	else {
		memset(buf,0,32);
		strcpy(buf,"login ok");
		write(sockfd,buf,strlen(buf));
	}
	mysql_free_result(ptr);
	mysql_close(&db);
	return;
}

void client_add(char trade_name[],char menu[],char tel[],int sockfd)
{
	MYSQL db,*pdb;
	char sql[1024];
	mysql_init(&db);
	pdb = mysql_real_connect(&db,"localhost","root","123","xyx",0,NULL,0);
	if(pdb == NULL){
		printf("mysql connect error\n");
		return;
	}
	printf("connect mysql ok\n");
	memset(sql,0,sizeof(sql));
	sprintf(sql,"insert into menu values(\'%s\',\'%s\',\'%s\');",trade_name,menu,tel);
	
	if(mysql_query(&db,sql)){
		printf("insert into failed\n");
		return;
	}
	char buf[32] = "add menu ok";
	write(sockfd,buf,sizeof(buf));
	mysql_close(&db);
	return;
}

void client_alter(char trade_name[],char menu[],int sockfd)
{
	MYSQL db,*pdb;
	char sql[1024];
	char buf[32];
	mysql_init(&db);
	pdb = mysql_real_connect(&db,"localhost","root","123","xyx",0,NULL,0);
	if(pdb == NULL){
		printf("connect mysql error\n");
		return;
	}
	sprintf(sql,"update menu set menu = \'%s\' where trade_name = \'%s\';",menu,trade_name);
	if(mysql_query(&db,sql)){
		printf("alter menu error\n");
		return;
	}
	strcpy(buf,"alter ok");
	write(sockfd,buf,strlen(buf));
	mysql_close(&db);
	return;
}

void client_inquiry(int sockfd)
{
	MYSQL db,*pdb;
	char sql[1024];
	char buf[4096];
	MYSQL_RES *ptr;
	MYSQL_ROW result;
	MYSQL_FIELD *field;
	int ret,row,column,i,j;
	mysql_init(&db);
	pdb = mysql_real_connect(&db,"localhost","root","123","xyx",0,NULL,0);
	if(pdb == NULL){
		printf("connect mysql error\n");
		return;
	}
	strcpy(sql,"select * from menu;");
	if(mysql_query(&db,sql)){
		printf("inquiiy menu error\n");
		return;
	}
	ptr = mysql_store_result(&db);
	row = mysql_num_rows(ptr);
	column = mysql_num_fields(ptr);
	memset(buf,0,sizeof(buf));
	for(i = 0;i < row;i++){
		result = mysql_fetch_row(ptr);
		for(j = 0;j < column;j++){
			strcat(buf,result[j]);
			strcat(buf,"\t");
		}
		strcat(buf,"\n");
	}
	ret = write(sockfd,buf,strlen(buf));
	if(ret < 0)
		return;
	mysql_free_result(ptr);
	mysql_close(&db);
}

void order_management(char name[],char trade_name[],char menu[],int sockfd)
{
	time_t t;
	char *pc;
	time(&t);
	pc = ctime(&t);
	MYSQL db,*pdb;
	MYSQL_RES *ptr;
	MYSQL_ROW result;
	MYSQL_FIELD *field;
	int ret,row,column,i,j;
	char sql[1024];
	char management[10] = "ok";
	char buf[15];
	mysql_init(&db);
	pdb = mysql_real_connect(&db,"localhost","root","123","xyx",0,NULL,0);
	if(pdb == NULL){
		printf("mysql connect error\n");
		return;
	}
	printf("connect mysql ok\n");
	memset(sql,0,sizeof(sql));
	strcpy(buf,"order ok");
	sprintf(sql,"insert into goods_client values(\'%s\',\'%s\',\'%s\',\'%s\',\'%s\');",name,trade_name,menu,pc,buf);
	
	if(mysql_query(&db,sql)){
		printf("insert into failed\n");
		return;
	}
	memset(sql,0,sizeof(sql));
	sprintf(sql,"insert into goods_management values(\'%s\',\'%s\',\'%s\',\'%s\',\'%s\');",name,trade_name,menu,pc,management);
	
	if(mysql_query(&db,sql)){
		printf("insert into failed\n");
		return;
	}
	memset(sql,0,sizeof(sql));
	sprintf(sql,"select * from goods_client where name = \'%s\';",name);
	if(mysql_query(&db,sql)){
		printf("inquiry goods_client error\n");
		return;
	}
	ptr = mysql_store_result(&db);
	row = mysql_num_rows(ptr);
	column = mysql_num_fields(ptr);
	for(i = 0;i < row;i++){
		result = mysql_fetch_row(ptr);
		ret = write(sockfd,result[column-1],8);
		if(ret < 0)
			return;
	}
	mysql_close(&db);
	return;
}

void cancel_order(char name[],char trade_name[],char menu[],int sockfd)
{
	time_t t;
	char *pc;
	time(&t);
	pc = ctime(&t);
	MYSQL db,*pdb;
	char sql[1024];
	char management[10] = "cancel";
	mysql_init(&db);
	pdb = mysql_real_connect(&db,"localhost","root","123","xyx",0,NULL,0);
	if(pdb == NULL){
		printf("mysql connect error\n");
		return;
	}
	printf("connect mysql ok\n");
	memset(sql,0,sizeof(sql));
	sprintf(sql,"insert into goods_management values(\'%s\',\'%s\',\'%s\',\'%s\',\'%s\');",name,trade_name,menu,pc,management);
	
	if(mysql_query(&db,sql)){
		printf("insert into goods_management failed\n");
		return;
	}
	char buf[32] = "cancel an order ok";
	write(sockfd,buf,sizeof(buf));
	sprintf(sql,"insert into goods_client values(\'%s\',\'%s\',\'%s\',\'%s\',\'%s\');",name,trade_name,menu,pc,management);
	
	if(mysql_query(&db,sql)){
		printf("insert into goods_client failed\n");
		return;
	}
	mysql_close(&db);
	return;
}

void inquiry_receive_order(int sockfd)
{
	MYSQL db,*pdb;
	char sql[1024];
	char buf[4096];
	char rec[15];
	MYSQL_RES *ptr;
	MYSQL_ROW result;
	MYSQL_FIELD *field;
	int ret,row,column,i,j;
	mysql_init(&db);
	pdb = mysql_real_connect(&db,"localhost","root","123","xyx",0,NULL,0);
	if(pdb == NULL){
		printf("connect mysql error\n");
		return;
	}
	strcpy(sql,"select * from goods_client;");
	if(mysql_query(&db,sql)){
		printf("inquiry order error\n");
		return;
	}
	ptr = mysql_store_result(&db);
	row = mysql_num_rows(ptr);
	column = mysql_num_fields(ptr);
	memset(buf,0,sizeof(buf));
	for(i = 0;i < row;i++){
		result = mysql_fetch_row(ptr);
		if(strcmp(result[column-1],"order ok") == 0){
			for(j = 0;j < column;j++){
				strcat(buf,result[j]);
				strcat(buf,"\t");
			}
			strcat(buf,"\n");
		}
	//	strcat(buf,"\n");
	}
	ret = write(sockfd,buf,strlen(buf));
	if(ret < 0)
		return;
	mysql_free_result(ptr);
	mysql_close(&db);
}

void receive_order(char name[],int sockfd)
{
	MYSQL db,*pdb;
	char sql[1024];
	char buf[4096];
	char rec[15];
	char order[10] = "order ok";
	MYSQL_RES *ptr;
	MYSQL_ROW result;
	MYSQL_FIELD *field;
	int ret,row,column,i,j;
	mysql_init(&db);
	pdb = mysql_real_connect(&db,"localhost","root","123","xyx",0,NULL,0);
	if(pdb == NULL){
		printf("connect mysql error\n");
		return;
	}
	strcpy(sql,"select * from goods_client;");
	if(mysql_query(&db,sql)){
		printf("inquiiy menu error\n");
		return;
	}
	ptr = mysql_store_result(&db);
	row = mysql_num_rows(ptr);
	column = mysql_num_fields(ptr);
	memset(buf,0,sizeof(buf));
	if(strcmp(name,"y") == 0){
		memset(sql,0,sizeof(sql));
		memset(rec,0,sizeof(rec));
		strcpy(rec,"have receive");
		for(i = 0;i < row;i++){
			result = mysql_fetch_row(ptr);
			if(strcmp(result[column-1],order) == 0){
				sprintf(sql,"update goods_client set flag = \'%s\' where flag = \'%s\';",rec,order);	
				if(mysql_query(&db,sql)){
					printf("update goods_client error\n");
					return;
				}
				for(j = 0;j < column;j++){
					strcat(buf,result[j]);
					strcat(buf,"\t");
				}	
				ret = write(sockfd,buf,strlen(buf));
				if(ret < 0)
					return;
			}
		}
	}
	if(strcmp(name,"n") == 0){
		memset(sql,0,sizeof(sql));
		memset(rec,0,sizeof(rec));
		strcpy(rec,"have refuse");
		for(i = 0;i < row;i++){
			result = mysql_fetch_row(ptr);
			if(strcmp(result[column-1],order) == 0){
				sprintf(sql,"update goods_client set flag = \'%s\' where flag = \'%s\';",rec,order);		
				if(mysql_query(&db,sql)){
					printf("inquiiy menu error\n");
					return;
				}
				for(j = 0;j < column;j++){
					strcat(buf,result[j]);
					strcat(buf,"\t");
				}	
				ret = write(sockfd,buf,strlen(buf));
				if(ret < 0)
					return;
			}
		}
	}
	mysql_free_result(ptr);
	mysql_close(&db);
}

void inquiry_order(char name[],int sockfd)
{
	MYSQL db,*pdb;
	char sql[1024];
	char buf[4096];
	char rec[15];
	MYSQL_RES *ptr;
	MYSQL_ROW result;
	MYSQL_FIELD *field;
	int ret,row,column,i,j;
	mysql_init(&db);
	pdb = mysql_real_connect(&db,"localhost","root","123","xyx",0,NULL,0);
	if(pdb == NULL){
		printf("connect mysql error\n");
		return;
	}
	sprintf(sql,"select * from goods_client where name = \'%s\';",name);
	if(mysql_query(&db,sql)){
		printf("inquiry order error\n");
		return;
	}
	ptr = mysql_store_result(&db);
	row = mysql_num_rows(ptr);
	column = mysql_num_fields(ptr);
	memset(buf,0,sizeof(buf));
	for(i = 0;i < row;i++){
		result = mysql_fetch_row(ptr);
		for(j = 0;j < column;j++){
			strcat(buf,result[j]);
			strcat(buf,"\t");
		}
		strcat(buf,"\n");
	}
	ret = write(sockfd,buf,strlen(buf));
	if(ret < 0)
		return;
	mysql_free_result(ptr);
	mysql_close(&db);
}

int main()
{
	int i, maxi, listenfd, connfd, sockfd,epfd,nfds;
	ssize_t n;
	char line[MAXLINE];
	socklen_t clilen;

	struct epoll_event ev,events[20];
	struct sockaddr_in ser_addr,cli_addr;
    
	listenfd = socket(AF_INET, SOCK_STREAM, 0);
	if(listenfd < 0){
		printf("cretae socket error\n");
		return -1;
	}
	setnonblocking(listenfd);

	epfd = epoll_create(256);
	ev.data.fd = listenfd;
	ev.events = EPOLLIN | EPOLLET;
	epoll_ctl(epfd,EPOLL_CTL_ADD,listenfd,&ev);

	memset(&ser_addr, 0, sizeof(ser_addr));
	ser_addr.sin_family = AF_INET;
	ser_addr.sin_port = htons(2016);
	inet_pton(AF_INET,"127.0.0.1",&ser_addr.sin_addr.s_addr);
	bind(listenfd,(struct sockaddr *)&ser_addr, sizeof(ser_addr));
	listen(listenfd, LISTENQ);
	maxi = 0;
	for ( ; ; ) {
		nfds = epoll_wait(epfd,events,20,500);
		for(i = 0;i < nfds;++i){
			if(events[i].data.fd == listenfd){
				clilen = sizeof(&cli_addr);
				connfd = accept(listenfd,(struct sockaddr *)&cli_addr, &clilen);
				if(connfd < 0){
					perror("connfd<0");
					exit(1);
				}
				setnonblocking(connfd);
				printf("accept a connection \n");			
	
				ev.data.fd = connfd;
				ev.events = EPOLLIN | EPOLLET;
				epoll_ctl(epfd,EPOLL_CTL_ADD,connfd,&ev);
			}
			else if(events[i].events & EPOLLIN){
				if ( (sockfd = events[i].data.fd) < 0)
					continue;
				if ( (n = read(sockfd, line, MAXLINE)) < 0) {
					if (errno == ECONNRESET) {
						close(sockfd);
						events[i].data.fd = -1;
					} else
						printf("readline error\n");
				}
				else if (n == 0){
					close(sockfd);
					events[i].data.fd = -1;
				}
				int tailLen = 0;
				while(1){
					if(tailLen >= n)
						break;
					MSG msg;
					int msglen;
					msglen = *((int *)line);
					msglen = ntohl(msglen);
					memcpy(&(msg), line+tailLen, msglen);
					msg.type = ntohl(msg.type);
					if(msg.type == MSGTYPE_REGIST){
						client_regist(msg.name,msg.code,sockfd);
					}
					else if(msg.type == MSGTYPE_CODE){
						client_load(msg.name,msg.code,sockfd);
					}
					else if(msg.type == MSGTYPE_ADD){
						client_add(msg.trade_name,msg.menu,msg.tel,sockfd);

					}
					else if(msg.type == MSGTYPE_ALTER){
						client_alter(msg.trade_name,msg.menu,sockfd);
					}
					else if(msg.type == MSGTYPE_INQUIRY){
						client_inquiry(sockfd);
					}
					else if(msg.type == MSGTYPE_PLACE){
						order_management(msg.name,msg.trade_name,msg.menu,sockfd);
					}
					else if(msg.type == MSGTYPE_CANACEL){
						cancel_order(msg.name,msg.trade_name,msg.menu,sockfd);	
					}
					else if(msg.type == MSGTYPE_RECEIVE_INQUIRY){
						inquiry_receive_order(sockfd);
					}
					else if(msg.type == MSGTYPE_INQUIRY_ORDER){
						inquiry_order(msg.name,sockfd);
					}
					else if(msg.type == MSGTYPE_RECEIVE){
						receive_order(msg.name,sockfd);
					}
					tailLen = tailLen + msglen; 
					ev.data.fd = sockfd;
			//		ev.events = EPOLLOUT | EPOLLET;
					epoll_ctl(epfd,EPOLL_CTL_MOD,sockfd,&ev);
					printf("**************\n");
				}
			}
			else if(events[i].events & EPOLLOUT){
				sockfd = events[i].data.fd;
				strcpy(line,"hello client");
				write(sockfd, line, strlen(line));

				ev.data.fd=sockfd;

				ev.events=EPOLLIN|EPOLLET;

				epoll_ctl(epfd,EPOLL_CTL_MOD,sockfd,&ev);
			}
		}
	}
	return 0;
}
