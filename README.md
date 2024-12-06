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



### 🛠️임베디드 소프트웨어 흐름도 및 구성도




### 📝 시스템 커스텀 가이드




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
