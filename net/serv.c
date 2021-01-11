#include <stdio.h> 
#include <fcntl.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/file.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include<strings.h>
#include<time.h>

#define MAXLINE  511
#define MAX_SOCK 1024 //�ֶ󸮽��� 64

char *EXIT_STRING = "exit";
char *START_STRING = "Connected to chat_server\n";
char *START_STRING1 = "*******************Game Start*******************\n";
char *START_STRING2 = "***������ �Էµ� �ܾ ���Է��ϸ� Ż���˴ϴ�***\n";
char *START_STRING3 = "���� ���� �ܾ�:";
int maxfdp1;                // �ִ� ���Ϲ�ȣ +1
int num_chat = 0;           // ä�� ������ ��
int clisock_list[ MAX_SOCK ]; // ä�ÿ� ������ ���Ϲ�ȣ ���
int listen_sock;

char *game_start_word[10] = {"���", "������", "��ȭ", "����", "����", "�뷡��", "����", "�ѱ�", "����", "����"};
char first[10];
char last[10];

void addClient(int s, struct sockaddr_in *newcliaddr);// ���ο� ä�� ������ ó��
void removeClient(int);        // ä�� Ż�� ó�� �Լ�
int set_nonblock(int sockfd);  // ������ �ͺ������ ����
int is_nonblock(int sockfd);   // ������ �ͺ�� ������� Ȯ��
int tcp_listen(int host, int port, int backlog); // ���� ���� �� listen�Լ�
void errquit(char *mesg) { perror(mesg); exit(1); }
int game_start(int num_chat); //������ �����ϱ� ���� �Լ�
void first_word(char buf2[], char* word); //ù�ܾ� ���� �Լ�
void last_word(char buf2[], char* word); //������ �ܾ� ���� �Լ�
char *check_the_rule(char buf3[], int i); //��Ģ�Ǻ��Լ�

int main(int argc, char *argv[]) {
    char buf[MAXLINE];	
    int i, j, nbyte, count, addrlen;
    int accp_sock, clilen;
    struct sockaddr_in  cliaddr;

	int number=0;
	char* new_word;
	char firstt[10];
	char lastt[10];
	char compare_l[10];

    if(argc != 3) {
        printf("���� :%s port\n", argv[0]);
       exit(0);
   }

    listen_sock = tcp_listen(INADDR_ANY,atoi(argv[2]),5);
    if(listen_sock==-1)
        errquit("tcp_listen fail");
    if(set_nonblock(listen_sock) == -1)
        errquit("set_nonblock fail");

    for(count=0; ;count++) {

        addrlen = sizeof(cliaddr);
        accp_sock= accept(listen_sock, (struct sockaddr *)&cliaddr, &clilen);
        if(accp_sock == -1 && errno!=EWOULDBLOCK)
            errquit("accept fail");
        else if(accp_sock >0) {

            // ä�� Ŭ���̾�Ʈ ��Ͽ� �߰�
            clisock_list[num_chat] = accp_sock;

            // ��ſ� ������ �ͺ�� ��尡 �ƴ�
            if(is_nonblock(accp_sock)!=0 && set_nonblock(accp_sock)<0 )
                errquit("set_nonblock fail");

            addClient(accp_sock,&cliaddr);
            send(accp_sock, START_STRING, strlen(START_STRING), 0);
            printf("user%d ����.\n", num_chat);

			//ù �ܾ� �����ֱ�
		   if(num_chat == 3){
	  		    number = game_start(num_chat);
	
		   }
        }

        // Ŭ���̾�Ʈ�� ���� �޽����� ��� Ŭ���̾�Ʈ���� ���
        for(i = 0; i < num_chat; i++) {
            errno = 0;
            nbyte = recv(clisock_list[i], buf, MAXLINE, 0);
            if(nbyte==0) {
                removeClient(i);    // abrupt exit
                continue;
            }
            else if( nbyte==-1 && errno == EWOULDBLOCK)
                continue;
            // ���Ṯ�� ó��
            if(strstr(buf, EXIT_STRING) != NULL) {
                removeClient(i);    // abrupt exit
                continue;
            }
            // ��� ä�� �����ڿ��� �޽��� ���
            buf[nbyte] = 0;
            for (j = 0; j < num_chat; j++)
                send(clisock_list[j], buf, nbyte, 0);

            printf("%s\n", buf);
			
			//���� ���� �޼����� �̸� �и�
			//printf("%s\n", check_the_rule(buf,i));
			new_word = check_the_rule(buf,i);
			printf("%s\n", new_word);

			first_word(firstt,new_word);
			last_word(lastt,new_word);
			printf("ù��° ����: %s\n", firstt);
			printf("������ ����: %s\n", lastt);

			
			strcpy(compare_l, lastt);
			printf("**********%s\n", compare_l);
			//printf("%s\n",compare_l[0]);
		
			
		
			//if(compare_l == NULL){
			//     memcpy(compare_l,lastt, sizeof(lastt));
			//}
			//else{
			//	if(compare_l == firstt)
			//		printf("��ġ\n");
			//}

			//printf("%s",compare_l[0]);

        }
    }
}

//������ �����ϱ� ���� �Լ�
int game_start(int num_chat) {
	char *game_start_word[10] = {"���", "������", "��ȭ", "����", "����", "�뷡��", "����", "�ѱ�", "����", "����"};
	int number = 0; //���ڿ� �迭�� �ε��� �� ������ ���� ����
	//char first[10];
	//char last[10];
	char *word;
	int i;

	srand((int)time(NULL)); 
	number = rand()%5+1;

	printf("%s",START_STRING1);
	printf("%s\n",START_STRING2);
	printf("���� ���� �ܾ�: %s\n", game_start_word[number]);
	word = game_start_word[number];
	printf("%d\n",strlen(word));
	printf("%d\n",word[strlen(word)]);

	first_word(first,word);
	last_word(last,word);
	printf("ù��° ����: %s\n", first);
	printf("������ ����: %s\n", last);


	for(i=0; i<num_chat; i++){
		send(clisock_list[i], START_STRING1, strlen(START_STRING1),0);
		send(clisock_list[i], START_STRING2, strlen(START_STRING2),0);
		send(clisock_list[i], START_STRING3, strlen(START_STRING3),0);
		send(clisock_list[i], game_start_word[number], strlen(game_start_word[number]),0);
	}


	return number;

}

void first_word(char buf2[], char* word){

	sprintf(buf2,"%c%c%c",word[0], word[1], word[2]);	
}

void last_word(char buf2[], char* word){
	int addlen=strlen(word);

	if(addlen%3 == 0){	//3����Ʈ
		sprintf(buf2, "%c%c%c", word[addlen-3], word[addlen-2], word[addlen-1]); //2����Ʈ
	}
	if(addlen%3 == 1){
		sprintf(buf2, "%c%c%c", word[addlen-4], word[addlen-3], word[addlen-2]);
	}
}


char *check_the_rule(char buf3[], int i){
	
	//buf�� �ִ� user�̸��� �ܾ� �и��ϱ�
	char *new_word;
	char *ptr = strtok(buf3, ":");

	new_word = ptr;
	new_word = strtok(NULL, ":" );


	//if first_word(first, word)

	return new_word;
}


//void compare(char prelast, char first){
//
//
//}

//������ -> ����  �������� ������ �ܾ�� ������ ù��° �ܾ� �� 

// ���ο� ä�� ������ ó��
void addClient(int s, struct sockaddr_in *newcliaddr) {
    char buf[20];
    inet_ntop(AF_INET,&newcliaddr->sin_addr,buf,sizeof(buf));
    printf("new client: %s\n",buf);
     // ä�� Ŭ���̾�Ʈ ��Ͽ� �߰�
    clisock_list[num_chat] = s;
    num_chat++;
}
    // ä�� Ż�� ó��
void removeClient(int i) {
    close(clisock_list[i]);
    if(i != num_chat-1)
        clisock_list[i] = clisock_list[num_chat-1];
    num_chat--;
    printf("ä�� ������ 1�� Ż��. ���� ������ �� = %d\n", num_chat);
}

// ������ nonblock ���� Ȯ��
int is_nonblock(int sockfd) {
    int val;
    // ������ �÷��� ���� ���´�
    val=fcntl(sockfd, F_GETFL,0);
    // �ͺ�� ������� Ȯ��
    if(val & O_NONBLOCK)
        return 0;
    return -1;
}

    // ������ �ͺ�� ���� ����
int set_nonblock(int sockfd) {
    int val;
    // ������ �÷��� ���� ���´�
    val=fcntl(sockfd, F_GETFL,0);
    if(fcntl(sockfd, F_SETFL, val | O_NONBLOCK) == -1)
        return -1;
    return 0;
}

// listen ���� ���� �� listen
int  tcp_listen(int host, int port, int backlog) {
    int sd;
    struct sockaddr_in servaddr;

    sd = socket(AF_INET, SOCK_STREAM, 0);
    if(sd == -1) {
        perror("socket fail");
        exit(1);
    }
    // servaddr ����ü�� ���� ����
    bzero((char *)&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(host);
    servaddr.sin_port = htons(port);
    if (bind(sd , (struct sockaddr *)&servaddr, sizeof(servaddr))
        < 0) {
        perror("bind fail");  exit(1);
    }
    // Ŭ���̾�Ʈ�κ��� �����û�� ��ٸ�
    listen(sd, backlog);
    return sd;
}




