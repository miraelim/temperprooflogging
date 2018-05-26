#include <stdio.h>
#include <string.h>
#include <openssl/sha.h>
#include <sys/file.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <openssl/x509.h>
#include <openssl/hmac.h>
#include <openssl/evp.h>
//#include "tss2/TPM_Types.h"
#include <time.h>

#define BUFF_SIZE 2048

unsigned char digest[SHA256_DIGEST_LENGTH];
//const char* id = "MIRAE LIM";
//char rootkey[SHA256_DIGEST_LENGTH*2+1];
char rootkey[BUFF_SIZE];
char keyobject[BUFF_SIZE];
FILE  *fd, *kfd, *hmac, *temp1, *temporary, *keyobj;
int fd1, fd2;
char    temp[BUFF_SIZE];
char	hmac_log[BUFF_SIZE];
struct log{
    char* log;
    char* hmac_log;
};
float gap;
time_t starttime=0, endtime=0;


void makeRootkey(){
    system("openssl rand 256 >> key.txt");
}


void start(){
    kfd = fopen("key.txt", "r");
    char key[1024];
    if(kfd<0)
	printf("open kfd first fail\n");

    else{
	printf("open fd1 success\n");
	fscanf(kfd,"%s", key);
	printf("key\n");
    }
    system("./seal.sh");
    fclose(kfd);

}
void get_filelock(){

    fd1 = open("key.txt", O_RDWR);
    fd2 = open("weatherdatapoint.txt", O_RDWR);
    if(fd1<0)
	printf("open fd1 fail\n");

    else{
	printf("open fd1 success\n");
	read(fd1, keyobject, 2048);
    }

    //    printf("test1\n");
    if(fd2<0)
	printf("open fd2 fail\n");

    else{
	printf("open fd2 success\n");
	read(fd2, temp, 2048);
	printf("logs: %s\n",temp);
    }

    if (flock(fd1, LOCK_SH) == -1) {
	printf("key lock failed\n");
	exit(1);
    }
    else
	printf("key.txt lock success\n");

    if (flock(fd2, LOCK_SH) == -1) {
	printf("log lock failed\n");
	exit(1);
    }
    else
	printf("log.txt lock success\n");
    printf("fd in filelock:%d\n",fd2);
}

void get_fileunlock(){
    if (flock(fd1, LOCK_UN) == -1) {
	printf("keyobject unlock failed\n");
	exit(1);
    }
    else
	printf("keyobject.txt unlock success\n");

    if (flock(fd2, LOCK_UN) == -1) {
	printf("log unlock failed\n");
	exit(1);
    }
    else
	printf("log.txt unlock success\n");
    close(fd1);
    close(fd2);
}

void log_hmac(char *string){
    system("hmac256 \"key.txt\" \"temp1.txt\" >> temporary.txt");
    temporary = fopen("temporary.txt", "r");
    if(temporary<0)
	printf("temporary.txt fail\n");
    else
	printf("temporary.txt success\n");

    fscanf(temporary, "%s", hmac_log);
    printf("log hmac : %s\n",hmac_log);
    fprintf(hmac,"%s\t%s\n", string, hmac_log);
    fclose(temporary);
    system("rm temporary.txt");

}

void generate_newkey(){
     keyobj = fopen("keyobject.txt", "w");
           if(keyobj<0)
	         printf("keyobject.txt openfail\n");
              else
           printf("keyobj.txt open success\n");



}

int main(){
    char string[1024] ;

    starttime = clock();
    fd = fopen("weatherdatapoint.txt", "r");

    if(fd<0)
	printf("weatherdatapoint fail\n");
    else 
	printf("weatherdatapoint success\n");

    temp1 = fopen("temp1.txt", "w");

    if(temp1<0)
	printf("temp1.txt openfail\n");
    else
	printf("temp1 open success\n");

    hmac = fopen("hmac.txt", "a");

    if(hmac<0)
	printf("hmac open fail\n");
    else
	printf("hmac open success\n");

     keyobj = fopen("keyobject.txt", "w");
     if(keyobj<0)
       printf("keyobject.txt openfail\n");
        else
         printf("keyobj.txt open success\n");

    makeRootkey();
    get_filelock();

    while( fgets(string, 1024,fd)!= NULL){
	system("./unseal.sh");
	printf("%s\n",string);

	fprintf(temp1, "%s", string);

	log_hmac(string);
	//	generate_newkey();
	// save_keyobject();
	endtime = clock();
	gap = (float) (endtime - starttime)/(CLOCKS_PER_SEC);
	printf("time: %f\n",gap);
    }
    get_fileunlock();
//    generate_newkey();
    fclose(hmac);
    fclose(temp1);
    fclose(keyobj);
    return 0;
}
