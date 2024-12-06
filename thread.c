#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <wiringPi.h>

int num = 0;
pthread_mutex_t mid;
#define GPIO18 18

//뮤텍스 오브젝트
void *producerFunc(void *); //생산자 쓰레드 실행 함수
void *consumerFunc(void *); //소비자 쓰레드 실행 함수

//IN1(12) ~ IN4(21)
int pin_arr[4] = {12, 16, 20, 21};
int one_two_phase[8][4] = {{1,0,0,0},
						{1,1,0,0},
						{0,1,0,0},
						{0,1,1,0},
						{0,0,1,0},
						{0,0,1,1},
						{0,0,0,1}};

int one_two_Phase_Rotate(int steps, int dir){
	for(int i = 0; i<steps; i++){
		int phase = i%8;
		if(dir==0){
			phase = 8 - phase-1;
		}
		for(int j = 0; j < 4; j++){
			digitalWrite(pin_arr[j], one_two_phase[phase][j]);
		}

	delay(1);
	}	
	
}
void one_two_Phase_Rotate_Angle(float angle, int dir){
	int step = 4096/360*angle;
	one_two_Phase_Rotate(step,dir);
}
void init_Step()
{
	//4
	for(int i = 0 ; i< 4; i++)
	{
		pinMode(pin_arr[i], OUTPUT);
	}
}





int main(){
	wiringPiSetupGpio();	
	pinMode(GPIO18, INPUT); 
	init_Step();
	
	pthread_t ptProduc, ptConsum;
	pthread_mutex_init(&mid, NULL); // 뮤텍스 초기화
	pthread_create(&ptProduc, NULL, producerFunc, NULL);
	pthread_create(&ptConsum, NULL, consumerFunc, NULL);
	//생산자 쓰레드 생성
	//소비자 쓰레드 생성●
	pthread_join(ptProduc, NULL); //생산자 쓰레드 종료 대기
	pthread_join(ptConsum, NULL); //소비자 쓰레드 종료 대기
	pthread_mutex_destroy(&mid); // 뮤텍스 제거
	
	return 0;
}

void* producerFunc(void* arg){
	while(1){
		if(digitalRead(GPIO18) == 0){
			pthread_mutex_lock(&mid); // 뮤텍스 잠금
			printf("[신호] Producer : %d -> " , num);
			num++;
			printf(" %d \n", num);
			pthread_mutex_unlock(&mid); // 뮤텍스 잠금 해제
			delay(300);
		}
	}
	return NULL;
}
void* consumerFunc(void* arg){
	while(1){
		if(num >= 1){
			one_two_Phase_Rotate_Angle(360,1);
			pthread_mutex_lock(&mid); // 뮤텍스 잠금
			printf("[소비] Consumer : %d -> ",num);
			num--;
			printf(" %d \n", num);
			pthread_mutex_unlock(&mid); // 뮤텍스 잠금 해제
			delay(300);			
		}
	}
	return NULL;
}
