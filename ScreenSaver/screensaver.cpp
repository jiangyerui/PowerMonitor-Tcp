#include "screensaver.h"

ScreenSaver::ScreenSaver()
{

}

void ScreenSaver::restore()
{

}

bool ScreenSaver::save(int level)
{
   Q_UNUSED(level)

    if(g_lock == false)
    {
        if(g_login == true)
        {
           g_login = false;
        }
    }

    return g_login;

}

