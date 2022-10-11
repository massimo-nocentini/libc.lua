
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


struct item_s {
    lua_State *L;
    int idx;
};

typedef struct item_s item_t;

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

static const struct luaL_Reg libc [] = {
	{"qsort", l_qsort},
    {"bsearch", l_bsearch},
    {"strcmp", l_strcmp},
    {"l64a", l_l64a},
    {"a64l", l_a64l},
    {"lldiv", l_lldiv},
	{NULL, NULL} /* sentinel */
};
 
int luaopen_liblibc (lua_State *L) {
	luaL_newlib(L, libc);
	return 1;
}
