/* XKb Switch API for using in vim libcall() */

#include <algorithm>
#include "XKeyboard.h"

using namespace std;
using namespace kb;

namespace
{
    class  XKeyboardWrapper
    {
        public:
            XKeyboardWrapper() : xkb( NULL ), unusable( false )
            {}

            ~XKeyboardWrapper()
            {
                delete xkb;
            }

            XKeyboard *  get( void )
            {
                if ( unusable )
                    return NULL;

                if ( xkb )
                    return xkb;

                try
                {
                    xkb = new XKeyboard();
                }
                catch( ... )
                {
                    unusable = true;
                    return NULL;
                }

                return xkb;
            }

        private:
            XKeyboard *  xkb;
            bool         unusable;

    }  xkb;

    string_vector  symNames;

    string_vector &  getSymNames( void )
    {
        static bool  loaded = false;

        if ( loaded )
            return symNames;

        XKeyboard *  xkb = ::xkb.get();

        if ( ! xkb )
        {
            loaded = true;
            return symNames;
        }

        loaded = true;
        XkbGetControls( xkb->_display, XkbAllControlsMask, xkb->_kbdDescPtr );
        XkbGetNames( xkb->_display, XkbSymbolsNameMask, xkb->_kbdDescPtr );

        Atom symNameAtom = xkb->_kbdDescPtr->names->symbols;
        if ( symNameAtom == None )
            return symNames;

        char *  symNameC = XGetAtomName( xkb->_display, symNameAtom );
        string  symName( symNameC );

        XFree( symNameC );

        if ( symName.empty() )
            return symNames;

        symNames = parse2( symName, nonsyms() );

        return symNames;
    }
}


extern "C"
{
    const char *  Xkb_Switch_getXkbLayout( const char *  /* unused */ )
    {
        try
        {
            XKeyboard *  xkb = ::xkb.get();

            if ( ! xkb )
                return "";

            string_vector &  syms = getSymNames();

            return syms.at( xkb->get_group() ).c_str();
        }
        catch( ... )
        {
        }

        return NULL;
    }


    const char *  Xkb_Switch_setXkbLayout( const char *  newgrp )
    {
        try
        {
            XKeyboard *  xkb = ::xkb.get();

            if ( ! xkb )
                return "";

            string_vector &  syms = getSymNames();

            if ( newgrp == NULL || newgrp[ 0 ] == '\0' )
                return NULL;

            string_vector::iterator  i = find( syms.begin(), syms.end(),
                                               newgrp );

            if ( i == syms.end() )
               return NULL;

            xkb->set_group( i - syms.begin() );
        }
        catch( ... )
        {
        }

        return NULL;
    }
}

