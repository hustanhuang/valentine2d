#include <stdio.h>
#include <stdlib.h>

#include <lauxlib.h>
#include <lua.h>
#include <lualib.h>

#include <GLUT/glut.h>

#pragma clang diagnostic ignored "-Wdeprecated-declarations"

struct window_conf {
    int pos_x;
    int pos_y;
    int size_w;
    int size_h;
    const char* title;
};

int getInteger(lua_State* L, const char* var, int fallback)
{
    lua_getglobal(L, var);
    int ret = lua_tointeger(L, 1);
    lua_pop(L, 1);
    return ret ? ret : fallback;
}

const char* getString(lua_State* L, const char* var, const char* fallback)
{
    lua_getglobal(L, var);
    const char* ret = lua_tostring(L, 1);
    lua_pop(L, 1);
    return ret ? ret : fallback;
}

struct window_conf* getWindowConf()
{
    struct window_conf* conf = malloc(sizeof(struct window_conf));

    lua_State* L = luaL_newstate();

    int did_conf_lua = luaL_dofile(L, "conf.lua");

    if (did_conf_lua != 0) {
        conf->pos_x = 0;
        conf->pos_y = 0;
        conf->size_w = 512;
        conf->size_h = 512;
        conf->title = "valentine2d";
    } else {
        conf->pos_x = getInteger(L, "pos_x", 0);
        conf->pos_y = getInteger(L, "pos_y", 0);
        conf->size_w = getInteger(L, "size_w", 512);
        conf->size_h = getInteger(L, "size_h", 512);
        conf->title = getString(L, "title", "valentine2d");
    }

    lua_close(L);

    return conf;
}

void freeWindowConf(struct window_conf* conf)
{
    free(conf);
}

int extSetColor(lua_State* L)
{
    int r = lua_tonumber(L, 1);
    int g = lua_tonumber(L, 2);
    int b = lua_tonumber(L, 3);

    // printf("set color <%d, %d, %d>\n", r, g, b);

    glColor3ub(r, g, b);

    return 0;
}

int extDrawLine(lua_State* L)
{
    int from_x = lua_tonumber(L, 1);
    int from_y = lua_tonumber(L, 2);
    int to_x = lua_tonumber(L, 3);
    int to_y = lua_tonumber(L, 4);

    // printf("draw line from <%d, %d> to <%d, %d>\n", from_x, from_y, to_x, to_y);

    glBegin(GL_LINES);
    {
        glVertex2i(from_x, from_y);
        glVertex2i(to_x, to_y);
    }
    glEnd();

    return 0;
}

void display()
{
    glClear(GL_COLOR_BUFFER_BIT);

    lua_State* L = luaL_newstate();
    luaL_openlibs(L);

    lua_pushcfunction(L, extSetColor);
    lua_setglobal(L, "setColor");

    lua_pushcfunction(L, extDrawLine);
    lua_setglobal(L, "drawLine");

    luaL_dofile(L, "main.lua");

    lua_close(L);

    glFlush();
}

int main(int argc, char* argv[])
{
    // init GLUT
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);

    // read conf.lua and init window
    struct window_conf* conf = getWindowConf();

    glutInitWindowPosition(conf->pos_x, conf->pos_y);
    glutInitWindowSize(conf->size_w, conf->size_h);
    glutCreateWindow(conf->title);

    glClearColor(0.0, 0.0, 0.0, 0.0);
    glMatrixMode(GL_PROJECTION);
    gluOrtho2D(0.0, conf->size_w, 0.0, conf->size_h);

    freeWindowConf(conf);

    // exec main.lua
    glutDisplayFunc(display);

    glutMainLoop();
    return 0;
}
