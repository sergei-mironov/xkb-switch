/*
 * XKb Switch API for using in vim libcall()
 *
 * Copyright (C) 2013 by Alexei Rad'kov
 * Copyright (C) 2010-2023 by Sergei Mironov
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 3 of the License, or (at your option)
 * any later version.
 */

#include <algorithm>
#include "XKeyboard.hpp"

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
                    xkb = new XKeyboard(0);
                    xkb->open_display();
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
        xkb->build_layout(symNames);
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

