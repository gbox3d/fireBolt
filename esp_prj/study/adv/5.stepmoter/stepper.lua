--
-- Created by IntelliJ IDEA.
-- User: gunpower
-- Date: 2016. 4. 29.
-- Time: 오전 8:04
-- To change this template use File | Settings | File Templates.
--
-- stepper.lua
-- See more at: http://www.esp8266.com/viewtopic.php?f=19&t=2326#sthash.RX08Sdet.dpuf
-- simple stepper driver for controlling a stepper motor with a
-- l293d driver
-- esp-12 pins:  16 14 12 13
-- nodemcu pins:  0  5  6  7
local stepper_pins = {0,5,6,7}
-- half or full stepping
local step_states4 = {
    {1,0,0,1},
    {1,1,0,0},
    {0,1,1,0},
    {0,0,1,1}
}
local step_states8 = {
    {1,0,0,0},
    {1,1,0,0},
    {0,1,0,0},
    {0,1,1,0},
    {0,0,1,0},
    {0,0,1,1},
    {0,0,0,1},
    {1,0,0,1},
}

local step_states = step_states8 -- choose stepping mode
local step_numstates = 8 -- change to match number of rows in step_states

--step_states = step_states
--step_numstates = 4
local step_delay = 20 -- choose speed
local step_state = 0 -- updated by step_take-function
local step_direction = 1 -- choose step direction -1, 1
local step_stepsleft = 0 -- number of steps to move, will de decremented
local step_timerid = 4 -- which timer to use for the steps
local OnEndcallback = nil

-- setup pins
for i = 1, 4, 1 do
    gpio.mode(stepper_pins[i],gpio.OUTPUT)
end

-- turn off all pins to let motor rest
local function step_stopstate()
    for i = 1, 4, 1 do
        gpio.write(stepper_pins[i], 0)
    end
end

-- make stepper take one step
local function step_take()

    -- jump to the next state in the direction, wrap
    step_state = step_state + step_direction
    if step_state > step_numstates then
        step_state = 1;
    elseif step_state < 1 then
        step_state = step_numstates
    end

    --print("step " .. step_state)
    -- write the current state to the pins
    for i = 1, 4, 1 do
        gpio.write(stepper_pins[i], step_states[step_state][i])
    end

    -- might take another step after step_delay
    step_stepsleft = step_stepsleft-1
    if step_stepsleft > 0 then
        tmr.alarm(step_timerid, step_delay, 0, step_take )
    else
        step_stopstate()
        if(OnEndcallback) then OnEndcallback() end

    end
end

-- public method to start moving number of 'int steps' in 'int direction'
function stepper_move(steps, direction, delay,endCallback)
    tmr.stop(step_timerid)
    step_stepsleft = steps
    step_direction = direction
    step_delay = delay
    step_take()

    if(endCallback) then OnEndcallback = endCallback end;

end

-- public method to cancel moving
function stepper_stop()
    tmr.stop(step_timerid)
    step_stepsleft = 0
    step_stopstate()
end


