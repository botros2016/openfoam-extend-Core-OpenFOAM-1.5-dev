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

Description
    Splits mesh by making internal faces external. Uses attachDetach.

    Generates a meshModifier of the form:

    Splitter
    {
        type                       attachDetach;
        faceZoneName               membraneFaces;
        masterPatchName            masterPatch;
        slavePatchName             slavePatch;
        triggerTimes               runTime.value();
    }

    so will detach at the current time and split all faces in membraneFaces
    into masterPatch and slavePatch (which have to be present but of 0 size)

\*---------------------------------------------------------------------------*/

#include "argList.H"
#include "polyMesh.H"
#include "Time.H"
#include "polyTopoChange.H"
#include "mapPolyMesh.H"
#include "faceSet.H"
#include "attachDetach.H"
#include "polyTopoChanger.H"
#include "regionSide.H"

using namespace Foam;

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

// Calculation engine for set of faces in a mesh
typedef PrimitivePatch<face, List, const pointField&> facePatch;


// Find edge between points v0 and v1.
label findEdge(const primitiveMesh& mesh, const label v0, const label v1)
{
    const labelList& pEdges = mesh.pointEdges()[v0];

    forAll(pEdges, pEdgeI)
    {
        label edgeI = pEdges[pEdgeI];

        const edge& e = mesh.edges()[edgeI];

        if (e.otherVertex(v0) == v1)
        {
            return edgeI;
        }
    }

    FatalErrorIn
    (
        "findEdge(const primitiveMesh&, const label, const label)"
    )   << "Cannot find edge between mesh points " << v0 << " and " << v1
        << abort(FatalError);

    return -1;
}


// Checks whether patch present
void checkPatch(const polyBoundaryMesh& bMesh, const word& name)
{
    label patchI = bMesh.findPatchID(name);

    if (patchI == -1)
    {
        FatalErrorIn("checkPatch(const polyBoundaryMesh&, const word&)")
            << "Cannot find patch " << name << endl
            << "It should be present but of zero size" << endl
            << "Valid patches are " << bMesh.names()
            << exit(FatalError);
    }

    if (bMesh[patchI].size() != 0)
    {
        FatalErrorIn("checkPatch(const polyBoundaryMesh&, const word&)")
            << "Patch " << name << " is present but not of zero size"
            << exit(FatalError);
    }
}


// Main program:

int main(int argc, char *argv[])
{
    Foam::argList::noParallel();

    Foam::argList::validArgs.append("faceSet");
    Foam::argList::validArgs.append("masterPatch");
    Foam::argList::validArgs.append("slavePatch");
    Foam::argList::validOptions.insert("overwrite", "");

#   include "setRootCase.H"
#   include "createTime.H"
#   include "createPolyMesh.H"

    word setName(args.additionalArgs()[0]);
    word masterPatch(args.additionalArgs()[1]);
    word slavePatch(args.additionalArgs()[2]);
    bool overwrite = args.options().found("overwrite");

    // List of faces to split
    faceSet facesSet(mesh, setName);

    Info<< "Read " << facesSet.size() << " faces to split" << endl << endl;


    // Convert into labelList and check

    labelList faces(facesSet.toc());

    forAll(faces, i)
    {
        if (!mesh.isInternalFace(faces[i]))
        {
            FatalErrorIn(args.executable())
            << "Face " << faces[i] << " in faceSet " << setName
            << " is not an internal face."
            << exit(FatalError);
        }
    }


    // Check for empty master and slave patches
    checkPatch(mesh.boundaryMesh(), masterPatch);
    checkPatch(mesh.boundaryMesh(), slavePatch);


    //
    // Find 'side' of all faces on splitregion. Uses regionSide which needs
    // set of edges on side of this region. Use PrimitivePatch to find these.
    //

    IndirectList<face> zoneFaces(mesh.faces(), faces);

    // Addressing on faces only in mesh vertices.
    facePatch fPatch(zoneFaces(), mesh.points());

    const labelList& meshPoints = fPatch.meshPoints();

    // Mark all fence edges : edges on boundary of fPatch but not on boundary
    // of polyMesh
    labelHashSet fenceEdges(fPatch.size());

    const labelListList& allEdgeFaces = fPatch.edgeFaces();

    forAll(allEdgeFaces, patchEdgeI)
    {
        if (allEdgeFaces[patchEdgeI].size() == 1)
        {
            const edge& e = fPatch.edges()[patchEdgeI];

            label edgeI =
                findEdge
                (
                    mesh,
                    meshPoints[e.start()],
                    meshPoints[e.end()]
                );

            fenceEdges.insert(edgeI);
        }
    }

    // Find sides reachable from 0th face of faceSet
    label startFaceI = faces[0];

    regionSide regionInfo
    (
        mesh,
        facesSet,
        fenceEdges,
        mesh.faceOwner()[startFaceI],
        startFaceI
    );

    // Determine flip state for all faces in faceSet
    boolList zoneFlip(faces.size());

    forAll(faces, i)
    {
        zoneFlip[i] = !regionInfo.sideOwner().found(faces[i]);
    }


    // Create and add face zones and mesh modifiers
    List<pointZone*> pz(0);
    List<faceZone*> fz(1);
    List<cellZone*> cz(0);

    fz[0] =
        new faceZone
        (
            "membraneFaces",
            faces,
            zoneFlip,
            0,
            mesh.faceZones()
        );

    Info << "Adding point and face zones" << endl;
    mesh.addZones(pz, fz, cz);

    polyTopoChanger splitter(mesh);
    splitter.setSize(1);

    // Add the sliding interface mesh modifier to start working at current
    // time
    splitter.set
    (
        0,
        new attachDetach
        (
            "Splitter",
            0,
            splitter,
            "membraneFaces",
            masterPatch,
            slavePatch,
            scalarField(1, runTime.value())
        )
    );

    Info<< nl << "Constructed topologyModifier:" << endl;
    splitter[0].writeDict(Info);

    if (!overwrite)
    {
        runTime++;
    }

    splitter.changeMesh();

    Info << nl << "Writing polyMesh" << endl;
    if (!mesh.write())
    {
        FatalErrorIn(args.executable())
            << "Failed writing polyMesh."
            << exit(FatalError);
    }

    Info<< nl << "end" << endl;
    return 0;
}


// ************************************************************************* //
