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

#include "fieldAverageItem.H"
#include "volFields.H"
#include "OFstream.H"

// * * * * * * * * * * * * * Private Member Functions  * * * * * * * * * * * //

template<class Type>
void Foam::fieldAverage::addMeanField
(
    const label fieldi,
    wordList& meanFieldList
) const
{
    if (faItems_[fieldi].mean())
    {
        typedef GeometricField<Type, fvPatchField, volMesh> fieldType;

        const word& fieldName = faItems_[fieldi].fieldName();

        const word meanFieldName = fieldName + EXT_MEAN;

        Info<< "Reading/calculating field " << meanFieldName << nl << endl;

        if (obr_.foundObject<fieldType>(meanFieldName))
        {
            meanFieldList[fieldi] = meanFieldName;
        }
        else if (obr_.found(meanFieldName))
        {
            Info<< "Cannot allocate average field " << meanFieldName
                << " since an object with that name already exists."
                << " Disabling averaging." << nl << endl;
            meanFieldList[fieldi] = word::null;
        }
        else
        {
            const fieldType& baseField =
                obr_.lookupObject<fieldType>(fieldName);

            fieldType* fPtr = new fieldType
            (
                IOobject
                (
                    meanFieldName,
                    obr_.time().timeName(),
                    obr_,
                    IOobject::READ_IF_PRESENT,
                    IOobject::NO_WRITE
                ),
                baseField
            );

            // Store on registry
            fPtr->store();

            meanFieldList[fieldi] = meanFieldName;
        }
    }
}


template<class Type1, class Type2>
void Foam::fieldAverage::addPrime2MeanField
(
    const label fieldi,
    const wordList& meanFieldList,
    wordList& prime2MeanFieldList
) const
{
    if (faItems_[fieldi].mean() && meanFieldList[fieldi] != word::null)
    {
        typedef GeometricField<Type1, fvPatchField, volMesh> fieldType1;
        typedef GeometricField<Type2, fvPatchField, volMesh> fieldType2;

        const word& fieldName = faItems_[fieldi].fieldName();

        const word meanFieldName = fieldName + EXT_PRIME2MEAN;
        Info<< "Reading/calculating field " << meanFieldName << nl << endl;

        if (obr_.foundObject<fieldType2>(meanFieldName))
        {
            prime2MeanFieldList[fieldi] = meanFieldName;
        }
        else if (obr_.found(meanFieldName))
        {
            Info<< "Cannot allocate average field " << meanFieldName
                << " since an object with that name already exists."
                << " Disabling averaging." << nl << endl;
            prime2MeanFieldList[fieldi] = word::null;
        }
        else
        {
            const fieldType1& baseField =
                obr_.lookupObject<fieldType1>(fieldName);
            const fieldType1& meanField =
                obr_.lookupObject<fieldType1>(meanFieldList[fieldi]);

            fieldType2* fPtr = new fieldType2
            (
                IOobject
                (
                    meanFieldName,
                    obr_.time().timeName(),
                    obr_,
                    IOobject::READ_IF_PRESENT,
                    IOobject::NO_WRITE
                ),
                sqr(baseField) - sqr(meanField)
            );

            // Store on registry
            fPtr->store();

            prime2MeanFieldList[fieldi] = meanFieldName;
        }
    }
}


template<class Type>
void Foam::fieldAverage::calculateMeanFields(const wordList& meanFieldList)
const
{
    typedef GeometricField<Type, fvPatchField, volMesh> fieldType;

    const scalar dt = obr_.time().deltaT().value();

    forAll(faItems_, i)
    {
        if (faItems_[i].mean() && meanFieldList[i] != word::null)
        {
            const word& fieldName = faItems_[i].fieldName();
            const fieldType& baseField =
                obr_.lookupObject<fieldType>(fieldName);
            fieldType& meanField = const_cast<fieldType&>
            (
                obr_.lookupObject<fieldType>(meanFieldList[i])
            );

            scalar alpha = 0.0;
            scalar beta = 0.0;
            if (faItems_[i].timeBase())
            {
                 alpha = (totalTime_[i] - dt)/totalTime_[i];
                 beta = dt/totalTime_[i];
            }
            else
            {
                alpha = scalar(totalIter_[i] - 1)/scalar(totalIter_[i]);
                beta = 1.0/scalar(totalIter_[i]);
            }

            meanField = alpha*meanField + beta*baseField;
        }
    }
}


template<class Type1, class Type2>
void Foam::fieldAverage::calculatePrime2MeanFields
(
    const wordList& meanFieldList,
    const wordList& prime2MeanFieldList
) const
{
    typedef GeometricField<Type1, fvPatchField, volMesh> fieldType1;
    typedef GeometricField<Type2, fvPatchField, volMesh> fieldType2;

    const scalar dt = obr_.time().deltaT().value();

    forAll(faItems_, i)
    {
        if
        (
            faItems_[i].prime2Mean()
         && meanFieldList[i] != word::null
         && prime2MeanFieldList[i] != word::null
        )
        {
            const word& fieldName = faItems_[i].fieldName();
            const fieldType1& baseField =
                obr_.lookupObject<fieldType1>(fieldName);
            const fieldType1& meanField =
                obr_.lookupObject<fieldType1>(meanFieldList[i]);
            fieldType2& prime2MeanField = const_cast<fieldType2&>
            (
                obr_.lookupObject<fieldType2>(prime2MeanFieldList[i])
            );

            scalar alpha = 0.0;
            scalar beta = 0.0;
            if (faItems_[i].timeBase())
            {
                alpha = (totalTime_[i] - dt)/totalTime_[i];
                beta = dt/totalTime_[i];
            }
            else
            {
                alpha = scalar(totalIter_[i] - 1)/scalar(totalIter_[i]);
                beta = 1.0/scalar(totalIter_[i]);
            }

            prime2MeanField =
                alpha*prime2MeanField
              + beta*sqr(baseField)
              - sqr(meanField);
        }
    }
}


template<class Type1, class Type2>
void Foam::fieldAverage::addMeanSqrToPrime2Mean
(
    const wordList& meanFieldList,
    const wordList& prime2MeanFieldList
) const
{
    typedef GeometricField<Type1, fvPatchField, volMesh> fieldType1;
    typedef GeometricField<Type2, fvPatchField, volMesh> fieldType2;

    forAll(faItems_, i)
    {
        if
        (
            faItems_[i].prime2Mean()
         && meanFieldList[i] != word::null
         && prime2MeanFieldList[i] != word::null
        )
        {
            const fieldType1& meanField =
                obr_.lookupObject<fieldType1>(meanFieldList[i]);
            fieldType2& prime2MeanField = const_cast<fieldType2&>
            (
                obr_.lookupObject<fieldType2>(prime2MeanFieldList[i])
            );

            prime2MeanField += sqr(meanField);
        }
    }
}


template<class Type>
void Foam::fieldAverage::writeFieldList(const wordList& fieldList) const
{
    typedef GeometricField<Type, fvPatchField, volMesh> fieldType;

    forAll(fieldList, i)
    {
        if (fieldList[i] != word::null)
        {
            const fieldType& f = obr_.lookupObject<fieldType>(fieldList[i]);
            f.write();
        }
    }
}


// ************************************************************************* //
