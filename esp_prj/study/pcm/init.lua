function cb_drained(d)
    print("drained "..node.heap())

    file.seek("set", 0)
    -- uncomment the following line for continuous playback
    --d:play(pcm.RATE_8K)

    tmr.alarm(0, 2000, tmr.ALARM_SINGLE, function()
        play_count = play_count+1
        print(play_count)
        if play_count > 5 then tmr.stop(0) drv:close() print('close ')
        else
            print("hey there")
            drv:play(pcm.RATE_8K)
        end
    end)


end

function cb_stopped(d)
    print("playback stopped")
    file.seek("set", 0)
end

function cb_paused(d)
    print("playback paused")
end

file.open("ef01.u8", "r")

drv = pcm.new(pcm.SD, 1)

-- fetch data in chunks of LUA_BUFFERSIZE (1024) from file
drv:on("data", file.read)

-- get called back when all samples were read from the file
drv:on("drained", cb_drained)

drv:on("stopped", cb_stopped)
drv:on("paused", cb_paused)

play_count = 0;
-- start playback
drv:play(pcm.RATE_8K)


