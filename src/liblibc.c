
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


typedef struct item_s {
    lua_State *L;
    int idx;
} item_t;

static int compare(const void *v, const void *w) {

    item_t *vc = (item_t *) v;
    item_t *wc = (item_t *) w;

    lua_State *L = vc->L;

    lua_pushvalue(L, -1);
    lua_geti(L, -3, vc->idx);
    lua_geti(L, -4, wc->idx);
    lua_call(L, 2, 1);
    int comparison = lua_tointeger(L, -1);
    lua_pop(L, 1);

    return comparison;
}

static int l_qsort(lua_State *L) {
	
    /* Initial checks */
    assert(lua_istable(L, -2));
    assert(lua_isfunction(L, -1));

    int table_absidx = lua_absindex(L, -2);

    lua_len(L, table_absidx);		// push on the stack the number of elements to sort.
    int nel = lua_tointeger(L, -1); 	// get that number.
    lua_pop(L, 1);                  	// clean the stack.

    item_t *permutation = (item_t *) malloc(sizeof(item_t) * nel);

    for (int i = 0; i < nel; i++) {
	
	    item_t item; 
	
	    item.L = L;
	    item.idx = i + 1;
        
        permutation[i] = item;
    }

    time_t starttime = time(NULL);
    qsort (permutation, nel, sizeof(item_t), compare);
    time_t endtime = time(NULL);

    lua_createtable(L, nel, 0);	// the sorted table
    lua_createtable(L, nel, 0);	// the sorting permutation

    for (int i = 0; i < nel; i++) {
        int idx = permutation[i].idx;

        lua_geti(L, table_absidx, idx);
        lua_seti(L, -3, i + 1);

        lua_pushinteger(L, idx);	// to also provide the sorting permutation.
        lua_seti(L, -2, i + 1);
    }

    lua_pushinteger(L, endtime - starttime);

    free(permutation);

    return 3;
}

static int compare_bsearch(const void *k, const void *v) {

    item_t *key = (item_t *) k;
    item_t *each = (item_t *) v;

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

static int l_bsearch(lua_State *L) {
	
    /* Initial checks */
    assert(lua_istable(L, -3));
    assert(lua_isfunction(L, -1));

    int table_absidx = lua_absindex(L, -3);

    lua_len(L, table_absidx);		// push on the stack the number of elements to sort.
    int nel = lua_tointeger(L, -1); 	// get that number.
    lua_pop(L, 1);                  	// clean the stack.

    item_t *permutation = (item_t *) malloc(sizeof(item_t) * nel);

    for (int i = 0; i < nel; i++) {
	
	    item_t item; 
	
	    item.L = L;
	    item.idx = i + 1;
        
        permutation[i] = item;
    }

    item_t key;
    key.L = L;
    key.idx = table_absidx + 1;

    item_t *found = (item_t *) bsearch (&key, permutation, nel, sizeof(item_t), compare_bsearch);

    free(permutation);

    if (found != NULL) {
        lua_geti(L, table_absidx, found->idx);
    } else {
        lua_pushnil(L);
    }
    
    return 1;
}

static int l_l64a(lua_State *L) {
    lua_Integer n = lua_tointeger(L, -1);
    
    const char *str = l64a(n);
    lua_pushstring(L, str);

    return 1;
}

static int l_a64l(lua_State *L) {
    
    const char *str = lua_tostring(L, -1);
    
    long n = a64l(str);
    lua_pushinteger(L, n);

    return 1;
}

static int l_lldiv(lua_State *L) {

    lua_Integer n = lua_tointeger(L, -2);
    lua_Integer m = lua_tointeger(L, -1);

    lldiv_t d = lldiv(n, m);

    lua_pushinteger(L, d.quot);
    lua_pushinteger(L, d.rem);

    return 2;

}

static int l_strcmp(lua_State *L) {

    const char *s = lua_tostring(L, -2);
    const char *r = lua_tostring(L, -1);

    int comp = strcmp(s, r);

    lua_pushinteger(L, comp);

    return 1;
}

static int l_fma(lua_State *L) {

    lua_Number x = lua_tonumber(L, -3);
    lua_Number y = lua_tonumber(L, -2);
    lua_Number z = lua_tonumber(L, -1);

    lua_Number r = fma(x, y, z);

    lua_pushnumber(L, r);

    return 1;
}

static int l_constants(lua_State *L) {
    
    assert (lua_istable(L, -1) == 1);

    lua_pushnumber(L, M_E);	                /* e */
    lua_setfield(L, -2, "M_E");

    lua_pushnumber(L, M_LOG2E);	            /* log_2 e */
    lua_setfield(L, -2, "M_LOG2E");

    lua_pushnumber(L, M_LOG10E);	        /* log_10 e */
    lua_setfield(L, -2, "M_LOG10E");

    lua_pushnumber(L, M_LN2);	            /* log_e 2 */
    lua_setfield(L, -2, "M_LN2");

    lua_pushnumber(L, M_LN10);	            /* log_e 10 */
    lua_setfield(L, -2, "M_LN10");

    lua_pushnumber(L, M_PI);	            /* pi */
    lua_setfield(L, -2, "M_PI");

    lua_pushnumber(L, M_PI_2);	            /* pi/2 */
    lua_setfield(L, -2, "M_PI_2");

    lua_pushnumber(L, M_PI_4);	            /* pi/4 */
    lua_setfield(L, -2, "M_PI_4");

    lua_pushnumber(L, M_1_PI);	            /* 1/pi */
    lua_setfield(L, -2, "M_1_PI");

    lua_pushnumber(L, M_2_PI);	            /* 2/pi */
    lua_setfield(L, -2, "M_2_PI");

    lua_pushnumber(L, M_2_SQRTPI);	        /* 2/sqrt(pi) */
    lua_setfield(L, -2, "M_2_SQRTPI");

    lua_pushnumber(L, M_SQRT2);	            /* sqrt(2) */
    lua_setfield(L, -2, "M_SQRT2");

    lua_pushnumber(L, M_SQRT1_2);	        /* 1/sqrt(2) */
    lua_setfield(L, -2, "M_SQRT1_2");

    lua_Number M_GR = 1.61803398874989484820458683436563811772030917980576286213544862270526046281890244970720720418939113748475;
    lua_pushnumber(L, M_GR);                /* Golden ratio */
    lua_setfield(L, -2, "M_GR");

    lua_Number M_SR = 0.61803398874989484820458683436563811772030917980576286213544862270526046281890244970720720418939113748475;
    lua_pushnumber(L, M_SR);                /* Silver ratio */ 
    lua_setfield(L, -2, "M_SR");

    lua_pushnumber(L, M_GR + M_SR);         /* Platinum ratio */ 
    lua_setfield(L, -2, "M_PR");

    lua_pushnumber(L, 1.0 - M_SR);          /* Bronze ratio */ 
    lua_setfield(L, -2, "M_BR");

    return 0;
}

static void * pthread_create_callback (void *arg) {

    item_t* ud = (item_t*) arg;

    lua_State* auxstate = ud->L;
    int nargs = ud->idx;

    lua_call (auxstate, nargs, LUA_MULTRET);    // all the checks have been done by `l_pthread_create`. 

    pthread_exit (arg);

    return arg;
}

static int l_pthread_create_curry (lua_State* L) {

    int nargs = lua_gettop (L);     // including the function to be called in the C callback.

    item_t* ud = (item_t *) malloc (sizeof (item_t));

    lua_State* S = lua_newthread (L);           // the new thread is left on the stack,
    int newthread_pos = lua_absindex (L, -1);   // and get its absolute index.
    
    for (int i = 1; i <= nargs; i++) lua_pushvalue (L, i);  // duplicate all the arguments.

    lua_xmove (L, S, nargs);    // then move them in chunk.

    ud->L = S;
    ud->idx = nargs - 1;
    
    pthread_t* t = (pthread_t*) malloc (sizeof(pthread_t));
    pthread_attr_t* attr = (pthread_attr_t*) lua_touserdata (L, lua_upvalueindex(1));
    int res = pthread_create (t, attr, pthread_create_callback, ud);

    lua_pushinteger (L, res);

    lua_newtable (L);

    lua_pushlightuserdata (L, t);
    lua_setfield (L, -2, "pthread");

    lua_pushlightuserdata (L, attr);
    lua_setfield (L, -2, "attribute");

    lua_pushlightuserdata (L, ud);
    lua_setfield (L, -2, "userdata");

    lua_pushvalue (L, newthread_pos);
    lua_setfield (L, -2, "cothread");

    lua_remove (L, newthread_pos);

    return 2;
}

static int l_pthread_create (lua_State* L) {

    luaL_argcheck (L, lua_istable (L, -1), 1, "Expected a table of pthread attributes.");

    // for now ignore the given attributes table completely.

    pthread_attr_t* attr = (pthread_attr_t*) malloc (sizeof(pthread_attr_t));

    int s = pthread_attr_init(attr);
    if (s != 0) luaL_error (L, "pthread_attr_init failed.");
    
    lua_pushlightuserdata (L, attr);

    lua_pushcclosure (L, &l_pthread_create_curry, 1);

    return 1;
}

static int l_pthread_join(lua_State* L) {

    assert (lua_istable (L, -1));
    int table_idx = lua_absindex (L, -1);

    lua_getfield (L, table_idx, "pthread");
    pthread_t* pthread = (pthread_t*) lua_touserdata (L, -1);
    lua_pop (L, 1);

    lua_getfield (L, table_idx, "userdata");
    void* userdata = lua_touserdata (L, -1);
    lua_pop (L, 1);

    void *res;
    int flag = pthread_join (*pthread, &res);

    int nret = 0;

    lua_pushinteger (L, flag);
    nret++;

    if (res == userdata) {

        item_t* ud = (item_t*) res;
        lua_State* auxstate = ud->L;

        int returned = lua_gettop (auxstate);// - (ud->idx + 1);

        lua_xmove (auxstate, L, returned);

        nret += returned;
    }

    return nret;
}

static int l_pthread_self(lua_State* L) {

    luaL_argcheck (L, lua_isfunction (L, -1), 1, "Expected a function that consumes a pthread.");

    int nargs = lua_gettop (L);

    if (nargs > 1) luaL_argerror (L, 2, "Just one argument is expected.");

    pthread_t pthread = pthread_self ();

    lua_pushvalue (L, -1);  // dup the function to be called.
    lua_newtable (L);
    lua_pushlightuserdata (L, &pthread);
    lua_setfield (L, -2, "pthread");
    
    lua_call (L, 1, LUA_MULTRET);

    return lua_gettop (L) - nargs;
}

static int l_pthread_equal(lua_State* L) {

    lua_getfield (L, -2, "pthread");
    pthread_t* r = (pthread_t*) lua_touserdata (L, -1);
    lua_pop (L, 1);

    lua_getfield (L, -1, "pthread");
    pthread_t* s = (pthread_t*) lua_touserdata (L, -1);
    lua_pop (L, 1);

    int cmp = pthread_equal (*r, *s);

    lua_pushboolean (L, cmp != 0 ? 1 : 0);

    return 1;
}

static int l_pthread_detach(lua_State* L) {

    lua_getfield (L, -1, "pthread");
    pthread_t* s = (pthread_t*) lua_touserdata (L, -1);
    lua_pop (L, 1);

    int retcode = pthread_detach (*s);

    lua_pushinteger (L, retcode);

    return 1;
}

static int l_pthread_cancel(lua_State* L) {

    lua_getfield (L, -1, "pthread");
    pthread_t* s = (pthread_t*) lua_touserdata (L, -1);
    lua_pop (L, 1);

    int retcode = pthread_cancel (*s);

    lua_pushinteger (L, retcode);

    return 1;
}

static int l_pthread_attribute (lua_State* L) {

    lua_getfield (L, -1, "attribute");
    pthread_attr_t* attr = (pthread_attr_t*) lua_touserdata (L, -1);
    lua_pop (L, 1);

    lua_newtable (L);

    int res;

    int detached;
    res = pthread_attr_getdetachstate (attr, &detached);
    if (res != 0) luaL_error (L, "pthread_attr_getdetachstate failed.");
    lua_pushboolean (L, detached);
    lua_setfield (L, -2, "detachstate");

    return 1;
}

static const struct luaL_Reg libc [] = {
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
    {"pthread_attribute", l_pthread_attribute},
    {"pthread_cancel", l_pthread_cancel},
	{NULL, NULL} /* sentinel */
};

static void pthread_constants (lua_State* L){

    lua_newtable (L);

    lua_pushinteger (L, PTHREAD_CREATE_JOINABLE);
    lua_setfield (L, -2, "create_joinable");

    lua_pushinteger (L, PTHREAD_CREATE_DETACHED);
    lua_setfield (L, -2, "create_detached");

    lua_setfield (L, -2, "pthread");

}

int luaopen_liblibc (lua_State *L) {
	luaL_newlib(L, libc);

    pthread_constants (L);

	return 1;
}
