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
    GGIInterpolation

Description
    Mass-conservative face interpolation of face data between two
    primitivePatches

Author
    Hrvoje Jasak, Wikki Ltd.  All rights reserved

Contributor:
    Martin Beaudoin, Hydro-Quebec, (2008)

SourceFiles
    GGIInterpolation.C
    GGIInterpolate.C
    GGIInterpolationWeights.C

\*---------------------------------------------------------------------------*/

#ifndef GGIInterpolation_H
#define GGIInterpolation_H

#include "className.H"
#include "labelList.H"
#include "scalarField.H"
#include "pointField.H"
#include "FieldFields.H"
#include "faceList.H"
#include "intersection.H"
#include "point2D.H"
#include "NamedEnum.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                   Class GGIInterpolationName Declaration
\*---------------------------------------------------------------------------*/

class GGIInterpolationName
{
public:

    // Public enumerations

        //- Define a return type for isVertexInsidePolygon.
        //  The names are rather obvious
        // on vertex, or on edge or inside == inside
        enum insideOutside
        {
            ALL_OUTSIDE,
            ALL_INSIDE,
            PARTIALLY_OVERLAPPING
        };

        //- Quick reject overlap search algorithm
        enum quickReject
        {
            THREE_D_DISTANCE,
            AABB,
            N_SQUARED
        };


    // Static data

        ClassName("GGIInterpolation");

        //- Quick reject names
        static const NamedEnum<quickReject, 3> quickRejectNames_;


    // Constructors

        //- Construct null
        GGIInterpolationName()
        {}

};


/*---------------------------------------------------------------------------*\
                      Class GGIInterpolation Declaration
\*---------------------------------------------------------------------------*/

template<class MasterPatch, class SlavePatch>
class GGIInterpolation
:
    public GGIInterpolationName
{
    // Private data types

        // Definition for a 3D orthoNormalBasis type
        typedef VectorSpace<Vector<vector>, vector, 3> orthoNormalBasis;


    // Private data

        //- Reference to the master patch
        const MasterPatch& masterPatch_;

        //- Reference to the slave patch
        const SlavePatch& slavePatch_;

        //- Slave-to-master transformation tensor.  Transforms slave data to
        //  master plane.  Size equals number of slave faces; zero length
        //  indicates no transform.  Size 1 indicates constant transform
        //  HJ, 6/Jan/2009
        const tensorField forwardT_;

        //- Master-to-slave transformation tensor.  Transforms slave data to
        //  master plane.  Size equals number of master faces; zero length
        //  indicates no transform.  Size 1 indicates constant transform
        //  HJ, 6/Jan/2009
        const tensorField reverseT_;

        //- Slave-to-master separation vector. Translation of slave data to
        //  master plane.  Size equals number of slave faces; zero length
        //  indicates no translation.  MB, 28/Jan/2009
        const vectorField forwardSep_;

        //- Master non-overlap face tolerance factor
        const scalar masterNonOverlapFaceTol_;

        //- Slave non-overlap face tolerance factor
        const scalar slaveNonOverlapFaceTol_;

        //- Rescale the GGI weighting factors or not
        const bool rescaleGGIWeightingFactors_;

        //- Quick reject search algorithm
        quickReject reject_;


    // Demand-driven data

        // Master-to-slave interpolation

            //- Master to slave addressing
            mutable labelListList* masterAddrPtr_;

            //- Master to slave  weights
            mutable scalarListList* masterWeightsPtr_;

        // Slave-to-master interpolation

            //- Slave to master addressing
            mutable labelListList* slaveAddrPtr_;

            //- Slave to master weights
            mutable scalarListList* slaveWeightsPtr_;

            //- List of uncovered master faces
            mutable labelList* uncoveredMasterAddrPtr_;

            //- List of uncovered slave faces
            mutable labelList* uncoveredSlaveAddrPtr_;


    // Private static data

        //- Facet area error tolerance
        static const scalar areaErrorTol_;

        //- Facet normal featureCos criteria
        static const scalar featureCosTol_;


    // Private Member Functions

        //- Disallow default bitwise copy construct
        GGIInterpolation(const GGIInterpolation&);

        //- Disallow default bitwise assignment
        void operator=(const GGIInterpolation&);


    // Helper functions for demand-driven data

        //- Evaluate faces neighborhood based of sphere defined by faces BB
        void findNeighbours3D(labelListList& result) const;

        //- Evaluate faces neighborhood based of faces Axis Aligned BB
        void findNeighboursAABB(labelListList& result) const;

        //- Projects a list of points onto a plane located at
        //  planeOrig, oriented along planeNormal
        tmp<pointField> projectPointsOnPlane
        (
            const pointField& lpoints,
            const vector& planeOrig,
            const vector& planeDirection,
            scalarField& distanceProjection
        ) const;

        //- Compute an orthonormal basis. Useful for 2D projection
        orthoNormalBasis computeOrthonormalBasis
        (
            const vector& normalVectorCentre,
            const vector& normalVector,
            const pointField& pointsOnPlane
        ) const;

        //- Compute the 2D projection of a 3D list of point into the
        //  orthonormal basis orthoBase
        List<point2D> projectPoints3Dto2D
        (
            const orthoNormalBasis& orthoBase,
            const vector& orthoBaseOffset,
            const pointField& pointsIn3D,
            scalarField& distanceProjection
        ) const;

        //- Compute the 1D projection of a 2D list of points onto the
        //  projectionDir direction
        scalarField projectPoints2Dto1D
        (
            const vector2D& normalizedProjectionDir,
            const point2D& normalizedProjectionDirOffset,
            const List<point2D>& lPoints2D
        ) const;

        //- Detect the intersection of 2 2D polygons using the method of
        //  Separating Axes Theorem
        bool detect2dPolygonsOverlap
        (
            const List<point2D>& poly1,
            const List<point2D>& poly2,
            const scalar& tolFactor,
            const bool firstCall = true // Optional parameter: first call
        ) const;

        //- Compute the intersection area between 2 2D polygons
        scalar polygonIntersection
        (
            const List<point2D>& poly1,
            const List<point2D>& poly2
        ) const;

        //- Detect is the points from a subject polygon are inside or
        //  outside a clipping polygon
        insideOutside isVertexInsidePolygon
        (
            const List<point2D>& clippingPolygon,
            const List<point2D>& subjectPolygon,
            List<bool>& subjectVertexInside
        ) const;

        //- Compute the intersection between two 2D polygons using the
        //  Sutherland Hodgman algorithm
        List<point2D> clipPolygon2DSutherlandHodgman
        (
            const List<point2D>& clippingPolygon,
            const List<point2D>& subjectPolygon
        ) const;

        //- Compute the intersection between two 2D polygons using the
        //  Greiner Hormann algorithm
        List<point2D> clipPolygon2DGreinerHormann
        (
            const List<point2D>& clippingPolygon,
            const List<point2D>& subjectPolygon,
            const List<bool>& subjectVertexInside
        ) const;

        //- Compute the area of a 2D polygon
        scalar area2D
        (
            const List<point2D>& polygon
        ) const;


        //- Calculate addressing and weights
        void calcAddressing() const;

        //- Rescale GGI weighting factors
        void rescaleWeightingFactors() const;

        //- Find non-overlapping faces
        tmp<labelField> findNonOverlappingFaces
        (
            const scalarListList& patchWeights,
            const scalar& nonOverlapFaceTol
        ) const;

        //- Clear all geometry and addressing
        void clearOut();


    // Interpolation data access

        //- Interpolate given source and target, addressing and weights
        template<class Type>
        static void interpolate
        (
            const Field<Type>& ff,
            Field<Type>& result,
            const labelListList& addr,
            const scalarListList& weights
        );

        //- Bridge uncovered faces given addressing
        template<class Type>
        static void bridge
        (
            const Field<Type>& bridgeField,
            Field<Type>& ff,
            const labelList& addr
        );

        //- Is a transform required?
        inline bool doTransform() const
        {
            return forwardT_.size() > 0;
        }

        //- Is a separation required?
        inline bool doSeparation() const
        {
            return forwardSep_.size() > 0;
        }


public:

    // Constructors

        //- Construct from components
        GGIInterpolation
        (
            const MasterPatch& masterPatch,
            const SlavePatch&  slavePatch,
            const tensorField& forwardT,
            const tensorField& reverseT,
            const vectorField& forwardSep,
            const scalar masterFaceNonOverlapFaceTol = 0.0,
            const scalar slaveFaceNonOverlapFaceTol = 0.0,
            const bool rescaleGGIWeightingFactors = true,
            const quickReject reject = AABB
        );


    // Destructor

        ~GGIInterpolation();


    // Member Functions

        // Access

        //- Return reference to master addressing
        const labelListList& masterAddr() const;

        //- Return reference to master weights
        const scalarListList& masterWeights() const;

        //- Return reference to slave addressing
        const labelListList& slaveAddr() const;

        //- Return reference to slave weights
        const scalarListList& slaveWeights() const;

        //- Return reference to the master list of non-overlap faces
        const labelList& uncoveredMasterFaces() const;

        //- Return reference to the slave list of non-overlap faces
        const labelList& uncoveredSlaveFaces() const;


    // Interpolation functions

        //- Interpolate from master to slave
        template<class Type>
        tmp<Field<Type> > masterToSlave(const Field<Type>& pf) const;

        template<class Type>
        tmp<Field<Type> > masterToSlave
        (
            const tmp<Field<Type> >& tpf
        ) const;

        //- Interpolate from slave to master
        template<class Type>
        tmp<Field<Type> > slaveToMaster(const Field<Type>& pf) const;

        template<class Type>
        tmp<Field<Type> > slaveToMaster
        (
            const tmp<Field<Type> >& tpf
        ) const;


        //- Bridge uncovered master patch field
        template<class Type>
        void bridgeMaster
        (
            const Field<Type>& bridgeField,
            Field<Type>& ff
        ) const;

        //- Bridge uncovered slave patch field
        template<class Type>
        void bridgeSlave
        (
            const Field<Type>& bridgeField,
            Field<Type>& ff
        ) const;


    // Edit

        //- Correct weighting factors for moving mesh.
        bool movePoints();
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

#ifdef NoRepository
#   include "GGIInterpolation.C"
#endif

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
