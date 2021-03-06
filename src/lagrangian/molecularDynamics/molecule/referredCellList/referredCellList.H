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

Class
    Foam::referredCellList

Description

SourceFiles
    referredCellListI.H
    referredCellList.C

\*---------------------------------------------------------------------------*/

#ifndef referredCellList_H
#define referredCellList_H

#include "referredCell.H"
#include "molecule.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                        Class referredCellList Declaration
\*---------------------------------------------------------------------------*/

class referredCellList
:
    public List< referredCell >
{
    // Private data

        moleculeCloud& molCloud_;


public:

    // Constructors

        //- Construct from moleculeCloud
        referredCellList(moleculeCloud& molCloud);

        //- Construct from components
        referredCellList
        (
            moleculeCloud& molCloud,
            const List<referredCell>& referredCells,
            const List<label>& realCells
        );


    // Destructor

        ~referredCellList();


    // Member Functions

        void setRealCellsInRange();

        void referMolecules();

        inline const moleculeCloud& molCloud();
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#include "referredCellListI.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
