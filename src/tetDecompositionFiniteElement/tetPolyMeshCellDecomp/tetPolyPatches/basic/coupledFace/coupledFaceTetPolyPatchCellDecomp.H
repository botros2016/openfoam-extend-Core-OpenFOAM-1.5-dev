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
    coupledFaceTetPolyPatchCellDecomp

Description
    Coupled patch.  Used as the base class for processor
    and cyclic tetPolyPatches

SourceFiles
    coupledFaceTetPolyPatchCellDecomp.C

\*---------------------------------------------------------------------------*/

#ifndef coupledFaceTetPolyPatchCellDecomp_H
#define coupledFaceTetPolyPatchCellDecomp_H

#include "lduInterface.H"
#include "faceTetPolyPatchCellDecomp.H"
#include "coupledPolyPatch.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

class tetPolyBoundaryMeshCellDecomp;

/*---------------------------------------------------------------------------*\
               Class coupledFaceTetPolyPatchCellDecomp Declaration
\*---------------------------------------------------------------------------*/

class coupledFaceTetPolyPatchCellDecomp
:
    public lduInterface,
    public faceTetPolyPatchCellDecomp
{
    // Private data

        const coupledPolyPatch& coupledPolyPatch_;


    // Private Member Functions

        //- Disallow default construct as copy
        coupledFaceTetPolyPatchCellDecomp
        (
            const coupledFaceTetPolyPatchCellDecomp&
        );

        //- Disallow default assignment
        void operator=(const coupledFaceTetPolyPatchCellDecomp&);


protected:

    // Demand driven private data

        //- The set of labels of the tetPolyPatch points which are
        //  non-global, i.e. present in this coupled
        mutable labelList* nonGlobalPatchPointsPtr_;

        //- Mesh points
        mutable labelList* meshPointsPtr_;


        // Construction of demand-driven data

            //- Calculate mesh points
            virtual void calcMeshPoints() const = 0;


public:

    typedef tetPolyBoundaryMeshCellDecomp BoundaryMesh;


    //- Runtime type information
    TypeName(coupledPolyPatch::typeName_());


    // Constructors

        //- Construct from components
        coupledFaceTetPolyPatchCellDecomp
        (
            const polyPatch& patch,
            const tetPolyBoundaryMeshCellDecomp& bm
        );


    // Destructor

        virtual ~coupledFaceTetPolyPatchCellDecomp();


    // Member Functions

        // Access

            //- Return size
            virtual label size() const
            {
                return meshPoints().size();
            }

            //- Return true because this patch is coupled
            virtual bool coupled() const
            {
                return true;
            }


        // Access functions for demand driven data

            //- Return the set of labels of the tetPolyPatch points which are
            //  non-global, i.e. present in this coupled
            virtual const labelList& nonGlobalPatchPoints() const;

            //- Return mesh points
            virtual const labelList& meshPoints() const;

            //- Return pointField of points in patch.  Not impelemented.
            virtual const pointField& localPoints() const;

            //- Return point unit normals.  Not impelemented.
            virtual const vectorField& pointNormals() const;
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
