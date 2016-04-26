--
-- Created by IntelliJ IDEA.
-- User: gunpower
-- Date: 2016. 3. 3.
-- Time: 오전 12:38
-- To change this template use File | Settings | File Templates.
--

strTemp = string.char(151,152,0,0,0,153,154,155)
print( strTemp:byte(1,8) ) --1은 시작 , 6은 길이

--중간 3개바이트 교체하기
strTemp = strTemp:sub(1,2) .. string.char(161,162,163) .. strTemp:sub(-3,-1)
print( strTemp:byte(1,8) )
