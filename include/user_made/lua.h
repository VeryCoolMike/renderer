#ifndef LUA_H
#define LUA_H

#include "structs.h"
#include "helper.h"
#include "objects.h"


extern std::vector<vertices> objList;

int lua_checkBool(lua_State *L, int narg)
{
    if (!lua_isboolean(L, narg)) {
        luaL_typeerror(L, narg, "boolean");
    }
    return lua_toboolean(L, narg);
}


int lua_getTime(lua_State *L)
{
    auto now = std::chrono::system_clock::now();
    auto now_ms = std::chrono::time_point_cast<std::chrono::milliseconds>(now);
    auto epoch = now_ms.time_since_epoch();
    auto value = std::chrono::duration_cast<std::chrono::milliseconds>(epoch);

    lua_pushinteger(L, value.count());

    return 1;
}

int lua_wait(lua_State *L)
{
    float length = luaL_optnumber(L, 1, 0.03);
    std::this_thread::sleep_for(std::chrono::milliseconds(int(length * 1000)));

    return 0;
}

// OBJECTS

int lua_instanceNew(lua_State *L)
{
    std::string instanceType = luaL_checkstring(L, 1);
    if (instanceType == "Object")
    {
        addObject(currentIDNumber, "Part", cubeObj, REGULAR);
    }
    else if (instanceType == "Light")
    {
        addObject(currentIDNumber, "Light", cubeObj, LIGHT);
    }

    LuaObject *udata = (LuaObject*)lua_newuserdata(L, sizeof(LuaObject));
    udata->obj = &objects.back();

    luaL_getmetatable(L, "LuaObject");
    lua_setmetatable(L, -2);
    std::cout << objects.back().vertices.name << std::endl;
    std::cout << udata->obj->vertices.name << std::endl;

    return 1;
}

int lua_getCameraPos(lua_State *L)
{
    lua_newtable(L);

    lua_pushnumber(L, cameraPos.x);
    lua_rawseti(L, -2, 1);

    lua_pushnumber(L, cameraPos.y);
    lua_rawseti(L, -2, 2);

    lua_pushnumber(L, cameraPos.z);
    lua_rawseti(L, -2, 3);

    return 1;
}

int lua_global_index(lua_State *L)
{
    std::string field = luaL_checkstring(L, 2);

    if (field == "cameraPos")
    {
        lua_newtable(L);

        lua_pushnumber(L, cameraPos.x);
        lua_rawseti(L, -2, 1);
        lua_pushnumber(L, cameraPos.y);
        lua_rawseti(L, -2, 2);
        lua_pushnumber(L, cameraPos.z);
        lua_rawseti(L, -2, 3);

        lua_pushnumber(L, cameraPos.x);
        lua_setfield(L, -2, "x");
        lua_pushnumber(L, cameraPos.y);
        lua_setfield(L, -2, "y");
        lua_pushnumber(L, cameraPos.z);
        lua_setfield(L, -2, "z");
        return 1;
    }

    std::cerr << error("LUA ERROR: Field provided is unkown: ") << field << std::endl;
    return 0;
}

int lua_global_new_index(lua_State *L)
{
    std::string field = luaL_checkstring(L, 2);

    if (field == "cameraPos")
    {
        if (!lua_isstring(L, 3)) // No other parameter given
        {
            std::cout << "hi guys" << std::endl;
            if (!lua_istable(L, 3))
            {
                std::cerr << error("LUA ERROR: Expected a table!") << std::endl;
                return 0;
            }

            lua_rawgeti(L, 3, 1);
            cameraPos.x = luaL_checknumber(L, -1);
            lua_pop(L, 1);

            lua_rawgeti(L, 3, 2);
            cameraPos.y = luaL_checknumber(L, -1);
            lua_pop(L, 1);

            lua_rawgeti(L, 3, 3);
            cameraPos.z = luaL_checknumber(L, -1);
            lua_pop(L, 1);

            return 0;
        }
        else
        {
            std::string field2 = luaL_checkstring(L, 3);
            std::cout << field2 << std::endl;
            std::cout << "hi guys 2" << std::endl;
        }
        
    }

    std::cerr << error("LUA ERROR: Field provided is unkown: ") << field << std::endl;
    return 0;
}

int lua_index(lua_State *L)
{
    LuaObject *udata = (LuaObject*)luaL_checkudata(L, 1, "LuaObject");
    std::string field = luaL_checkstring(L, 2);

    if (field == "name")
    {
        lua_pushstring(L, udata->obj->name.c_str());
        return 1;
    }
    else if (field == "id")
    {
        lua_pushinteger(L, udata->obj->id);
        return 1;
    }
    // Transforms
    else if (field == "pos")
    {
        lua_newtable(L);

        lua_pushnumber(L, udata->obj->transform.pos.x);
        lua_rawseti(L, -2, 1);

        lua_pushnumber(L, udata->obj->transform.pos.y);
        lua_rawseti(L, -2, 2);

        lua_pushnumber(L, udata->obj->transform.pos.z);
        lua_rawseti(L, -2, 3);

        return 1;
    }
    else if (field == "rot")
    {
        lua_newtable(L);

        lua_pushnumber(L, udata->obj->transform.rot.x);
        lua_rawseti(L, -2, 1);

        lua_pushnumber(L, udata->obj->transform.rot.y);
        lua_rawseti(L, -2, 2);

        lua_pushnumber(L, udata->obj->transform.rot.z);
        lua_rawseti(L, -2, 3);

        return 1;
    }
    else if (field == "scale")
    {
        lua_newtable(L);

        lua_pushnumber(L, udata->obj->transform.scale.x);
        lua_rawseti(L, -2, 1);

        lua_pushnumber(L, udata->obj->transform.scale.y);
        lua_rawseti(L, -2, 2);

        lua_pushnumber(L, udata->obj->transform.scale.z);
        lua_rawseti(L, -2, 3);

        return 1;
    }

    if (udata->obj->objectType == LIGHT) // The object is a light
    {
        int index = -1;
        for (int i = 0; i < lightArray.size(); i++)
        {
            if (lightArray[i].id == udata->obj->id)
            {
                index = i;
                break;
            }
        }
        
        if (index != -1) // Make sure that the light exists
        {
            if (field == "lightStrength")
            {
                lua_pushnumber(L, lightArray[index].strength);
            }
        }
    }
    
    std::cerr << error("LUA ERROR: Field provided is unkown: ") << field << std::endl;
    return 0;
}

int lua_newIndex(lua_State *L)
{
    LuaObject *udata = (LuaObject*)luaL_checkudata(L, 1, "LuaObject");
    std::string field = luaL_checkstring(L, 2); // The 
    

    // General things (Every object should have this)
    if (field == "name")
    {
        std::string value = luaL_checkstring(L, 3);
        udata->obj->name = value;
        return 0;
    }
    else if (field == "id")
    {
        std::cerr << error("LUA ERROR: Unable to change ID manually!") << std::endl;
        return 0;
    } 
    // Transforms
    else if (field == "pos")
    {
        if (!lua_istable(L, 3))
        {
            std::cerr << error("LUA ERROR: Expected a table!") << std::endl;
            return 0;
        }

        lua_rawgeti(L, 3, 1);
        udata->obj->transform.pos.x = luaL_checknumber(L, -1);
        lua_pop(L, 1);

        lua_rawgeti(L, 3, 2);
        udata->obj->transform.pos.y = luaL_checknumber(L, -1);
        lua_pop(L, 1);

        lua_rawgeti(L, 3, 3);
        udata->obj->transform.pos.z = luaL_checknumber(L, -1);
        lua_pop(L, 1);

        return 0;
    }
    else if (field == "rot")
    {
        if (!lua_istable(L, 3))
        {
            std::cerr << error("LUA ERROR: Expected a table!") << std::endl;
            return 0;
        }

        lua_rawgeti(L, 3, 1);
        udata->obj->transform.rot.x = luaL_checknumber(L, -1);
        lua_pop(L, 1);

        lua_rawgeti(L, 3, 2);
        udata->obj->transform.rot.y = luaL_checknumber(L, -1);
        lua_pop(L, 1);

        lua_rawgeti(L, 3, 3);
        udata->obj->transform.rot.z = luaL_checknumber(L, -1);
        lua_pop(L, 1);

        return 0;
    }
    else if (field == "scale")
    {
        if (!lua_istable(L, 3))
        {
            std::cerr << error("LUA ERROR: Expected a table!") << std::endl;
            return 0;
        }

        lua_rawgeti(L, 3, 1);
        udata->obj->transform.scale.x = luaL_checknumber(L, -1);
        lua_pop(L, 1);

        lua_rawgeti(L, 3, 2);
        udata->obj->transform.scale.y = luaL_checknumber(L, -1);
        lua_pop(L, 1);

        lua_rawgeti(L, 3, 3);
        udata->obj->transform.scale.z = luaL_checknumber(L, -1);
        lua_pop(L, 1);

        return 0;
    }
    else if (field == "model")
    {
        std::string name = luaL_checkstring(L, 3);
        for (int i = 0; i < objList.size(); i++)
        {
            if (objList[i].name == name)
            {
                for (int v = 0; v < objects.size(); v++)
                {
                    if (udata->obj->id == objects[v].id)
                    {
                        updateVertices(objList[i], objects[v]);
                        return 0;
                    }
                }
                
            }
        }
        std::cerr << error("LUA ERROR: Unable to find object") << std::endl;
        return 0;
    }

    if (udata->obj->objectType == LIGHT) // The object is a light
    {
        int index = -1;
        for (int i = 0; i < lightArray.size(); i++)
        {
            if (lightArray[i].id == udata->obj->id)
            {
                index = i;
                break;
            }
        }
        
        if (index != -1) // Make sure that the light exists
        {
            if (field == "lightStrength")
            {
                float value = luaL_checknumber(L, 3);
                lightArray[index].strength = value;
                return 0;
            }
        }
        
    }

    
    std::cerr << error("LUA ERROR: Field provided is unkown: ") << field << std::endl;
    return 0;
}

void registerLuaFunctions(lua_State *L)
{
    // General functions
    lua_register(L, "getTime", lua_getTime);
    lua_register(L, "wait", lua_wait);
    lua_register(L, "getCameraPos", lua_getCameraPos);


    // Instances
    lua_newtable(L);
    lua_pushcfunction(L, lua_instanceNew);
    lua_setfield(L, -2, "new");
    lua_setglobal(L, "Instance");

    // Metatables
    luaL_newmetatable(L, "LuaObject");
    lua_pushcfunction(L, lua_index);
    lua_setfield(L, -2, "__index");

    lua_pushcfunction(L, lua_newIndex);
    lua_setfield(L, -2, "__newindex");

    lua_pop(L, 1);
    
    // Create the globals table
    lua_newtable(L);

    // Create a metatable for it
    lua_newtable(L);
    lua_pushcfunction(L, lua_global_index);
    lua_setfield(L, -2, "__index");  

    lua_pushcfunction(L, lua_global_new_index);
    lua_setfield(L, -2, "__newindex");  

    // Set the metatable
    lua_setmetatable(L, -2);

    // Now set as global
    lua_setglobal(L, "globals");
}


void RunScript(const std::string& script)
{
    
    lua_State *L = luaL_newstate();
    // Register custom functions
    
    registerLuaFunctions(L);
    
    luaL_openlibs(L);

    if (luaL_dofile(L, script.c_str()) != LUA_OK)
    {
        std::cerr << error("Error loading script: ") << error(lua_tostring(L, -1)) << std::endl;
        lua_pop(L, 1);
        return;
    }

    

    lua_close(L);
}

#endif // LUA_H