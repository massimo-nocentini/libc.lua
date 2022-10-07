
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

    lua_len(L, table_absidx);  // push on the stack the number of elements to sort.
    int nel = lua_tointeger(L, -1); // get that number.
    lua_pop(L, 1);                  // clean the stack.

    item_t *permutation = (item_t *) malloc(sizeof(item_t) * nel);

    for (int i = 0; i < nel; i++) {
        item_t *item = (item_t *) malloc(sizeof(item_t));
        
        item->L = L;
        item->idx = i + 1;
        
        permutation[i] = *item;
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

        //free(&permutation[i]);
    }

    lua_pushinteger(L, endtime - starttime);

    free(permutation);

    return 3;
}

static const struct luaL_Reg libc [] = {
	{"qsort", l_qsort},
	{NULL, NULL} /* sentinel */
};
 
int luaopen_liblibc (lua_State *L) {
	luaL_newlib(L, libc);
	return 1;
}
