/* XKb Switch API for using in vim libcall() */

#include <algorithm>
#include "XKeyboard.h"

using namespace std;


namespace
{
    XKeyboard     xkb;
    StringVector  symNames;


    StringVector &  getSymNames( void )
    {
        static bool  loaded = false;

        if ( loaded )
            return symNames;

        loaded = true;

        XkbGetControls( xkb._display, XkbAllControlsMask, xkb._kbdDescPtr );
        XkbGetNames( xkb._display, XkbSymbolsNameMask, xkb._kbdDescPtr );

        Atom symNameAtom = xkb._kbdDescPtr->names->symbols;
        if ( symNameAtom == None )
            return symNames;

        char *  symNameC = XGetAtomName( xkb._display, symNameAtom );
        string  symName( symNameC );

        XFree( symNameC );

        if ( symName.empty() )
            return symNames;

        XkbSymbolParser  symParser;

        symParser.parse( symName, symNames );

        return symNames;
    }
}


extern "C"
{
    const char *  Xkb_Switch_getXkbLayout( const char *  /* unused */ )
    {
        StringVector &  syms = getSymNames();

        try
        {
            return syms.at( xkb.getCurrentGroupNum() ).c_str();
        }
        catch( ... )
        {
            return NULL;
        }
    }


    const char *  Xkb_Switch_setXkbLayout( const char *  newgrp )
    {
        StringVector &  syms = getSymNames();

        if ( newgrp == NULL || newgrp[ 0 ] == '\0' )
            return NULL;

        StringVector::iterator  i = find( syms.begin(), syms.end(), newgrp );

        if ( i == syms.end() )
           return NULL;

        try
        {
            xkb.setGroupByNum( i - syms.begin() );
        }
        catch( ... )
        {
        }

        return NULL;
    }
}

