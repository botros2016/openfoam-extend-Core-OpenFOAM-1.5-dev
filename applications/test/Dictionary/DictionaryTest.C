/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | Copyright held by original author
     \\/     M anipulation  |
-------------------------------------------------------------------------------
License
    This file is part of OpenFOAM.

    OpenFOAM is free software; you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by the
    Free Software Foundation; either version 2 of the License, or (at your
    option) any later version.

    OpenFOAM is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
    for more details.

    You should have received a copy of the GNU General Public License
    along with OpenFOAM; if not, write to the Free Software Foundation,
    Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA

Application
    
Description

\*---------------------------------------------------------------------------*/

#include "OSspecific.H"

#include "IOstreams.H"
#include "Dictionary.H"

using namespace Foam;

class ent
:
    public Dictionary<ent>::link
{
    word keyword_;
    int i_;

public:

    ent(const word& keyword, int i)
    :
        keyword_(keyword),
        i_(i)
    {}

    const word& keyword() const
    {
        return keyword_;
    }

    friend Ostream& operator<<(Ostream& os, const ent& e)
    {
        os << e.keyword_ << ' ' << e.i_ << endl;
        return os;
    }
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //
//  Main program:

int main(int argc, char *argv[])
{
    Dictionary<ent>* dictPtr = new Dictionary<ent>;
    Dictionary<ent>& dict = *dictPtr;

    for (int i = 0; i<10; i++)
    {
        ent* ePtr = new ent(word("ent") + name(i), i);
        dict.append(ePtr->keyword(), ePtr);
        dict.swapUp(ePtr);
    }

    Info<< dict << endl;

    dict.swapDown(dict.first());

    for
    (
        Dictionary<ent>::const_iterator iter = dict.begin();
        iter != dict.end();
        ++iter
    )
    {
        Info<< "element : " << *iter;
    }

    Info<< dict.toc() << endl;

    delete dictPtr;

    dictPtr = new Dictionary<ent>;
    Dictionary<ent>& dict2 = *dictPtr;

    for (int i = 0; i<10; i++)
    {
        ent* ePtr = new ent(word("ent") + name(i), i);
        dict2.append(ePtr->keyword(), ePtr);
        dict2.swapUp(ePtr);
    }

    Info<< dict2 << endl;

    Info<< nl << "Bye." << endl;
    return 0;
}


// ************************************************************************* //
