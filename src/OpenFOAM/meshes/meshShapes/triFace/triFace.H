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
    Foam::triFace

Description
    A triangle face primitive using a FixedList.

SourceFiles
    triFaceI.H

\*---------------------------------------------------------------------------*/

#ifndef triFace_H
#define triFace_H

#include "FixedList.H"
#include "edgeList.H"
#include "pointHit.H"
#include "intersection.H"
#include "pointField.H"
#include "triPointRef.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

class face;

// Forward declaration of friend functions and operators

class triFace;
inline bool operator==(const triFace&, const triFace&);
inline bool operator!=(const triFace&, const triFace&);


/*---------------------------------------------------------------------------*\
                           class triFace Declaration
\*---------------------------------------------------------------------------*/

class triFace
:
    public FixedList<label, 3>
{

public:

    // Constructors

        //- Construct null
        inline triFace();

        //- Construct from three point labels
        inline triFace
        (
            const label a,
            const label b,
            const label c
        );

        //- Construct from a UList<label>
        explicit inline triFace(const UList<label>&);

        //- Construct from Istream
        inline triFace(Istream&);


    // Member Functions

        //- Collapse face by removing duplicate point labels
        //  return the collapsed size, set collapsed point labels to -1
        inline label collapse();

        //- Return the edge direction on the face
        //  - +1: forward (counter-clockwise) on the face
        //  - -1: reverse (clockwise) on the face
        //  -  0: edge not found on the face
        inline int edgeDirection(const edge&) const;


        // Properties

            //- Return the points corresponding to this face
            inline pointField points(const pointField& points) const;

            //- Return triangle as a face
            inline face triFaceFace() const;

            //- Return number of edges
            inline label nEdges() const;

            //- Return edges
            inline edgeList edges() const;

            //- Return centre (centroid)
            inline point centre(const pointField&) const;

            //- Return scalar magnitude
            inline scalar mag(const pointField&) const;

            //- Return vector normal
            inline vector normal(const pointField&) const;

            //- Return swept-volume
            inline scalar sweptVol
            (
                const pointField& oldPoints,
                const pointField& newPoints
            ) const;

            //- Return point intersection with a ray starting at p, with
            //  direction n.
            inline pointHit ray
            (
                const point& p,
                const vector& q,
                const pointField& points,
                const intersection::algorithm = intersection::FULL_RAY,
                const intersection::direction dir = intersection::VECTOR
            ) const;

            //- Return the triangle
            inline triPointRef tri(const pointField&) const;

        //- compare triFaces
        //  -  0: different
        //  - +1: identical
        //  - -1: same face, but different orientation
        static inline int compare(const triFace&, const triFace&);

    // Friend Operators

        friend bool operator==(const triFace&, const triFace&);
        friend bool operator!=(const triFace&, const triFace&);
};


//- Hash<triFace> specialisation
//  Simple commutative hash.
template<>
inline label Hash<triFace>::operator()(const triFace& t) const
{
    return (t[0]*t[1]*t[2] + t[0]+t[1]+t[2]);
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#include "triFaceI.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
