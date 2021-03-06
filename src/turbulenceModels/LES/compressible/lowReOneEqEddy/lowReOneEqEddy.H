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
    Foam::compressible::LESModels::lowReOneEqEddy

Description
    One Equation Eddy Viscosity Model for compressible flow

    @verbatim
        d/dt(rho*k) + div(rho*U*k) - div(muEff*grad(k))
        =
        -rho*B*L - ce*rho*k^3/2/delta

    and

        B = 2/3*k*I - 2*nuSgs*dev(D)

    where

        nuSgsHiRe = ck*sqrt(k)*delta
        nuSgs = (nu/beta)*(1 - exp(-beta*nuSgsHiRe/nu));
    @endverbatim

SourceFiles
    lowReOneEqEddy.C

\*---------------------------------------------------------------------------*/

#ifndef compressibleLowReOneEqEddy_H
#define compressibleLowReOneEqEddy_H

#include "GenEddyVisc.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{
namespace compressible
{
namespace LESModels
{

/*---------------------------------------------------------------------------*\
                           Class lowReOneEqEddy Declaration
\*---------------------------------------------------------------------------*/

class lowReOneEqEddy
:
    public GenEddyVisc
{
    // Private data

        dimensionedScalar ck_;
        dimensionedScalar beta_;

    // Private Member Functions

        // Disallow default bitwise copy construct and assignment
        lowReOneEqEddy(const lowReOneEqEddy&);
        lowReOneEqEddy& operator=(const lowReOneEqEddy&);


public:

    //- Runtime type information
    TypeName("lowReOneEqEddy");


    // Constructors

        //- Constructor from components
        lowReOneEqEddy
        (
            const volScalarField& rho,
            const volVectorField& U,
            const surfaceScalarField& phi,
            const basicThermo& thermoPhysicalModel
        );


    // Destructor

        ~lowReOneEqEddy()
        {}


    // Member Functions

        //- Return the effective diffusivity for k
        tmp<volScalarField> DkEff() const
        {
            return tmp<volScalarField>
            (
                new volScalarField("DkEff", muSgs_ + mu())
            );
        }

        //- Correct Eddy-Viscosity and related properties
        void correct(const tmp<volTensorField>& gradU);

        //- Read turbulenceProperties dictionary
        bool read();
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace LESModels
} // End namespace compressible
} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
