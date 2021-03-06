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

\*---------------------------------------------------------------------------*/

#include "spectEddyVisc.H"
#include "addToRunTimeSelectionTable.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{
namespace incompressible
{
namespace LESModels
{

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

defineTypeNameAndDebug(spectEddyVisc, 0);
addToRunTimeSelectionTable(LESModel, spectEddyVisc, dictionary);

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

// from components
spectEddyVisc::spectEddyVisc
(
    const volVectorField& U,
    const surfaceScalarField& phi,
    transportModel& transport
)
:
    LESModel(typeName, U, phi, transport),
    GenEddyVisc(U, phi, transport),


    cB_
    (
        dimensioned<scalar>::lookupOrAddToDict
        (
            "cB",
            coeffDict(),
            8.22
        )
    ),
    cK1_
    (
        dimensioned<scalar>::lookupOrAddToDict
        (
            "cK1",
            coeffDict(),
            0.83
        )
    ),
    cK2_
    (
        dimensioned<scalar>::lookupOrAddToDict
        (
            "cK2",
            coeffDict(),
            1.03
        )
    ),
    cK3_
    (
        dimensioned<scalar>::lookupOrAddToDict
        (
            "cK3",
            coeffDict(),
            4.75
        )
    ),
    cK4_
    (
        dimensioned<scalar>::lookupOrAddToDict
        (
            "cK4",
            coeffDict(),
            2.55
        )
    )
{
    printCoeffs();
}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

tmp<volScalarField> spectEddyVisc::k() const
{
    volScalarField Eps = 2*nuEff()*magSqr(symm(fvc::grad(U())));

    return
        cK1_*pow(delta(), 2.0/3.0)*pow(Eps, 2.0/3.0)
        *exp(-cK2_*pow(delta(), -4.0/3.0)*nu()/pow(Eps, 1.0/3.0))
      - cK3_*pow(Eps*nu(), 1.0/2.0)
       *erfc(cK4_*pow(delta(), -2.0/3.0)*pow(Eps, -1.0/6.0));
}


void spectEddyVisc::correct(const tmp<volTensorField>& gradU)
{
    GenEddyVisc::correct(gradU);

    volScalarField Re = sqr(delta())*mag(symm(gradU))/nu();

    for (label i=0; i<5; i++)
    {
        nuSgs_ =
            nu()
           /(
               scalar(1)
               - exp(-cB_*pow(nu()/(nuSgs_ + nu()), 1.0/3.0)*pow(Re, -2.0/3.0))
            );
    }

    nuSgs_.correctBoundaryConditions();
}


bool spectEddyVisc::read()
{
    if (GenEddyVisc::read())
    {
        cB_.readIfPresent(coeffDict());
        cK1_.readIfPresent(coeffDict());
        cK2_.readIfPresent(coeffDict());
        cK3_.readIfPresent(coeffDict());
        cK4_.readIfPresent(coeffDict());

        return true;
    }
    else
    {
        return false;
    }
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace LESModels
} // End namespace incompressible
} // End namespace Foam

// ************************************************************************* //
