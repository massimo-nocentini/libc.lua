
/*
    This is a glue c file for importing delta client c functions into Lua workflow.
*/
#define __USE_XOPEN
// #define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>
#include <lua.h>
#include <lauxlib.h>
#include <time.h>
#include <math.h>
#include <pthread.h>

typedef struct item_s
{
    lua_State *L;
    int idx;
} item_t;

int compare(const void *v, const void *w)
{

    item_t *vc = (item_t *)v;
    item_t *wc = (item_t *)w;

    lua_State *L = vc->L;

    lua_pushvalue(L, -1);
    lua_geti(L, -3, vc->idx);
    lua_geti(L, -4, wc->idx);
    lua_call(L, 2, 1);
    int comparison = lua_tointeger(L, -1);
    lua_pop(L, 1);

    return comparison;
}

int l_qsort(lua_State *L)
{

    /* Initial checks */
    assert(lua_istable(L, -2));
    assert(lua_isfunction(L, -1));

    int table_absidx = lua_absindex(L, -2);

    lua_len(L, table_absidx);       // push on the stack the number of elements to sort.
    int nel = lua_tointeger(L, -1); // get that number.
    lua_pop(L, 1);                  // clean the stack.

    item_t *permutation = (item_t *)malloc(sizeof(item_t) * nel);

    for (int i = 0; i < nel; i++)
    {

        item_t item;

        item.L = L;
        item.idx = i + 1;

        permutation[i] = item;
    }

    time_t starttime = time(NULL);
    qsort(permutation, nel, sizeof(item_t), compare);
    time_t endtime = time(NULL);

    lua_createtable(L, nel, 0); // the sorted table
    lua_createtable(L, nel, 0); // the sorting permutation

    for (int i = 0; i < nel; i++)
    {
        int idx = permutation[i].idx;

        lua_geti(L, table_absidx, idx);
        lua_seti(L, -3, i + 1);

        lua_pushinteger(L, idx); // to also provide the sorting permutation.
        lua_seti(L, -2, i + 1);
    }

    lua_pushinteger(L, endtime - starttime);

    free(permutation);

    return 3;
}

int compare_bsearch(const void *k, const void *v)
{

    item_t *key = (item_t *)k;
    item_t *each = (item_t *)v;

    lua_State *L = key->L;

    lua_pushvalue(L, -1);
    lua_pushvalue(L, key->idx);
    lua_geti(L, -5, each->idx);
    lua_call(L, 2, 1);
    int comparison = lua_tointeger(L, -1);
    lua_pop(L, 1);

    return comparison;
}

/* Do a binary search for KEY in BASE, which consists of NMEMB elements
   of SIZE bytes each, using COMPAR to perform the comparisons.  */
/*extern void *bsearch (const void *__key, const void *__base,
              size_t __nmemb, size_t __size, __compar_fn_t __compar) */

int l_bsearch(lua_State *L)
{

    /* Initial checks */
    assert(lua_istable(L, -3));
    assert(lua_isfunction(L, -1));

    int table_absidx = lua_absindex(L, -3);

    lua_len(L, table_absidx);       // push on the stack the number of elements to sort.
    int nel = lua_tointeger(L, -1); // get that number.
    lua_pop(L, 1);                  // clean the stack.

    item_t *permutation = (item_t *)malloc(sizeof(item_t) * nel);

    for (int i = 0; i < nel; i++)
    {

        item_t item;

        item.L = L;
        item.idx = i + 1;

        permutation[i] = item;
    }

    item_t key;
    key.L = L;
    key.idx = table_absidx + 1;

    item_t *found = (item_t *)bsearch(&key, permutation, nel, sizeof(item_t), compare_bsearch);

    if (found != NULL)
    {
        lua_geti(L, table_absidx, found->idx);
    }
    else
    {
        lua_pushnil(L);
    }

    free(permutation);

    return 1;
}

int l_l64a(lua_State *L)
{
    lua_Integer n = lua_tointeger(L, -1);

    const char *str = l64a(n);
    lua_pushstring(L, str);

    return 1;
}

int l_a64l(lua_State *L)
{

    const char *str = lua_tostring(L, -1);

    long n = a64l(str);
    lua_pushinteger(L, n);

    return 1;
}

int l_lldiv(lua_State *L)
{

    lua_Integer n = lua_tointeger(L, -2);
    lua_Integer m = lua_tointeger(L, -1);

    lldiv_t d = lldiv(n, m);

    lua_pushinteger(L, d.quot);
    lua_pushinteger(L, d.rem);

    return 2;
}

int l_strcmp(lua_State *L)
{

    const char *s = lua_tostring(L, -2);
    const char *r = lua_tostring(L, -1);

    int comp = strcmp(s, r);

    lua_pushinteger(L, comp);

    return 1;
}

int l_fma(lua_State *L)
{

    lua_Number x = lua_tonumber(L, 1);
    lua_Number y = lua_tonumber(L, 2);
    lua_Number z = lua_tonumber(L, 3);

    lua_Number r = fma(x, y, z);

    lua_pushnumber(L, r);

    return 1;
}

void math_constants(lua_State *L)
{
    lua_newtable(L);

    lua_pushnumber(L, M_E); /* e */
    lua_setfield(L, -2, "M_E");

    lua_pushnumber(L, M_LOG2E); /* log_2 e */
    lua_setfield(L, -2, "M_LOG2E");

    lua_pushnumber(L, M_LOG10E); /* log_10 e */
    lua_setfield(L, -2, "M_LOG10E");

    lua_pushnumber(L, M_LN2); /* log_e 2 */
    lua_setfield(L, -2, "M_LN2");

    lua_pushnumber(L, M_LN10); /* log_e 10 */
    lua_setfield(L, -2, "M_LN10");

    lua_pushnumber(L, M_PI); /* pi */
    lua_setfield(L, -2, "M_PI");

    lua_pushnumber(L, M_PI_2); /* pi/2 */
    lua_setfield(L, -2, "M_PI_2");

    lua_pushnumber(L, M_PI_4); /* pi/4 */
    lua_setfield(L, -2, "M_PI_4");

    lua_pushnumber(L, M_1_PI); /* 1/pi */
    lua_setfield(L, -2, "M_1_PI");

    lua_pushnumber(L, M_2_PI); /* 2/pi */
    lua_setfield(L, -2, "M_2_PI");

    lua_pushnumber(L, M_2_SQRTPI); /* 2/sqrt(pi) */
    lua_setfield(L, -2, "M_2_SQRTPI");

    lua_pushnumber(L, M_SQRT2); /* sqrt(2) */
    lua_setfield(L, -2, "M_SQRT2");

    lua_pushnumber(L, M_SQRT1_2); /* 1/sqrt(2) */
    lua_setfield(L, -2, "M_SQRT1_2");

    lua_Number M_GR = 1.61803398874989484820458683436563811772030917980576286213544862270526046281890244970720720418939113748475;
    lua_pushnumber(L, M_GR); /* Golden ratio */
    lua_setfield(L, -2, "M_GR");

    lua_Number M_SR = 0.61803398874989484820458683436563811772030917980576286213544862270526046281890244970720720418939113748475;
    lua_pushnumber(L, M_SR); /* Silver ratio */
    lua_setfield(L, -2, "M_SR");

    lua_pushnumber(L, M_GR + M_SR); /* Platinum ratio */
    lua_setfield(L, -2, "M_PR");

    lua_pushnumber(L, 1.0 - M_SR); /* Bronze ratio */
    lua_setfield(L, -2, "M_BR");

    lua_setfield(L, -2, "math");
}

int l_pthread_attr_init(lua_State *L)
{
    luaL_checktype(L, 1, LUA_TTABLE);

    int type;

    pthread_attr_t *attr = (pthread_attr_t *)malloc(sizeof(pthread_attr_t));

    type = pthread_attr_init(attr);
    if (type != 0)
        luaL_error(L, "pthread_attr_init failed.");

    lua_pushinteger(L, type);
    lua_pushlightuserdata(L, attr);

    type = lua_getfield(L, 1, "setdetachstate");
    if (type == LUA_TNUMBER)
    {
        type = pthread_attr_setdetachstate(attr, lua_tointeger(L, -1));
        if (type != 0)
            luaL_error(L, "pthread_attr_setdetachstate failed.");
    }
    lua_pop(L, 1);

    return 2;
}

int l_pthread_attr_destroy(lua_State *L)
{
    int type;

    pthread_attr_t *attr = (pthread_attr_t *)lua_touserdata(L, 1);

    type = pthread_attr_destroy(attr);

    free(attr);

    lua_pushinteger(L, type);

    return 1;
}

int l_pthread_mutex_init(lua_State *L)
{
    int s;

    pthread_mutexattr_t *attr = lua_islightuserdata(L, 1) ? (pthread_mutexattr_t *)lua_touserdata(L, 1) : NULL;
    pthread_mutex_t *mutex = (pthread_mutex_t *)malloc(sizeof(pthread_mutex_t));

    s = pthread_mutex_init(mutex, attr);

    lua_pushinteger(L, s);
    lua_pushlightuserdata(L, mutex);

    return 2;
}

int l_pthread_mutex_destroy(lua_State *L)
{
    int type;

    pthread_mutex_t *attr = (pthread_mutex_t *)lua_touserdata(L, 1);

    type = pthread_mutex_destroy(attr);

    free(attr);

    lua_pushinteger(L, type);

    return 1;
}

int l_pthread_mutexattr_init(lua_State *L)
{
    int s;

    luaL_argcheck(L, lua_istable(L, -1), 1, "Expected a table of mutex attributes.");

    pthread_mutexattr_t *attr = (pthread_mutexattr_t *)malloc(sizeof(pthread_mutexattr_t));

    s = pthread_mutexattr_init(attr);

    lua_pushinteger(L, s);
    lua_pushlightuserdata(L, attr);

    s = lua_getfield(L, -1, "settype");
    if (s == LUA_TNUMBER)
    {
        s = pthread_mutexattr_settype(attr, lua_tointeger(L, -1));
        if (s != 0)
            luaL_error(L, "pthread_mutexattr_settype failed.");
    }
    lua_pop(L, 1);

    return 2;
}

int l_pthread_mutexattr_destroy(lua_State *L)
{
    int type;

    pthread_mutexattr_t *attr = (pthread_mutexattr_t *)lua_touserdata(L, 1);

    type = pthread_mutexattr_destroy(attr);

    free(attr);

    lua_pushinteger(L, type);

    return 1;
}

int l_pthread_mutex_lock(lua_State *L)
{

    pthread_mutex_t *s = (pthread_mutex_t *)lua_touserdata(L, -1);

    int retcode = pthread_mutex_lock(s);

    lua_pushinteger(L, retcode);

    return 1;
}

int l_pthread_mutex_unlock(lua_State *L)
{

    pthread_mutex_t *s = (pthread_mutex_t *)lua_touserdata(L, -1);

    int retcode = pthread_mutex_unlock(s);

    lua_pushinteger(L, retcode);

    return 1;
}

int l_pthread_cond_signal(lua_State *L)
{

    pthread_cond_t *s = (pthread_cond_t *)lua_touserdata(L, -1);

    int retcode = pthread_cond_signal(s);

    lua_pushinteger(L, retcode);

    return 1;
}

int l_pthread_cond_broadcast(lua_State *L)
{

    pthread_cond_t *s = (pthread_cond_t *)lua_touserdata(L, -1);

    int retcode = pthread_cond_broadcast(s);

    lua_pushinteger(L, retcode);

    return 1;
}

int l_pthread_cond_wait(lua_State *L)
{
    pthread_cond_t *s = (pthread_cond_t *)lua_touserdata(L, -2);
    pthread_mutex_t *m = (pthread_mutex_t *)lua_touserdata(L, -1);

    int retcode = pthread_cond_wait(s, m);

    lua_pushinteger(L, retcode);

    return 1;
}

int l_pthread_cond_init(lua_State *L)
{
    pthread_cond_t *cond = (pthread_cond_t *)malloc(sizeof(pthread_cond_t));

    int s = pthread_cond_init(cond, NULL);

    lua_pushinteger(L, s);
    lua_pushlightuserdata(L, cond);

    return 2;
}

int l_pthread_cond_destroy(lua_State *L)
{
    pthread_cond_t *cond = (pthread_cond_t *)lua_touserdata(L, -1);

    int s = pthread_cond_destroy(cond);

    free(cond);

    lua_pushinteger(L, s);

    return 1;
}

void *l_pthread_create_worker(void *arg)
{
    lua_State *L = (lua_State *)arg;
    int code = lua_pcall(L, 0, LUA_MULTRET, 0);
    lua_pushboolean(L, code == LUA_OK);
    lua_rotate(L, 1, 1); // put the code at the top of the stack.
    pthread_exit(L);
    return L;
}

int l_pthread_create(lua_State *L)
{
    luaL_checktype(L, 1, LUA_TTABLE);
    luaL_checktype(L, 2, LUA_TFUNCTION);

    pthread_attr_t attr;

    if (pthread_attr_init(&attr) != 0)
        luaL_error(L, "pthread_attr_init failed.");

    // if (lua_getfield(L, 1, "create_detached") == LUA_TBOOLEAN && lua_toboolean(L, -1))
    // {
    //     if (pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED) != 0)
    //         luaL_error(L, "pthread_attr_setdetachstate failed.");
    // }

    // lua_pop(L, 1);

    lua_remove(L, 1); // remove the attributes table from the stack.

    lua_State *S = lua_newthread(L);

    lua_rotate(L, 1, 1); // put the function at the bottom of the stack and the thread at the top.

    lua_xmove(L, S, 1); // move the function to the new state.

    pthread_t *thread = (pthread_t *)malloc(sizeof(pthread_t));

    int s = pthread_create(thread, &attr, &l_pthread_create_worker, S);

    if (pthread_attr_destroy(&attr) != 0)
    {
        free(thread);
        luaL_error(L, "pthread_attr_destroy failed.");
    }

    if (s != 0)
    {
        free(thread);
        luaL_error(L, "pthread_create: %s.\n", strerror(s));
    }

    lua_createtable(L, 0, 2);

    lua_pushvalue(L, 1); // push the Lua thread
    lua_setfield(L, -2, "thread");

    lua_pushlightuserdata(L, thread);
    lua_setfield(L, -2, "pthread");

    return 1;
}

int l_pthread_join(lua_State *L)
{
    luaL_checktype(L, 1, LUA_TTABLE);

    lua_getfield(L, 1, "thread");
    lua_State *S = lua_tothread(L, -1);

    lua_getfield(L, 1, "pthread");
    pthread_t *thread = (pthread_t *)lua_touserdata(L, -1);

    if (thread == NULL)
        luaL_error(L, "No thread to join.");

    lua_pop(L, 2); // remove the thread and the pthread from the stack.

    void *ret;

    int s = pthread_join(*thread, &ret);

    if (s != 0)
        luaL_error(L, "pthread_join failed: %s.\n", strerror(s));

    lua_pushlightuserdata(L, NULL);
    lua_setfield(L, 1, "pthread");

    free(thread);

    if (S != ret)
        luaL_error(L, "pthread_join failed: perhaps the thread was detached?");

    int n = lua_gettop(S);
    lua_xmove(S, L, n); // move the return value to the main state.

    if (lua_closethread(S, L) != LUA_OK)
        luaL_error(L, "lua_closethread failed: %s.\n", lua_tostring(L, -1));

    lua_pushnil(L);
    lua_setfield(L, 1, "thread");

    return n;
}

int l_pthread_cancel(lua_State *L)
{
    luaL_checktype(L, 1, LUA_TLIGHTUSERDATA);

    pthread_t *thread = (pthread_t *)lua_touserdata(L, 1);

    int s = pthread_cancel(*thread);

    if (s != 0)
        luaL_error(L, "pthread_cancel failed: %s.\n", strerror(s));

    return 0;
}

int l_strtok_r(lua_State *L)
{
    const char *orig = lua_tostring(L, 1);
    const char *delimiters = lua_tostring(L, 2);
    bool include_empty_lines = lua_toboolean(L, 3);

    char *str = (char *)malloc(sizeof(char) * strlen(orig) + 1);

    char *del = strcpy(str, orig);

    // auxiliary pointers for tokens and followers.
    char *pch = NULL;
    char *ptr = str;

    int lines = 1;

    lua_newtable(L); // for tokens.

    while ((pch = strtok_r(str, delimiters, &str)) != NULL)
    {
        while (include_empty_lines && ptr != pch)
        {
            lua_pushstring(L, "");
            lua_seti(L, -2, lines);

            lines++;
            ptr += 1;
        }

        lua_pushstring(L, pch);
        lua_seti(L, -2, lines);

        lines++;
        ptr = str;
    }

    while (include_empty_lines && *ptr != '\0')
    {
        lua_pushstring(L, "");
        lua_seti(L, -2, lines);

        lines++;
        ptr += 1;
    }

    if (lines == 1)
    {
        lua_pushstring(L, str);
        lua_seti(L, -2, lines);
    }

    free(del);

    return 1;
}

const struct luaL_Reg libc[] = {
    {"qsort", l_qsort},
    {"bsearch", l_bsearch},
    {"strcmp", l_strcmp},
    {"strtok_r", l_strtok_r},
    {"l64a", l_l64a},
    {"a64l", l_a64l},
    {"lldiv", l_lldiv},
    {"fma", l_fma},
    {"pthread_create", l_pthread_create},
    {"pthread_join", l_pthread_join},
    {"pthread_cancel", l_pthread_cancel},
    {"pthread_attr_init", l_pthread_attr_init},
    {"pthread_attr_destroy", l_pthread_attr_destroy},
    {"pthread_mutexattr_init", l_pthread_mutexattr_init},
    {"pthread_mutexattr_destroy", l_pthread_mutexattr_destroy},
    {"pthread_mutex_init", l_pthread_mutex_init},
    {"pthread_mutex_destroy", l_pthread_mutex_destroy},
    {"pthread_mutex_lock", l_pthread_mutex_lock},
    {"pthread_mutex_unlock", l_pthread_mutex_unlock},
    {"pthread_cond_init", l_pthread_cond_init},
    {"pthread_cond_destroy", l_pthread_cond_destroy},
    {"pthread_cond_signal", l_pthread_cond_signal},
    {"pthread_cond_broadcast", l_pthread_cond_broadcast},
    {"pthread_cond_wait", l_pthread_cond_wait},
    {NULL, NULL} /* sentinel */
};

void pthread_constants(lua_State *L)
{
    lua_newtable(L);

    lua_pushinteger(L, PTHREAD_CREATE_JOINABLE);
    lua_setfield(L, -2, "create_joinable");

    lua_pushinteger(L, PTHREAD_CREATE_DETACHED);
    lua_setfield(L, -2, "create_detached");

    lua_pushinteger(L, PTHREAD_MUTEX_NORMAL);
    lua_setfield(L, -2, "mutex_normal");

    lua_pushinteger(L, PTHREAD_MUTEX_ERRORCHECK);
    lua_setfield(L, -2, "mutex_errorcheck");

    lua_pushinteger(L, PTHREAD_MUTEX_RECURSIVE);
    lua_setfield(L, -2, "mutex_recursive");

    lua_pushinteger(L, PTHREAD_MUTEX_DEFAULT);
    lua_setfield(L, -2, "mutex_default");

    lua_setfield(L, -2, "pthread");
}

void push_stddef(lua_State *L)
{
    lua_newtable(L);

    lua_pushlightuserdata(L, NULL);
    lua_setfield(L, -2, "NULL");

    lua_setfield(L, -2, "stddef");
}

int luaopen_liblibc(lua_State *L)
{
    luaL_newlib(L, libc);

    math_constants(L);
    pthread_constants(L);
    push_stddef(L);

    return 1;
}
