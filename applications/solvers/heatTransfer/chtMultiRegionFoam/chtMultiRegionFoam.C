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
    chtMultiRegionFoam

Description
    Combination of heatConductionFoam and buoyantFoam for conjugate heat
    transfer between a solid region and fluid region

\*---------------------------------------------------------------------------*/

#include "fvCFD.H"
#include "basicThermo.H"
#include "compressible/RASModel/RASModel.H"
#include "fixedGradientFvPatchFields.H"
#include "regionProperties.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#include "solveContinuityEquation.C"
#include "solveMomentumEquation.C"
#include "compressibleContinuityErrors.C"
#include "solvePressureDifferenceEquation.C"
#include "solveEnthalpyEquation.C"
#include "compressibleCourantNo.C"

int main(int argc, char *argv[])
{

#   include "setRootCase.H"
#   include "createTime.H"

    regionProperties rp(runTime);

#   include "createFluidMeshes.H"
#   include "createSolidMeshes.H"

#   include "createFluidFields.H"

#   include "createSolidFields.H"

#   include "initContinuityErrs.H"

#   include "readTimeControls.H"

    if (fluidRegions.size())
    {
#       include "compressibleMultiRegionCourantNo.H"
#       include "setInitialDeltaT.H"
    }

    while(runTime.run())
    {
#       include "readTimeControls.H"

        if (fluidRegions.size())
        {
#           include "compressibleMultiRegionCourantNo.H"
#           include "setDeltaT.H"
        }

        runTime++;

        Info<< "Time = " << runTime.timeName() << nl << endl;

        forAll(fluidRegions, i)
        {
            Info<< "\nSolving for fluid region "
                << fluidRegions[i].name() << endl;
#           include "readFluidMultiRegionPISOControls.H"
#           include "solveFluid.H"
        }

        forAll(solidRegions, i)
        {
            Info<< "\nSolving for solid region "
                << solidRegions[i].name() << endl;
#           include "readSolidMultiRegionPISOControls.H"
#           include "solveSolid.H"
        }

        runTime.write();

        Info<< "ExecutionTime = " << runTime.elapsedCpuTime() << " s"
            << "  ClockTime = " << runTime.elapsedClockTime() << " s"
            << nl << endl;
    }

    Info << "End\n" << endl;

    return(0);
}


// ************************************************************************* //
