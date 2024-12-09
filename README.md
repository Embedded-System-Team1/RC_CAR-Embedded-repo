### 🎯범용 RC카 프로젝트 소개

RC 카의 제어와 기기 추가를 좀 더 확장하기 쉽게 하기 위해 제작되었습니다.
이 프로젝트는 아래와 같은 목적을 가지고 있습니다:

- **모터 및 제어 장치 확장**: DC 모터, 서보 모터 등 다양한 임베디드 제어 장치를 쉽게 추가하고, 사용자 맞춤 제어를 가능하게 함.
- **제어 방법의 커스터마이징**: WebSocket API, Bluetooth 등을 통해 사용자가 다양한 방식으로 RC 카를 제어할 수 있도록 유연성을 제공.
- **확장성과 안정성 강화**: 프로세스 간 통신과 동시성 제어를 통해 안정적이며 확장 가능한 시스템 구축.


### 📊 범용 RC 카 프로젝트 구성도
![image](https://github.com/user-attachments/assets/81e5fe47-def0-42ed-826a-f32e7bb2c866)
----
1. Controller
RC 카의 동작을 제어하며, DC 모터와 서보 모터의 속도 및 방향을 관리합니다. Thread Pool을 사용해 병렬 작업을 수행하며, 효율적인 리소스 활용과 빠른 응답성을 보장합니다.

2. Connection Handler
외부 입력(Bluetooth, WebSocket, Remote)을 처리하며, 동시 연결 시 경쟁 조건 방지를 위해 Mutex Lock을 사용해 안정성을 보장합니다.

3. IPC (Message Queue)
프로세스 간 데이터 교환을 위한 비동기 데이터 전달 역할을 수행합니다. 데이터를 FIFO 방식으로 처리하며, Connection Handler와 Controller 간의 의사소통을 중재합니다.

4. Thread Pool
Controller와 Connection Handler 모두에서 작업을 병렬로 처리하며, 리소스 최적화 및 성능 향상을 도모합니다.

5. 외부 제어 소프트웨어
WebSocket API와 Bluetooth 모듈을 통해 사용자가 실시간으로 RC 카를 제어할 수 있습니다.
----
동시성 제어
- Mutex Lock: Connection Handler에서 다중 연결 경쟁 조건 방지를 위해 사용.
- Thread Pool: 병렬 작업 처리로 자원 소모를 최소화하며 시스템 안정성을 높임.
- Message Queue: 비동기 메시지 전달로 데이터의 무결성을 유지함.

---

### 🛠️임베디드 소프트웨어 구성
![image](https://github.com/user-attachments/assets/816f6019-7554-4db1-876e-a28754b435e0)

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

**5. worker thread pool** 

   connection 과 device 제어는 비동기 식 실행을 위해 멀티 스레딩을 채택함
   장치 제어와 connection 은 real time 으로 작업이 실행되기 때문에 작업 단위마다 thread를 생성-종료 구조는 비적합
   controller 와 handler 프로세스가 실행될 때 thread 를 생성해놓고 thread pool 로 적재하여 리소스를 효율화 시킴

**6. concurrency control (connection multi thread)**

   connection 끼리 데이터 전송에서 한 메시지 단위로 동기화가 보장되어야 한다.
   
   각 connection 의 데이터 전송 transaction 의 동기화를 보장한다.
   
   handler.c 에서 mutex_lock 을 이용하여 동기화를 보장한다.

 



## 📝 시스템 커스텀 가이드

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

}
```

#### 컴파일

```bash
 gcc -o controller controller.c -lpthread -lwiringPi -ljansson
```

#### 실행

```bash
gcc -o controller controller.c -lpthread -lwiringPi -ljansson
gcc -o handler handler.c -lpthread -lwiringPi -ljansson
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
        전체적인 프로젝트 구성 설계, 제어 소프트웨어와 모터 사이의 파이프라인 개발
      </td>
  </table>
</div>
<br>
