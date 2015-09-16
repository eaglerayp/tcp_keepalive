# TCP keepalive:
### TCP keepalive:  參考[TCP Keepalive HOWTO ](http://www.tldp.org/HOWTO/html_single/TCP-Keepalive-HOWTO/)    
於RFC1122 TCP內建的option,由c socket或kernel設置參數:
* TCP_KEEPCNT  - tcp_keepalive_probes (重試幾次決定斷線)
* TCP_KEEPINTVL - tcp_keepalive_intvl(ACK傳了多久沒回應要重試)
* TCP_KEEPIDLE - tcp_keepalive_time (閒置多久時間開始傳ACK)  
一般linux是預設socket不使用keepalive的,所以要在code開啟socket時就設定(類似example setsockopt),linux判定斷線之後kernel內部直接關閉連結,在上層的應用無法callback處理.

### Heartbeat:  
之前常用在即時通訊(IM)等服務, 在應用層利用TCP packet自訂的protocol.  
利用TCP echo 封包來判定連線狀態,普遍傳遞時間很密集(可能幾分鐘就傳一次),自行判定斷線再呼叫關閉連線的後續程序.  
在傳輸過程需要偷帶一些data或者是使用者斷線後需要做反應的情況下使用.

### 測試並觀察tcp keepalive 運作模式
以兩台device觀察比較容易製造斷線的情況,不然就需要防火牆設定
* 利用tcpdump/wireshark ,netcat 
* nc -l之後以socket client 連入   
* 開socket server運行後以nc連入
* ps -aux | grep tcp_ka    observe  child process pattern
* sudo lsof -i | grep [port]  observerconnection state


### the tcp_keepalive_client program

You run this program as:
```
    ./tcp_ka_client [-c CNT] [-d IDLE] [-i INTVL] HOST:PORT
    ./tcp_ka_client -c 3 -d 10 -i 5 192.168.2.95:8050
```
The program creates a TCP socket and sets the `SO_KEEPALIVE` socket option to
`1`.  If any of the "-c", "-d", and "-i" options are specified, then the
`TCP_KEEPCNT`, `TCP_KEEPIDLE`, and `TCP_KEEPINTVL` socket options are set on the
socket to the respective option arguments.  Then the program connects to
HOST:PORT and enters a loop between calling poll(2) for the POLLIN event and
reading data.


### example pattern
![test serverside tcp keepalive](https://github.com/eaglerayp/tcp_keepalive/blob/master/doc/client.png)  
```
    ./tcp_ka_client -c 3 -d 10 -i 5 192.168.2.95:8050
```
when 20 sec serverside D/C( no ack) , then send 3 probes with interval = 5sec .  Judge D/C @ 35s, send RST then close connection.


