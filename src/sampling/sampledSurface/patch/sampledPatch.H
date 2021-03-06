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
    Foam::sampledPatch

Description
    A sampledSurface on a patch. Non-triangulated by default.

SourceFiles
    sampledPatch.C

\*---------------------------------------------------------------------------*/

#ifndef sampledPatch_H
#define sampledPatch_H

#include "sampledSurface.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                       Class sampledPatch Declaration
\*---------------------------------------------------------------------------*/

class sampledPatch
:
    public sampledSurface
{
    // Private data

        //- Name of patch
        const word patchName_;

        //- Index of patch in boundaryMesh
        const label patchIndex_;

        //- Zero size or copy of patch.localPoints()
        pointField points_;

        //- Faces (triangulated or non-triangulated)
        faceList faces_;

        //- Local patch face labels
        labelList patchFaceLabels_;

    // Private Member Functions

        //- Do all to construct geometry.
        void createGeometry();

        //- sample field on faces
        template <class Type>
        tmp<Field<Type> > sampleField
        (
            const GeometricField<Type, fvPatchField, volMesh>& vField
        ) const;


        template <class Type>
        tmp<Field<Type> >
        interpolateField(const interpolation<Type>&) const;


public:

    //- Runtime type information
    TypeName("sampledPatch");


    // Constructors

        //- Construct from components
        sampledPatch
        (
            const word& name,
            const polyMesh& mesh,
            const word& patchName,
            const bool triangulate = false
        );

        //- Construct from dictionary
        sampledPatch
        (
            const word& name,
            const polyMesh& mesh,
            const dictionary& dict
        );


    // Destructor

        virtual ~sampledPatch();


    // Member Functions

        const word patchName() const
        {
            return patchName_;
        }

        label patchIndex() const
        {
            return patchIndex_;
        }

        const labelList& patchFaceLabels() const
        {
            return patchFaceLabels_;
        }

        //- Points of surface
        virtual const pointField& points() const
        {
            return points_;
        }

        //- Faces of surface
        virtual const faceList& faces() const
        {
            return faces_;
        }


        //- Correct for mesh movement and/or field changes
        virtual void correct(const bool meshChanged);


        //- sample field on surface
        virtual tmp<scalarField> sample
        (
            const volScalarField&
        ) const;

        //- sample field on surface
        virtual tmp<vectorField> sample
        (
            const volVectorField&
        ) const;

        //- sample field on surface
        virtual tmp<sphericalTensorField> sample
        (
            const volSphericalTensorField&
        ) const;

        //- sample field on surface
        virtual tmp<symmTensorField> sample
        (
            const volSymmTensorField&
        ) const;

        //- sample field on surface
        virtual tmp<tensorField> sample
        (
            const volTensorField&
        ) const;


        //- interpolate field on surface
        virtual tmp<scalarField> interpolate
        (
            const interpolation<scalar>&
        ) const;


        //- interpolate field on surface
        virtual tmp<vectorField> interpolate
        (
            const interpolation<vector>&
        ) const;

        //- interpolate field on surface
        virtual tmp<sphericalTensorField> interpolate
        (
            const interpolation<sphericalTensor>&
        ) const;

        //- interpolate field on surface
        virtual tmp<symmTensorField> interpolate
        (
            const interpolation<symmTensor>&
        ) const;

        //- interpolate field on surface
        virtual tmp<tensorField> interpolate
        (
            const interpolation<tensor>&
        ) const;

        //- Write
        virtual void print(Ostream&) const;
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#ifdef NoRepository
#   include "sampledPatchTemplates.C"
#endif

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
