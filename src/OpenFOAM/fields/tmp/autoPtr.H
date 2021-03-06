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
    Foam::autoPtr

Description
    An auto-pointer similar to the STL auto_ptr but with automatic casting
    to a reference to the type and with pointer allocation checking on access.

SourceFiles
    autoPtrI.H

\*---------------------------------------------------------------------------*/

#ifndef autoPtr_H
#define autoPtr_H

#include <cstddef>

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                           Class autoPtr Declaration
\*---------------------------------------------------------------------------*/

template<class T>
class autoPtr
{
    // Public data

        //- Pointer to object
        mutable T* ptr_;


public:

    // Constructors

        //- Store object pointer
        inline explicit autoPtr(T* = NULL);

        //- Construct as copy by transfering pointer to this autoPtr and
        //  setting the arguments pointer to NULL
        inline autoPtr(const autoPtr<T>&);


    // Destructor

        //- Delete object if pointer is not NULL
        inline ~autoPtr();


    // Member Functions

        // Check

            //- Is the autoPtr valid, i.e. is the pointer set
            inline bool valid() const;


        // Edit

            //- Return object pointer for reuse
            inline T* ptr();

            //- Set pointer to that given.
            //  If object pointer already set issue a FatalError.
            inline void set(T* p);

            //- If object pointer already set delete object and
            //  set pointer to that given
            inline void reset(T* p = NULL);

            //- If object pointer points to valid object:
            //  delete object and set pointer to NULL
            inline void clear();


    // Member operators

        inline T& operator()();
        inline const T& operator()() const;

        //inline Tref operator*();
        //inline const Tref operator*() const;

        inline operator const T&() const;

        //- Return object pointer
        inline T* operator->();

        //- Return const object pointer
        inline const T* operator->() const;

        inline void operator=(const autoPtr<T>&);
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#include "autoPtrI.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
