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

Application
    kivaToFoam

Description
    Converts a KIVA3v grid to FOAM format

\*---------------------------------------------------------------------------*/

#include "argList.H"
#include "Time.H"
#include "polyMesh.H"
#include "IFstream.H"
#include "OFstream.H"
#include "cellShape.H"
#include "cellModeller.H"
#include "preservePatchTypes.H"
#include "emptyPolyPatch.H"
#include "wallPolyPatch.H"
#include "symmetryPolyPatch.H"
#include "wedgePolyPatch.H"
#include "cyclicPolyPatch.H"
#include "mathematicalConstants.H"

using namespace Foam;

// Supported KIVA versions
enum kivaVersions
{
    kiva3,
    kiva3v
};

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //
// Main program:

int main(int argc, char *argv[])
{
    argList::noParallel();
    argList::validOptions.insert("file", "fileName");
    argList::validOptions.insert("version", "[kiva3|kiva3v]");
    argList::validOptions.insert("zHeadMin", "scalar");

#   include "setRootCase.H"
#   include "createTime.H"

    fileName kivaFileName("otape17");
    if (args.options().found("file"))
    {
        kivaFileName = args.options()["file"];
    }

    kivaVersions kivaVersion = kiva3v;
    if (args.options().found("version"))
    {
        word kivaVersionName = args.options()["version"];

        if (kivaVersionName == "kiva3")
        {
            kivaVersion = kiva3;
        }
        else if (kivaVersionName == "kiva3v")
        {
            kivaVersion = kiva3v;
        }
        else
        {
            FatalErrorIn("main(int argc, char *argv[])")
                << "KIVA file version " << kivaVersionName << " not supported"
                << exit(FatalError);

            args.printUsage();

            FatalError.exit(1);
        }
    }

    scalar zHeadMin = -GREAT;
    if (args.options().found("zHeadMin"))
    {
        zHeadMin = atof(args.options()["zHeadMin"].c_str());
    }

#   include "readKivaGrid.H"

    Info << "End\n" << endl;

    return 0;
}


// ************************************************************************* //
