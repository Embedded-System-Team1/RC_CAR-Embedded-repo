## ✏️ 목차
1. [🎯 범용 RC카 프로젝트 소개](#범용-RC카-프로젝트-소개)
2. [📋 주요 기능](#-주요-기능)
3. [🚚임베디드 설계도](#임베디드-설계도)
4. [📊 범용 RC카 프로젝트 구성도](#범용-RC카-프로젝트-구성도)
5. [🛠️ 동시성 제어 관련 제한조건 구현 내용](#%EF%B8%8F동시성-제어-관련-제한조건-구현-내용)
6. [✨ 프로젝트 파일 구성](#프로젝트-파일-구성)
7. [🐛 개발시 문제점과 해결 (트러블 슈팅)](#개발시-문제점과-해결-트러블-슈팅)
8. [📝 시스템 커스텀 가이드](#시스템-커스텀-가이드)
   - [device 확장 가이드](#device-확장-가이드)
   - [새로운 모터 타입 추가 방법](#새로운-모터-타입-추가-방법)
9. [👬 팀원 소개 및 역할](#팀원-소개-및-역할)


#### 시연 링크
[RC카 프로젝트 시연](https://youtu.be/mZh_EuVhqVA)

#### 노션 페이지
[프로젝트 노션 페이지](https://standing-wall-902.notion.site/13506274252c8007b0b7d42e69015947?pvs=4)

----


### 🎯범용 RC카 프로젝트 소개
<img src="https://github.com/user-attachments/assets/37f378a7-bd56-44f7-a687-448247db18c6" alt="RC 카 이미지" width="300px">


RC 카의 제어와 기기 추가를 좀 더 확장하기 쉽게 하기 위해 제작되었습니다.
이 프로젝트는 아래와 같은 목적을 가지고 있습니다:

- **모터 및 제어 장치 확장**: DC 모터, 서보 모터 등 다양한 임베디드 제어 장치를 쉽게 추가하고, 사용자 맞춤 제어를 가능하게 함.
- **제어 방법의 커스터마이징**: WebSocket API, Bluetooth 등을 통해 사용자가 다양한 방식으로 RC 카를 제어할 수 있도록 유연성을 제공.
- **확장성과 안정성 강화**: 프로세스 간 통신과 동시성 제어를 통해 안정적이며 확장 가능한 시스템 구축.

---

### 📋 주요 기능

#### RC카 device 기능
1. **DC 모터를 활용한 전진 및 후진 방향 제어**  
   - RC카의 전진 및 후진 방향을 DC 모터로 정밀하게 제어합니다.

2. **서보 모터를 이용한 오픈카 모드**  
   - 서보 모터를 활용하여 차량의 오픈카 모드를 구현합니다.

3. **조도 센서 및 LED를 활용한 오토라이트 기능**  
   - 주변의 밝기를 감지하여 자동으로 라이트를 켜고 끄는 기능입니다.

4. **스피커를 활용한 경적 기능**  
   - 스피커를 통해 RC카에 경적 소리를 지원합니다.


#### 유연한 제어 방법

RC카는 다양한 인터페이스를 지원하여 유연한 제어가 가능합니다.

- **WebSocket API** : 웹(Web) 환경을 통해 실시간 제어 가능  
- **Bluetooth** : 모바일 환경에서 Bluetooth를 통해 제어 가능  

---

### 🚚임베디드 설계도
<img src="https://github.com/user-attachments/assets/50061b17-c480-43a7-9147-f668398c2860" alt="image" width="500">

**구성 설명**

**1. 건전지 스펙**
전원 공급:
RC 카의 모터 제어및 서보 모터, 라즈베리파이 보드 전원 공급에 사용
갯수 : 2개
전압: 3.7V 
용량: 2000mAh

**2. UART (블루투스 통신)**
역할:
블루투스 모듈과 라즈베리파이 간 통신을 통해 RC 카의 원격 제어수행
활용: 스마트폰 등 외부 장치에서 조종 명령을 전송

**3. 서보 모터 (Servo Motor)**
역할:
오픈 카 모드 제어
RC 카의 각도 및 조향 방향을 세밀하게 조정

**4. DC 모터와 어댑터**
역할:
RC 카의 구동 및 방향 제어
모터 드라이버를 통해 속도와 방향을 조절
2개의 DC 모터를 사용하여 RC 카를 움직임

**5. LED 핀과 조도 센서**
역할:
주변 조도 감지를 통해 오토라이트 모드 구현
조도 센서가 어두운 환경을 감지하면 LED가 자동으로 켜짐


**6. 스피커**
역할:
RC 카 경적을 내도록 함

---


### 📊범용 RC카 프로젝트 구성도
![image](https://github.com/user-attachments/assets/ac4d1f2b-d078-41f9-822e-03cd9b45591d)

----
1. Controller
RC 카의 동작을 제어하며, 현재 등록된 Device 를 제어합니다. Thread Pool을 사용해 병렬 작업을 수행하며, 효율적인 리소스 활용과 빠른 응답성을 보장합니다.

2. Connection Handler
외부 입력(Bluetooth, WebSocket, Remote)을 처리하며, 동시 연결 시 경쟁 조건 방지를 위해 Mutex Lock을 사용해 안정성을 보장합니다.

3. IPC (Message Queue)
프로세스 간 데이터 교환을 위한 비동기 데이터 전달 역할을 수행합니다. 데이터를 FIFO 방식으로 처리하며, Connection Handler와 Controller 간의 의사소통을 중재합니다.

4. Thread Pool
Controller와 Connection Handler 모두에서 작업을 병렬로 처리하며, 리소스 최적화 및 성능 향상을 도모합니다.

5. 외부 제어 소프트웨어
WebSocket API와 Bluetooth 모듈을 통해 사용자가 실시간으로 RC 카를 제어할 수 있습니다.

---

### 🛠️동시성 제어 관련 제한조건 구현 내용
![image](https://github.com/user-attachments/assets/816f6019-7554-4db1-876e-a28754b435e0)
1. IPC 메시지 큐
   
   역할: 프로세스 간 메시지 전달 (Handler → Controller). 
   
   특징:
   비동기 데이터 처리
   안정적 프로세스 통신

2. 쓰레드 풀

   구성:
   Controller: Device Worker Thread Pool
   Handler: Connection Worker Thread Pool </br>
   
   이유: 
   실시간 처리 강화
   리소스 효율화 (스레드 재사용) :  Device 제어 이벤트 마다 쓰레드를 생성 및 종료하면 리소스와 성능에 부담이 있어서 프로그램 시작시 쓰레드를 생성하고 재사용하게끔 설계
   확장성 유지 : 디바이스와 제어 장치가 추가되면 해당 쓰레드를 더 생성하면 됨

4. 커넥션 동기화
   
   문제: 커넥션 다중 지원 상황에서 제어 명령에 대한 경쟁 조건이 발생함, 제어 소프트웨어 간에 동기화 필요.
   
   해결: 
   Mutex Lock 사용 → 연결 단위 동기화 보장


---

### ✨프로젝트 파일 구성
**1. main.c**
   프로그램의 시작점이자 root process
   
   프로그램이 시작되면 child process 로  
   controller.c 와 handler.c를 fork spawn을 실행합니다.

**2. handler.c**

   RC카를 직접 제어하거나 웹 통신 블루투스 통신 등등
   RC카 제어를 위한 connection 을 핸들링 함
   
   connection에서 보내진 데이터를 IPC로 보내는 파이프라인 역할을 함

**3. controller.c**

   메세지 큐의 데이터를 polling 하여 device 장치를 제어하도록 실행함

**4. device_adaptor.c** 

   connection에서 보낸 데이터를 parsing 하여 적합한 device worker thread 에 동작하게 한다.
   device_adaptor 에 제공된 규악을 device 실행 메서드와 구조체 , 문자열 구조체 변환 메서드를 커스텀하면
   해당 device를 실행할 수 있다.


---

### 🐛개발시 문제점과 해결 (트러블 슈팅)

#### **1. 유연한 확장 기능 지원**
   
   문제
   
   정형화된 데이터 형식 필요
   
   해결 방안
   
   JSON ↔ 구조체 변환
   데이터 표준화 및 유연성 확보

#### **2. DEVICE 제어 비용 최적화**
   
   문제
   
   FORK 비용 및 CONTEXT SWITCHING 부담
   
   해결 방안
   
   THREAD 기반 제어 채택
   THREAD POOL 패턴 적용
   스레드 생성/종료 비용 최소화

#### **3. 커넥션 동기화**
   
   문제
   
   여러 커넥션이 동시에 접근 시 경쟁 조건 발생
   
   해결 방안
   
   MUTEX LOCK 사용
   커넥션의 연결 단위로 동기화
   데이터 정합성 및 연결 안정성 보장

#### **4. 웹 기반 응답률 최적화**
   
   문제
   
   HTTP 방식은 신뢰성과 안정성이 높지만, 메시지 단위가 무겁고 느리다는 단점이 존재
   
   해결 방안
   
   웹 소켓 프로토콜 채택
   신뢰성 있는 HTTP 위에 빠르고 작은 통신 가능


---


## 📝시스템 커스텀 가이드

### device 확장 가이드

---

### **새로운 모터 타입 추가 방법**

새로운 모터 타입을 추가하려면 다음 작업이 필요합니다:
1. **명령 구조체**를 정의합니다.  
   - 구조체에는 **반드시 `id` 필드**를 포함해야 합니다.
   - json 문자열을 구조체로 변경하는 함수를 구현합니다.
2. **명령 처리 함수**를 구현합니다.
3. **모터 핸들러**를 프레임워크에 등록합니다.

---

### **단계별 가이드**
- 확장할 device 관련 커스텀 함수를 정의할 파일을 생성합니다 (이번 예시는 survo.c 을 생성했다고 가정)

#### **1. 명령 구조체 정의하기**

먼저, survo.c 에 새로운 모터 타입의 명령 구조체를 정의합니다.  
구조체에는 **반드시 `id` 필드**가 포함되어야 하며, 모터의 동작에 필요한 다른 필드를 추가로 정의합니다.

```c
typedef struct {
    int id;         // 모터 고유 ID (필수 필드)
    int angle;      // 서보 모터 회전 각도
    int duration;   // 동작 지속 시간 (ms 단위)
} ServoMotorCommand;
```

**json 문자열을 구조체로 변경하는 함수를 작성합니다**
- survo.c 에 문자열을 구조체로 변경하는 메서드를 작성합니다
- dc_motor.c 파일에 구조체 변경 함수 예시가 작성되어 있습니다!
```c
void* parse_servo_motor_message(const char* json_str) {
}

```
#### 2. **명령 처리 함수**를 구현합니다.
- 아래는 예제입니다.
-  survo.c 에 명령 처리 메서드를 작성합니다
```c
void handle_servo_motor_command(void* command) {
    ServoMotorCommand* servo_command = (ServoMotorCommand*)command;
    printf("서보 모터 제어 - ID: %d, 각도: %d, 지속 시간: %dms\n", 
           servo_command->id, 
           servo_command->angle, 
           servo_command->duration);
}
```
#### 3. **모터 핸들러**를 프레임워크에 등록합니다.
- device_adaptor.c 파일에 해당 함수에 다음과 같은 코드를 추가합니다

```c
#include "survo.c" // device_adaptor.c 파일에 추가 

void register_custom_device_handler() { // 구현되어 있는 함수에 다음 코드를 추가
    MotorHandler servo_motor_handler = {
        .handle_command = handle_servo_motor_command,
        .parse_message = parse_servo_motor_message,
    };
    register_motor_handler(1, servo_motor_handler); // 모터 ID 1에 핸들러 등록
}
```

#### 컴파일

```bash
 gcc -o controller controller.c -lpthread -lwiringPi -ljansson
```

#### 실행

```bash
gcc -o controller controller.c -lpthread -lwiringPi -ljansson
gcc -o handler handler.c -lpthread -lwiringPi -ljansson -lwebsockets
sudo ./main
```


### 👬팀원 소개 및 역할

<div align="center">
  <table>
    <tr>
      <th>Profile</th>
      <th>Role</th>
    </tr>
    <tr>
      <td align="center">
        <a href="https://github.com/kang20">
          <img src="https://avatars.githubusercontent.com/u/75325326?v=4" width="100" height="80" alt=""/>
          <br/>
          <sub><b>강민기</b></sub>
        </a>
      </td>
      <td>
        전체적인 프로젝트 구성 설계, 제어 소프트웨어와 모터 사이의 파이프라인 개발, 동시성 처리 구현
      </td>
   </tr>
   <tr>
      <td align="center">
        <a href="https://github.com/Jinoko01">
          <img src="https://avatars.githubusercontent.com/u/126740959?v=4" width="100" height="80" alt=""/>
          <br/>
          <sub><b>황용진</b></sub>
        </a>
      </td>
      <td>
        웹과 라즈베리파이 간 웹소켓 통신을 위한 웹 플랫폼 개발, RC카 dc모터 조립, RC카와 핸들러간 데이터 통신 테스트 진행
      </td>
   </tr>
   <tr>
      <td align="center">
        <a href="https://github.com/combikms">
          <img src="https://avatars.githubusercontent.com/u/156290648?v=4" width="100" height="80" alt=""/>
          <br/>
          <sub><b>강인석</b></sub>
        </a>
      </td>
      <td>
        라즈베리파이5와 스마트폰 간 블루투스 통신을 위한 모바일 애플리케이션 개발, 부가 기능 확장
      </td>
   </tr>
     <tr>
      <td align="center">
        <a href="https://github.com/jungbk0808">
          <img src="https://avatars.githubusercontent.com/u/120279225?v=4" width="100" height="80" alt=""/>
          <br/>
          <sub><b>정보경</b></sub>
        </a>
      </td>
      <td>
        RC카 전체 조립 및 device 단의 조작 소프트웨어 개발, 부가 기능 확장, RC카와 핸들러간 데이터 통신 테스트 진행
      </td>
   </tr>
  </table>
</div>
<br>
