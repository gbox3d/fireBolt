# web api

## 사용 형식
```
접속주소/json?data=json문자열
```

## 기본 기능

### 파일읽기
```
fn='rf',name=,[offset=]
name : 파일이름
offset : 읽기 시작할위치 생략되면 처음부터

ex>
{
  fn : 'rf',
  name : 'config.json'
}
```
### 파일쓰기
```
fn='rw',fg='',name='',content=''
```