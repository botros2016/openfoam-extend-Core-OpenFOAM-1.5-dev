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

#include "turbulentMixingLengthFrequencyInletFvPatchScalarField.H"
#include "addToRunTimeSelectionTable.H"
#include "fvPatchFieldMapper.H"
#include "surfaceFields.H"
#include "volFields.H"
#include "RASModel.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

turbulentMixingLengthFrequencyInletFvPatchScalarField::
turbulentMixingLengthFrequencyInletFvPatchScalarField
(
    const fvPatch& p,
    const DimensionedField<scalar, volMesh>& iF
)
:
    fixedValueFvPatchField<scalar>(p, iF),
    mixingLength_(0.0),
    kName_("undefined-k")
{}

turbulentMixingLengthFrequencyInletFvPatchScalarField::
turbulentMixingLengthFrequencyInletFvPatchScalarField
(
    const turbulentMixingLengthFrequencyInletFvPatchScalarField& ptf,
    const fvPatch& p,
    const DimensionedField<scalar, volMesh>& iF,
    const fvPatchFieldMapper& mapper
)
:
    fixedValueFvPatchField<scalar>(ptf, p, iF, mapper),
    mixingLength_(ptf.mixingLength_),
    kName_(ptf.kName_)
{}

turbulentMixingLengthFrequencyInletFvPatchScalarField::
turbulentMixingLengthFrequencyInletFvPatchScalarField
(
    const fvPatch& p,
    const DimensionedField<scalar, volMesh>& iF,
    const dictionary& dict
)
:
    fixedValueFvPatchField<scalar>(p, iF, dict),
    mixingLength_(readScalar(dict.lookup("mixingLength"))),
    kName_(dict.lookup("k"))
{}

turbulentMixingLengthFrequencyInletFvPatchScalarField::
turbulentMixingLengthFrequencyInletFvPatchScalarField
(
    const turbulentMixingLengthFrequencyInletFvPatchScalarField& ptf
)
:
    fixedValueFvPatchField<scalar>(ptf),
    mixingLength_(ptf.mixingLength_),
    kName_(ptf.kName_)
{}

turbulentMixingLengthFrequencyInletFvPatchScalarField::
turbulentMixingLengthFrequencyInletFvPatchScalarField
(
    const turbulentMixingLengthFrequencyInletFvPatchScalarField& ptf,
    const DimensionedField<scalar, volMesh>& iF
)
:
    fixedValueFvPatchField<scalar>(ptf, iF),
    mixingLength_(ptf.mixingLength_),
    kName_(ptf.kName_)
{}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

void turbulentMixingLengthFrequencyInletFvPatchScalarField::updateCoeffs()
{
    if (updated())
    {
        return;
    }

    // Lookup Cmu corresponding to the turbulence model selected
    const incompressible::RASModel& RAS =
        db().lookupObject<incompressible::RASModel>("RASProperties");
    scalar Cmu = readScalar(RAS.coeffDict().lookup("Cmu"));

    scalar Cmu25 = pow(Cmu, 0.25);

    const fvPatchField<scalar>& kp =
        patch().lookupPatchField<volScalarField, scalar>(kName_);

    operator==(sqrt(kp)/(Cmu25*mixingLength_));

    fixedValueFvPatchField<scalar>::updateCoeffs();
}


void turbulentMixingLengthFrequencyInletFvPatchScalarField::write
(
    Ostream& os
) const
{
    fvPatchField<scalar>::write(os);
    os.writeKeyword("mixingLength")
        << mixingLength_ << token::END_STATEMENT << nl;
    os.writeKeyword("k") << kName_ << token::END_STATEMENT << nl;
    writeEntry("value", os);
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

makePatchTypeField
(
    fvPatchScalarField,
    turbulentMixingLengthFrequencyInletFvPatchScalarField
);


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// ************************************************************************* //
