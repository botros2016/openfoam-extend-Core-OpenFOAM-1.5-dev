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
    Foam::basicThermoCloud

Description

SourceFiles
    basicThermoCloud.C

\*---------------------------------------------------------------------------*/

#ifndef basicThermoCloud_H
#define basicThermoCloud_H

#include "ThermoCloud.H"
#include "basicThermoParcel.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                        Class basicThermoCloud Declaration
\*---------------------------------------------------------------------------*/

class basicThermoCloud
:
    public ThermoCloud<basicThermoParcel>
{

    // Private Member Functions

        //- Disallow default bitwise copy construct
        basicThermoCloud
        (
            const basicThermoCloud&
        );

        //- Disallow default bitwise assignment
        void operator=(const basicThermoCloud&);


public:

    //- Runtime type information
    TypeName("basicThermoCloud");


    // Constructors

        //- Construct given carrier gas fields
        basicThermoCloud
        (
            const word& cloudType,
            const volPointInterpolation& vpi,
            const volScalarField& rho,
            const volVectorField& U,
            const dimensionedVector& g,
            basicThermo& thermo
        );


    //- Destructor

        ~basicThermoCloud();


    // Member Functions

        //- Write fields
        void writeFields() const;
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
