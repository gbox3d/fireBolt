# redSkeleton (Express)


## 사용법
반드시 클로닝 이후에 npm install을 한번 실행해주세요.  
sample.config.env 를 config.en 로 바꿔주세요.


```sh
npm install # 처음 한번만
cp sample.config.env config.env

npm start # 배포용 실핼
npm run dev # 개발용 실행 

# pm2로 실행
pm2 start npm --time --name 'nutserver' -- start
pm2 start npm --time --name 'nutserver_dev' -- run dev
```

## api auth 인증방법
auth 인증토큰을 헤더에 전달한다.  
```
auth-token : 5874  
```







