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
#define MAX_SOCK 1024 //솔라리스는 64

char *EXIT_STRING = "exit";
char *START_STRING = "Connected to chat_server\n";
char *START_STRING1 = "*******************Game Start*******************\n";
char *START_STRING2 = "***이전에 입력된 단어를 재입력하면 탈락됩니다***\n";
char *START_STRING3 = "게임 시작 단어:";
int maxfdp1;                // 최대 소켓번호 +1
int num_chat = 0;           // 채팅 참가자 수
int clisock_list[ MAX_SOCK ]; // 채팅에 참가자 소켓번호 목록
int listen_sock;

char *game_start_word[10] = {"사과", "강아지", "영화", "여행", "공부", "노래방", "딸기", "한국", "숙제", "방학"};
char first[10];
char last[10];

void addClient(int s, struct sockaddr_in *newcliaddr);// 새로운 채팅 참가자 처리
void removeClient(int);        // 채팅 탈퇴 처리 함수
int set_nonblock(int sockfd);  // 소켓을 넌블록으로 설정
int is_nonblock(int sockfd);   // 소켓이 넌블록 모드인지 확인
int tcp_listen(int host, int port, int backlog); // 소켓 생성 및 listen함수
void errquit(char *mesg) { perror(mesg); exit(1); }
int game_start(int num_chat); //게임을 시작하기 위한 함수
void first_word(char buf2[], char* word); //첫단어 저장 함수
void last_word(char buf2[], char* word); //마지막 단어 저장 함수
char *check_the_rule(char buf3[], int i); //규칙판별함수

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
        printf("사용법 :%s port\n", argv[0]);
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

            // 채팅 클라이언트 목록에 추가
            clisock_list[num_chat] = accp_sock;

            // 통신용 소켓은 넌블록 모드가 아님
            if(is_nonblock(accp_sock)!=0 && set_nonblock(accp_sock)<0 )
                errquit("set_nonblock fail");

            addClient(accp_sock,&cliaddr);
            send(accp_sock, START_STRING, strlen(START_STRING), 0);
            printf("user%d 입장.\n", num_chat);

			//첫 단어 보내주기
		   if(num_chat == 3){
	  		    number = game_start(num_chat);
	
		   }
        }

        // 클라이언트가 보낸 메시지를 모든 클라이언트에게 방송
        for(i = 0; i < num_chat; i++) {
            errno = 0;
            nbyte = recv(clisock_list[i], buf, MAXLINE, 0);
            if(nbyte==0) {
                removeClient(i);    // abrupt exit
                continue;
            }
            else if( nbyte==-1 && errno == EWOULDBLOCK)
                continue;
            // 종료문자 처리
            if(strstr(buf, EXIT_STRING) != NULL) {
                removeClient(i);    // abrupt exit
                continue;
            }
            // 모든 채팅 참가자에게 메시지 방송
            buf[nbyte] = 0;
            for (j = 0; j < num_chat; j++)
                send(clisock_list[j], buf, nbyte, 0);

            printf("%s\n", buf);
			
			//버프 안의 메세지와 이름 분리
			//printf("%s\n", check_the_rule(buf,i));
			new_word = check_the_rule(buf,i);
			printf("%s\n", new_word);

			first_word(firstt,new_word);
			last_word(lastt,new_word);
			printf("첫번째 글자: %s\n", firstt);
			printf("마지막 글자: %s\n", lastt);

			
			strcpy(compare_l, lastt);
			printf("**********%s\n", compare_l);
			//printf("%s\n",compare_l[0]);
		
			
		
			//if(compare_l == NULL){
			//     memcpy(compare_l,lastt, sizeof(lastt));
			//}
			//else{
			//	if(compare_l == firstt)
			//		printf("일치\n");
			//}

			//printf("%s",compare_l[0]);

        }
    }
}

//게임을 시작하기 위한 함수
int game_start(int num_chat) {
	char *game_start_word[10] = {"사과", "강아지", "영화", "여행", "공부", "노래방", "딸기", "한국", "숙제", "방학"};
	int number = 0; //문자열 배열의 인덱스 값 난수를 위한 변수
	//char first[10];
	//char last[10];
	char *word;
	int i;

	srand((int)time(NULL)); 
	number = rand()%5+1;

	printf("%s",START_STRING1);
	printf("%s\n",START_STRING2);
	printf("게임 시작 단어: %s\n", game_start_word[number]);
	word = game_start_word[number];
	printf("%d\n",strlen(word));
	printf("%d\n",word[strlen(word)]);

	first_word(first,word);
	last_word(last,word);
	printf("첫번째 글자: %s\n", first);
	printf("마지막 글자: %s\n", last);


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

	if(addlen%3 == 0){	//3바이트
		sprintf(buf2, "%c%c%c", word[addlen-3], word[addlen-2], word[addlen-1]); //2바이트
	}
	if(addlen%3 == 1){
		sprintf(buf2, "%c%c%c", word[addlen-4], word[addlen-3], word[addlen-2]);
	}
}


char *check_the_rule(char buf3[], int i){
	
	//buf에 있는 user이름과 단어 분리하기
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

//강아지 -> 지구  강아지의 마지막 단어와 지구의 첫번째 단어 비교 

// 새로운 채팅 참가자 처리
void addClient(int s, struct sockaddr_in *newcliaddr) {
    char buf[20];
    inet_ntop(AF_INET,&newcliaddr->sin_addr,buf,sizeof(buf));
    printf("new client: %s\n",buf);
     // 채팅 클라이언트 목록에 추가
    clisock_list[num_chat] = s;
    num_chat++;
}
    // 채팅 탈퇴 처리
void removeClient(int i) {
    close(clisock_list[i]);
    if(i != num_chat-1)
        clisock_list[i] = clisock_list[num_chat-1];
    num_chat--;
    printf("채팅 참가자 1명 탈퇴. 현재 참가자 수 = %d\n", num_chat);
}

// 소켓이 nonblock 인지 확인
int is_nonblock(int sockfd) {
    int val;
    // 기존의 플래그 값을 얻어온다
    val=fcntl(sockfd, F_GETFL,0);
    // 넌블록 모드인지 확인
    if(val & O_NONBLOCK)
        return 0;
    return -1;
}

    // 소켓을 넌블록 모드로 설정
int set_nonblock(int sockfd) {
    int val;
    // 기존의 플래그 값을 얻어온다
    val=fcntl(sockfd, F_GETFL,0);
    if(fcntl(sockfd, F_SETFL, val | O_NONBLOCK) == -1)
        return -1;
    return 0;
}

// listen 소켓 생성 및 listen
int  tcp_listen(int host, int port, int backlog) {
    int sd;
    struct sockaddr_in servaddr;

    sd = socket(AF_INET, SOCK_STREAM, 0);
    if(sd == -1) {
        perror("socket fail");
        exit(1);
    }
    // servaddr 구조체의 내용 세팅
    bzero((char *)&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(host);
    servaddr.sin_port = htons(port);
    if (bind(sd , (struct sockaddr *)&servaddr, sizeof(servaddr))
        < 0) {
        perror("bind fail");  exit(1);
    }
    // 클라이언트로부터 연결요청을 기다림
    listen(sd, backlog);
    return sd;
}




