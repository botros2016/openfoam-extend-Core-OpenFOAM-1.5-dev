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

Description
    Read of a non-delimited hex label

\*---------------------------------------------------------------------------*/

#include "readHexLabel.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

Foam::label Foam::readHexLabel(ISstream& is)
{
    register label result = 0;

    char c = 0;

    static const label zeroOffset = int('0');

    // This takes into account that a is 10
    static const label alphaOffset = toupper('A') - 10;

    // Get next non-whitespace character
    while (is.get(c) && isspace(c))
    {}

    do
    {
        if (isspace(c) || c == 0) break;

        if (!isxdigit(c))
        {
            FatalIOErrorIn("readHexLabel(ISstream& is)", is)
                << "Illegal hex digit: \"" << c << "\""
                << exit(FatalIOError);
        }

        result *= 16;

        if (isdigit(c))
        {
            result += int(c) - zeroOffset;
        }
        else
        {
            result += toupper(c) - alphaOffset;
        }
    } while (is.get(c));

    return result;
}


// ************************************************************************* //
