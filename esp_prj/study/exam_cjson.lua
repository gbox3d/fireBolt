--
-- Created by IntelliJ IDEA.
-- User: gbox3d
-- Date: 15. 6. 12.
-- Time: 오후 1:08
-- To change this template use File | Settings | File Templates.
--


local json_obj = cjson.decode([[{"test":"simple"}]])
print(json_obj.test)

local lua_table = {}
lua_table.test = 'sample'
print(cjson.encode(lua_table))