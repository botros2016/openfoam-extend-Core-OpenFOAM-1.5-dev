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
    Foam::compressible::RASModels::mutStandardRoughWallFunctionFvPatchScalarField

Description
    Wall function boundary condition for rough walls

SourceFiles
    mutStandardWallFunctionFvPatchScalarField.C

\*---------------------------------------------------------------------------*/

#ifndef mutStandardRoughWallFunctionFvPatchScalarField_H
#define mutStandardRoughWallFunctionFvPatchScalarField_H

#include "fixedValueFvPatchFields.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{
namespace compressible
{
namespace RASModels
{

/*---------------------------------------------------------------------------*\
                    Class mutWallFunctionFvPatch Declaration
\*---------------------------------------------------------------------------*/

class mutStandardRoughWallFunctionFvPatchScalarField
:
    public fixedValueFvPatchScalarField
{
    // Private data

        //- Name of the velocity field
        word UName_;

        scalar roughnessHeight_;
        scalar roughnessConstant_;
        scalar roughnessFudgeFactor_;


public:

    //- Runtime type information
    TypeName("mutStandardRoughWallFunction");


    // Constructors

        //- Construct from patch and internal field
        mutStandardRoughWallFunctionFvPatchScalarField
        (
            const fvPatch&,
            const DimensionedField<scalar, volMesh>&
        );

        //- Construct from patch, internal field and dictionary
        mutStandardRoughWallFunctionFvPatchScalarField
        (
            const fvPatch&,
            const DimensionedField<scalar, volMesh>&,
            const dictionary&
        );

        //- Construct by mapping given 
        //  mutStandardRoughWallFunctionFvPatchScalarField
        //  onto a new patch
        mutStandardRoughWallFunctionFvPatchScalarField
        (
            const mutStandardRoughWallFunctionFvPatchScalarField&,
            const fvPatch&,
            const DimensionedField<scalar, volMesh>&,
            const fvPatchFieldMapper&
        );

        //- Construct as copy
        mutStandardRoughWallFunctionFvPatchScalarField
        (
            const mutStandardRoughWallFunctionFvPatchScalarField&
        );

        //- Construct and return a clone
        virtual tmp<fvPatchScalarField> clone() const
        {
            return tmp<fvPatchScalarField>
            (
                new mutStandardRoughWallFunctionFvPatchScalarField(*this)
            );
        }

        //- Construct as copy setting internal field reference
        mutStandardRoughWallFunctionFvPatchScalarField
        (
            const mutStandardRoughWallFunctionFvPatchScalarField&,
            const DimensionedField<scalar, volMesh>&
        );

        //- Construct and return a clone setting internal field reference
        virtual tmp<fvPatchScalarField> clone
        (
            const DimensionedField<scalar, volMesh>& iF
        ) const
        {
            return tmp<fvPatchScalarField>
            (
                new mutStandardRoughWallFunctionFvPatchScalarField(*this, iF)
            );
        }


    // Member functions

        // Access

            //- Return the name of the velocity field
            const word& UName() const
            {
                return UName_;
            }

            //- Return the fluctuation scale
            const scalar& roughnessHeight() const
            {
                return roughnessHeight_;
            }

            //- Return reference to the fluctuation scale to allow adjustment
            scalar& roughnessHeight()
            {
                return roughnessHeight_;
            }


            //- Return the fluctuation scale
            const scalar& roughnessConstant() const
            {
                return roughnessConstant_;
            }

            //- Return reference to the fluctuation scale to allow adjustment
            scalar& roughnessConstant()
            {
                return roughnessConstant_;
            }

            //- Return the fluctuation scale
            const scalar& roughnessFudgeFactor() const
            {
                return roughnessFudgeFactor_;
            }

            //- Return reference to the fluctuation scale to allow adjustment
            scalar& roughnessFudgeFactor()
            {
                return roughnessFudgeFactor_;
            }


        // Evaluation functions

            //- Evaluate the patchField
            virtual void evaluate
            (
                const Pstream::commsTypes commsType=Pstream::blocking
            );


        //- Write
        virtual void write(Ostream& os) const;
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace RASModels
} // End namespace compressible
} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
