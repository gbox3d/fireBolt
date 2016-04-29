--
-- Created by IntelliJ IDEA.
-- User: gunpower
-- Date: 2016. 4. 29.
-- Time: 오전 8:03
-- To change this template use File | Settings | File Templates.
--
dofile("stepper.lua")

print('step moter start')

stepper_move(1024,(1),1,function() print('end of step moter move') end)


