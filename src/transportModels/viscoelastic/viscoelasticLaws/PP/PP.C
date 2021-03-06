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

#include "PP.H"
#include "addToRunTimeSelectionTable.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

defineTypeNameAndDebug(PP, 0);
addToRunTimeSelectionTable(viscoelasticLaw, PP, dictionary);

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

// from components
PP::PP
(
    const word& name,
    const volVectorField& U,
    const surfaceScalarField& phi,
    const dictionary& dict
)
:
    viscoelasticLaw(name, U, phi),
    S_
    (
        IOobject
        (
            "S" + name,
            U.time().timeName(),
            U.mesh(),
            IOobject::MUST_READ,
            IOobject::AUTO_WRITE
        ),
        U.mesh()
    ),
    Lambda_
    (
        IOobject
        (
            "Lambda" + name,
            U.time().timeName(),
            U.mesh(),
            IOobject::MUST_READ,
            IOobject::AUTO_WRITE
        ),
        U.mesh()
    ),
    tau_
    (
        IOobject
        (
            "tau" + name,
            U.time().timeName(),
            U.mesh(),
            IOobject::NO_READ,
            IOobject::AUTO_WRITE
        ),
        U.mesh(),
        dimensionedSymmTensor
        (
            "zero",
            dimensionSet(1, -1, -2, 0, 0, 0, 0),
            symmTensor::zero
        )
    ),
    I_
    (
        dimensionedSymmTensor
        (
            "I",
            dimensionSet(0, 0, 0, 0, 0, 0, 0),
            symmTensor
            (
                1, 0, 0,
                   1, 0,
                      1
            )
        )
    ),
    rho_(dict.lookup("rho")),
    etaS_(dict.lookup("etaS")),
    etaP_(dict.lookup("etaP")),
    lambdaOb_(dict.lookup("lambdaOb")),
    lambdaOs_(dict.lookup("lambdaOs")),
    q_(dict.lookup("q"))
{}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

tmp<fvVectorMatrix> PP::divTau(volVectorField& U) const
{

     dimensionedScalar etaPEff = etaP_;

    return
    (
        fvc::div(tau_/rho_, "div(tau)")
      - fvc::laplacian(etaPEff/rho_, U, "laplacian(etaPEff,U)")
      + fvm::laplacian( (etaPEff + etaS_)/rho_, U, "laplacian(etaPEff+etaS,U)")
    );

}


void PP::correct()
{
    // Velocity gradient tensor
    volTensorField L = fvc::grad( U() );

    // Convected derivate term
    volTensorField C = S_ & L;

    // Twice the rate of deformation tensor
    volSymmTensorField twoD = twoSymm( L );


    // Evolution of orientation 
    tmp<fvSymmTensorMatrix> SEqn
    (
        fvm::ddt(S_)
        + fvm::div(phi(), S_)
        ==
        twoSymm( C )
        - fvm::Sp( 1/lambdaOb_, S_ ) 
        + 1/3/lambdaOb_*I_

    );

    SEqn().relax();
    solve(SEqn);


     // Evolution of the backbone stretch
    tmp<fvScalarMatrix> LambdaEqn
    (
        fvm::ddt(Lambda_)
        + fvm::div(phi(), Lambda_)
        ==
        fvm::Sp( (twoD && (S_/tr(S_)) ) / 2 , Lambda_ )
        - fvm::Sp( Foam::exp( 2/q_*(Lambda_ - 1))/lambdaOs_ , Lambda_ )
        + Foam::exp( 2/q_*(Lambda_ - 1))/lambdaOs_

    );

    LambdaEqn().relax();
    solve(LambdaEqn);


    // Pom Pom condition:
    Lambda_.max
    (
        dimensionedScalar
        (
            "zero",
            Lambda_.dimensions(),
            scalar( q_.value() )
        )
    );

    // Viscoelastic stress
    tau_ = etaP_/lambdaOb_ * (3*Foam::sqr(Lambda_)*S_/tr(S_) - I_);
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// ************************************************************************* //
