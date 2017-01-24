--
-- Created by IntelliJ IDEA.
-- User: gunpower
-- Date: 2016. 3. 3.
-- Time: 오후 11:20
-- To change this template use File | Settings | File Templates.
--


strTemp = string.char(91,92,93,94,95)
--전체 어레이로 바꿔넣기
tempArray = {strTemp:byte(1,-1) }
--하나씩출력해보기
for i = 1, table.getn(tempArray) do    print(tempArray[i] .. ',') end