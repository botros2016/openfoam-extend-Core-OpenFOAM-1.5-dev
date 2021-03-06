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

#include "Rebound.H"

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

template<class CloudType>
Foam::Rebound<CloudType>::Rebound
(
    const dictionary& dict,
    CloudType& cloud
)
:
    WallInteractionModel<CloudType>(dict, cloud, typeName),
    UFactor_(readScalar(this->coeffDict().lookup("UFactor")))
{}


// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

template<class CloudType>
Foam::Rebound<CloudType>::~Rebound()
{}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

template<class CloudType>
bool Foam::Rebound<CloudType>::active() const
{
    return true;
}


template<class CloudType>
void Foam::Rebound<CloudType>::correct
(
    const wallPolyPatch& wpp,
    const label faceId,
    vector& U
) const
{
    vector nw = wpp.faceAreas()[wpp.whichFace(faceId)];
    nw /= mag(nw);

    scalar Un = U & nw;
    vector Ut = U - Un*nw;

    if (Un > 0.0)
    {
        U -= UFactor_*2.0*Un*nw;
    }

    U -= Ut;

}


// ************************************************************************* //
