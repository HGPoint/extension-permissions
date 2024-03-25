#define EXTENSION_NAME PermissionsExt
#define LIB_NAME "Permissions"
#define MODULE_NAME "permissions"

#include <dmsdk/sdk.h>

#if defined(DM_PLATFORM_ANDROID)

#include "permissions_private.h"
#include "permissions_callback_private.h"

namespace dmPermissions {

    static int Lua_Check(lua_State* L)
    {
        DM_LUA_STACK_CHECK(L, 1);
        if (lua_type(L, 1) != LUA_TSTRING) {
            return DM_LUA_ERROR("Expected string, got %s. Wrong type for permission variable '%s'.", luaL_typename(L, 1), lua_tostring(L, 1));
        }
        int result = Check(luaL_checkstring(L, 1));
        lua_pushinteger(L, result);
        return 1;
    }

    static int Lua_Request(lua_State* L)
    {
        DM_LUA_STACK_CHECK(L, 0);
        if (lua_type(L, 1) != LUA_TTABLE) {
            return DM_LUA_ERROR("Expected table, got %s. Wrong type for permissions variable '%s'.", luaL_typename(L, 1), lua_tostring(L, 1));
        }
        if (lua_type(L, 2) != LUA_TFUNCTION) {
            return DM_LUA_ERROR("Expected function, got %s. Wrong type for permissions variable '%s'.", luaL_typename(L, 2), lua_tostring(L, 2));
        }
        int len = lua_objlen(L, 1);
        const char *permissions[len];
        for (int i = 0; i < len; i++)
        {
            lua_rawgeti(L, 1, i + 1);
            permissions[i] = lua_tostring(L, -1);
            lua_pop(L, 1);
        }
        SetLuaCallback(L, 2);
        Request(permissions, len);
        return 1;
    }

    static int Lua_OpenNotificationsSettings(lua_State* L)
    {
        DM_LUA_STACK_CHECK(L, 0);
        OpenNotificationsSettings();
        return 0;
    }

    static const luaL_reg Module_methods[] =
    {
        {"check", Lua_Check},
        {"request", Lua_Request},
        {"open_notifications_settings", Lua_OpenNotificationsSettings},
        {0, 0}
    };

    static void LuaInit(lua_State* L)
    {
        DM_LUA_STACK_CHECK(L, 0);
        luaL_register(L, MODULE_NAME, Module_methods);

        #define SETCONSTANT(name) \
        lua_pushnumber(L, (lua_Number) name); \
        lua_setfield(L, -2, #name); \

        SETCONSTANT(PERMISSION_GRANTED)
        SETCONSTANT(PERMISSION_DENIED)
        SETCONSTANT(PERMISSION_SHOW_RATIONALE)
        
        lua_pop(L, 1);
    }

    static dmExtension::Result Initialize(dmExtension::Params* params)
    {
        LuaInit(params->m_L);
        Initialize_Ext();
        InitializeCallback();
        return dmExtension::RESULT_OK;
    }

    static dmExtension::Result Update(dmExtension::Params* params)
    {
        UpdateCallback();
        return dmExtension::RESULT_OK;
    }

    static dmExtension::Result Finalize(dmExtension::Params* params)
    {
        FinalizeCallback();
        return dmExtension::RESULT_OK;
    }

} //namespace

DM_DECLARE_EXTENSION(EXTENSION_NAME, LIB_NAME, 0, 0, dmPermissions::Initialize, dmPermissions::Update, 0, dmPermissions::Finalize)

#else

static  dmExtension::Result InitializePermissions(dmExtension::Params* params)
{
    dmLogInfo("Registered extension %s (null)", LIB_NAME);
    return dmExtension::RESULT_OK;
}

static dmExtension::Result FinalizePermissions(dmExtension::Params* params)
{
    return dmExtension::RESULT_OK;
}

DM_DECLARE_EXTENSION(EXTENSION_NAME, LIB_NAME, 0, 0, InitializePermissions, 0, 0, FinalizePermissions)

#endif // DM_PLATFORM_ANDROID
