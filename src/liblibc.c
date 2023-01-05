
/*
    This is a glue c file for importing delta client c functions into Lua workflow.
*/

#include <stdio.h>
#include <stdlib.h>
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

static int compare(const void *v, const void *w)
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

static int l_qsort(lua_State *L)
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

static int compare_bsearch(const void *k, const void *v)
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

static int l_bsearch(lua_State *L)
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

    free(permutation);

    if (found != NULL)
    {
        lua_geti(L, table_absidx, found->idx);
    }
    else
    {
        lua_pushnil(L);
    }

    return 1;
}

static int l_l64a(lua_State *L)
{
    lua_Integer n = lua_tointeger(L, -1);

    const char *str = l64a(n);
    lua_pushstring(L, str);

    return 1;
}

static int l_a64l(lua_State *L)
{

    const char *str = lua_tostring(L, -1);

    long n = a64l(str);
    lua_pushinteger(L, n);

    return 1;
}

static int l_lldiv(lua_State *L)
{

    lua_Integer n = lua_tointeger(L, -2);
    lua_Integer m = lua_tointeger(L, -1);

    lldiv_t d = lldiv(n, m);

    lua_pushinteger(L, d.quot);
    lua_pushinteger(L, d.rem);

    return 2;
}

static int l_strcmp(lua_State *L)
{

    const char *s = lua_tostring(L, -2);
    const char *r = lua_tostring(L, -1);

    int comp = strcmp(s, r);

    lua_pushinteger(L, comp);

    return 1;
}

static int l_fma(lua_State *L)
{

    lua_Number x = lua_tonumber(L, -3);
    lua_Number y = lua_tonumber(L, -2);
    lua_Number z = lua_tonumber(L, -1);

    lua_Number r = fma(x, y, z);

    lua_pushnumber(L, r);

    return 1;
}

static int l_constants(lua_State *L)
{

    assert(lua_istable(L, -1) == 1);

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

    return 0;
}

static void *pthread_create_callback(void *arg)
{
    item_t *ud = (item_t *)arg;

    lua_State *auxstate = ud->L;
    int nargs = ud->idx;

    lua_State *from = lua_tothread(auxstate, -1);
    lua_pop(auxstate, 1);

    assert(lua_isfunction(auxstate, 1));
    // lua_pushvalue (auxstate, 1);

    int nres, retcode;
    retcode = lua_resume(auxstate, from, nargs, &nres);
    // retcode = lua_pcall (auxstate, nargs, LUA_MULTRET, 0);

    // nres = lua_gettop (auxstate);

    ud->idx = (retcode == LUA_OK || retcode == LUA_YIELD) ? nres : -1;

    pthread_exit(arg);

    return arg;
}

static int l_pthread_create_curry(lua_State *L)
{
    int type;

    int nargs = lua_gettop(L);

    lua_State *S = lua_tothread(L, lua_upvalueindex(3));
    item_t *ud = (item_t *)lua_touserdata(L, lua_upvalueindex(4));

    assert(S == ud->L); // this is our invariant.

    pthread_attr_t *attr = (pthread_attr_t *)lua_touserdata(L, lua_upvalueindex(5));

    ud->idx = nargs;

    pthread_t *t = (pthread_t *)malloc(sizeof(pthread_t));

    lua_pushvalue(L, lua_upvalueindex(1)); // push the function to be run in a pthread.
    lua_insert(L, 1);                      // and move it as the first argument.
    lua_pushvalue(L, lua_upvalueindex(3)); // push the calling coroutine.
    lua_xmove(L, ud->L, nargs + 2);        // move everything on the newly created coroutine.

    type = pthread_create(t, attr, &pthread_create_callback, ud);

    lua_pushinteger(L, type);

    type = pthread_attr_destroy(attr);
    if (type != 0)
        luaL_error(L, "pthread_attr_destroy failed.");

    free(attr);

    lua_newtable(L);

    lua_pushlightuserdata(L, t);
    lua_setfield(L, -2, "pthread");

    lua_pushlightuserdata(L, ud);
    lua_setfield(L, -2, "userdata");

    lua_pushvalue(L, lua_upvalueindex(3));
    lua_setfield(L, -2, "cothread");

    return 2;
}

static int l_pthread_create(lua_State *L)
{
    luaL_checktype(L, 1, LUA_TTABLE);
    luaL_checktype(L, 2, LUA_TNONE); // enforce exactly two arguments.

    int type;

    pthread_attr_t *attr = (pthread_attr_t *)malloc(sizeof(pthread_attr_t));

    type = pthread_attr_init(attr);
    if (type != 0)
        luaL_error(L, "pthread_attr_init failed.");

    type = lua_getfield(L, 1, "create_detached");
    if (type == LUA_TBOOLEAN && lua_toboolean(L, -1) == 1)
        pthread_attr_setdetachstate(attr, PTHREAD_CREATE_DETACHED);
    lua_pop(L, 1);

    type = lua_getfield(L, 1, "create_joinable");
    if (type == LUA_TBOOLEAN && lua_toboolean(L, -1) == 1)
        pthread_attr_setdetachstate(attr, PTHREAD_CREATE_JOINABLE);
    lua_pop(L, 1);

    type = lua_getfield(L, 1, "start_function");
    if (type != LUA_TFUNCTION)
        luaL_error(L, "The attributes table has to have the `start_function` field.");

    type = lua_pushthread(L); // this is the calling thread, doesn't mind if it is the main or not.

    lua_State *S = lua_newthread(L); // push a new thread.

    item_t *ud = (item_t *)malloc(sizeof(item_t));
    ud->L = S;

    lua_pushlightuserdata(L, ud);
    lua_pushlightuserdata(L, attr);

    lua_pushcclosure(L, l_pthread_create_curry, 5); // we also include the function given as second argument.

    return 1;
}

static int l_pthread_join(lua_State *L)
{

    assert(lua_istable(L, -1));
    int table_idx = lua_absindex(L, -1);

    lua_getfield(L, table_idx, "pthread");
    pthread_t *pthread = (pthread_t *)lua_touserdata(L, -1);
    lua_pop(L, 1);

    lua_getfield(L, table_idx, "userdata");
    void *userdata = lua_touserdata(L, -1);
    lua_pop(L, 1);

    void *res;
    int flag = pthread_join(*pthread, &res);

    int nret = 0;

    lua_pushinteger(L, flag);
    nret++;

    if (res == userdata)
    {
        item_t *ud = (item_t *)res;
        lua_State *auxstate = ud->L;

        int returned = ud->idx;

        free(pthread);
        free(ud);

        if (returned == -1)
        {
            lua_xmove(auxstate, L, 1);
            lua_error(L);
        }
        else
        {
            lua_xmove(auxstate, L, returned);
            int rs = lua_resetthread(auxstate);
            assert(rs == LUA_OK);
        }

        nret += returned;
    }

    return nret;
}

static int l_pthread_self(lua_State *L)
{

    luaL_argcheck(L, lua_isfunction(L, -1), 1, "Expected a function that consumes a pthread.");

    int nargs = lua_gettop(L);

    if (nargs > 1)
        luaL_argerror(L, 2, "Just one argument is expected.");

    pthread_t pthread = pthread_self();

    lua_pushvalue(L, -1); // dup the function to be called.
    lua_newtable(L);
    lua_pushlightuserdata(L, &pthread);
    lua_setfield(L, -2, "pthread");

    lua_call(L, 1, LUA_MULTRET);

    return lua_gettop(L) - nargs;
}

static int l_pthread_equal(lua_State *L)
{

    lua_getfield(L, -2, "pthread");
    pthread_t *r = (pthread_t *)lua_touserdata(L, -1);
    lua_pop(L, 1);

    lua_getfield(L, -1, "pthread");
    pthread_t *s = (pthread_t *)lua_touserdata(L, -1);
    lua_pop(L, 1);

    int cmp = pthread_equal(*r, *s);

    lua_pushboolean(L, cmp != 0 ? 1 : 0);

    return 1;
}

static int l_pthread_detach(lua_State *L)
{

    lua_getfield(L, -1, "pthread");
    pthread_t *s = (pthread_t *)lua_touserdata(L, -1);
    lua_pop(L, 1);

    int retcode = pthread_detach(*s);

    lua_pushinteger(L, retcode);

    return 1;
}

static int l_pthread_cancel(lua_State *L)
{

    lua_getfield(L, -1, "pthread");
    pthread_t *s = (pthread_t *)lua_touserdata(L, -1);
    lua_pop(L, 1);

    int retcode = pthread_cancel(*s);

    lua_pushinteger(L, retcode);

    return 1;
}

static int l_pthread_mutex_curry(lua_State *L)
{

    luaL_argcheck(L, lua_isfunction(L, -2), 1, "Expected a function that accepts a mutex and its attributes.");
    luaL_argcheck(L, lua_isfunction(L, -1), 2, "Expected a function that handles the error in case.");

    int nargs = lua_gettop(L); // including the function to be called in the C callback.
    assert(nargs == 2);

    pthread_mutex_t *mutex = (pthread_mutex_t *)lua_touserdata(L, lua_upvalueindex(1));
    pthread_mutexattr_t *attr = (pthread_mutexattr_t *)lua_touserdata(L, lua_upvalueindex(2));

    lua_pushvalue(L, -2); // duplicate the worker function.
    lua_pushlightuserdata(L, mutex);
    lua_pushlightuserdata(L, attr);

    int res = lua_pcall(L, 2, LUA_MULTRET, 0);

    pthread_mutex_destroy(mutex);
    pthread_mutexattr_destroy(attr);

    free(mutex);
    free(attr);

    if (res != LUA_OK)
    {
        lua_pushvalue(L, -2); // duplicate the error handler.
        lua_pushvalue(L, -2); // duplicate the error object.
        lua_remove(L, -3);    // remove the duplicated error object.

        lua_call(L, 1, LUA_MULTRET);
    }

    int nres = lua_gettop(L) - nargs;

    return nres;
}

static int l_pthread_mutex_init(lua_State *L)
{

    int s;

    luaL_argcheck(L, lua_istable(L, -1), 1, "Expected a table of mutex attributes.");

    pthread_mutex_t *mutex = (pthread_mutex_t *)malloc(sizeof(pthread_mutex_t));
    pthread_mutexattr_t *attr = (pthread_mutexattr_t *)malloc(sizeof(pthread_mutexattr_t));

    s = pthread_mutexattr_init(attr);
    if (s != 0)
        luaL_error(L, "pthread_mutexattr_init failed.");

    /*
    type = lua_getfield (L, -1, "create_detached");
    if (type == LUA_TBOOLEAN && lua_toboolean (L, -1) == 1)
        pthread_attr_setdetachstate (attr, PTHREAD_CREATE_DETACHED);
    lua_pop (L, 1);

    type = lua_getfield (L, -1, "create_joinable");
    if (type == LUA_TBOOLEAN && lua_toboolean (L, -1) == 1)
        pthread_attr_setdetachstate (attr, PTHREAD_CREATE_JOINABLE);
    lua_pop (L, 1);
    */

    s = pthread_mutex_init(mutex, attr);
    if (s != 0)
        luaL_error(L, "pthread_mutex_init failed.");

    lua_pushlightuserdata(L, mutex);
    lua_pushlightuserdata(L, attr);

    lua_pushcclosure(L, &l_pthread_mutex_curry, 2);

    return 1;
}

static int l_pthread_mutex_lock(lua_State *L)
{

    pthread_mutex_t *s = (pthread_mutex_t *)lua_touserdata(L, -1);

    int retcode = pthread_mutex_lock(s);

    lua_pushinteger(L, retcode);

    return 1;
}

static int l_pthread_mutex_unlock(lua_State *L)
{

    pthread_mutex_t *s = (pthread_mutex_t *)lua_touserdata(L, -1);

    int retcode = pthread_mutex_unlock(s);

    lua_pushinteger(L, retcode);

    return 1;
}

static int l_pthread_mutex_trylock(lua_State *L)
{

    pthread_mutex_t *s = (pthread_mutex_t *)lua_touserdata(L, -1);

    int retcode = pthread_mutex_trylock(s);

    lua_pushinteger(L, retcode);

    return 1;
}

static const struct luaL_Reg libc[] = {
    {"qsort", l_qsort},
    {"bsearch", l_bsearch},
    {"strcmp", l_strcmp},
    {"l64a", l_l64a},
    {"a64l", l_a64l},
    {"lldiv", l_lldiv},
    {"fma", l_fma},
    {"constants", l_constants},
    {"pthread_create", l_pthread_create},
    {"pthread_join", l_pthread_join},
    {"pthread_self", l_pthread_self},
    {"pthread_equal", l_pthread_equal},
    {"pthread_detach", l_pthread_detach},
    {"pthread_cancel", l_pthread_cancel},
    {"pthread_mutex_init", l_pthread_mutex_init},
    {"pthread_mutex_lock", l_pthread_mutex_lock},
    {"pthread_mutex_unlock", l_pthread_mutex_unlock},
    {"pthread_mutex_trylock", l_pthread_mutex_trylock},
    {NULL, NULL} /* sentinel */
};

static void pthread_constants(lua_State *L)
{

    lua_newtable(L);

    lua_pushinteger(L, PTHREAD_CREATE_JOINABLE);
    lua_setfield(L, -2, "create_joinable");

    lua_pushinteger(L, PTHREAD_CREATE_DETACHED);
    lua_setfield(L, -2, "create_detached");

    lua_setfield(L, -2, "pthread");
}

int luaopen_liblibc(lua_State *L)
{
    luaL_newlib(L, libc);

    pthread_constants(L);

    return 1;
}
