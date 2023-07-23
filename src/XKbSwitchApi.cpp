/*
 * Copyright (C) 2013 by Alexei Rad'kov
 * Copyright (C) 2010-2023 by Sergei Mironov
 *
 * This file is part of Xkb-switch.
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

/** XKb Switch API for using in vim libcall() */

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

