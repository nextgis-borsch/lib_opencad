/*******************************************************************************
 *  Project: libopencad
 *  Purpose: OpenSource CAD formats support library
 *  Author: Alexandr Borzykh, mush3d at gmail.com
 *  Author: Dmitry Baryshnikov, bishop.dev@gmail.com
 *  Language: C++
 *******************************************************************************
 *  The MIT License (MIT)
 *
 *  Copyright (c) 2016 Alexandr Borzykh
 *  Copyright (c) 2016 NextGIS, <info@nextgis.com>
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to deal
 *  in the Software without restriction, including without limitation the rights
 *  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *  copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in all
 *  copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *  SOFTWARE.
 *******************************************************************************/

#include "opencad_api.h"
#include "cadgeometry.h"

#include <cstddef>
#include <iostream>
#include <iomanip>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <memory>
#include <cadcolors.h>

using namespace std;

static int Usage(const char* pszErrorMsg = nullptr)
{
    cout << "Usage: cadinfo [--summary][--help][--formats][--version]\n"
            "               file_name \n";

    if( pszErrorMsg != nullptr )
    {
        cerr << "\nFAILURE: " << pszErrorMsg << "\n";
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

static int Version()
{
    cout << "cadinfo was compiled against libopencad "
         << OCAD_VERSION << "\n"
              << "and is running against libopencad "
         << GetVersionString() << "\n";

    return EXIT_SUCCESS;
}

static int Formats()
{
    cerr << GetCADFormats() << "\n";

    return EXIT_SUCCESS;
}

int main(int argc, char *argv[])
{
    if( argc < 1 )
       return -argc;
    else if(argc == 1)
        return Usage();

    bool bSummary = false;
    const char  *pszCADFilePath = nullptr;

    for( int iArg = 1; iArg < argc; ++iArg)
    {
        if (strcmp(argv[iArg],"-h")==0 || strcmp(argv[iArg],"--help")==0)
        {
            return Usage();
        }
        else if(strcmp(argv[iArg],"-f")==0 || strcmp(argv[iArg],"--formats")==0)
        {
            return Formats ();
        }
        else if(strcmp(argv[iArg],"-v")==0 || strcmp(argv[iArg],"--version")==0)
        {
            return Version();
        }
        else if(strcmp(argv[iArg],"-so")==0 || strcmp(argv[iArg],"--summary")==0)
        {
            bSummary = true;
        }
        else
        {
            pszCADFilePath = argv[iArg];
        }
    }

    CADFile *pCADFile = OpenCADFile( pszCADFilePath, CADFile::OpenOptions::READ_ALL, true );

    if (pCADFile == nullptr)
    {
        cerr << "Open CAD file " << (pszCADFilePath == NULL ? "NULL" : pszCADFilePath) << " failed.\n";
        return EXIT_FAILURE;
    }

    const CADHeader& header = pCADFile->getHeader ();
    header.print ();
    cout << "\n";

    const CADClasses& classes = pCADFile->getClasses ();
    classes.print ();
    cout << "\n";

    // print NOD info
    cout << "Named Object Dictionary records:\n";
    CADDictionary oNOD = pCADFile->GetNOD();
    for( size_t i = 0; i < oNOD.getRecordsCount (); ++i )
    {
        cout << "RECORD TYPE: ";
        if( oNOD.getRecord (i).second->getType () == CADObject::ObjectType::XRECORD ) 
            cout << "XRECORD ";
        else if( oNOD.getRecord (i).second->getType () == CADObject::ObjectType::DICTIONARY ) 
            cout << "DICTIONARY ";

        cout << "RECORD NAME: " << oNOD.getRecord (i).first << "\n";
        if( oNOD.getRecord (i).second->getType () == CADObject::ObjectType::XRECORD )
        {
            CADXRecord * cadxRecord = static_cast<CADXRecord*>(oNOD.getRecord (i).second.get());
            cout << "DATA: " << cadxRecord->getRecordData () << "\n";
        }
    }
    cout << "\n";

    int attdefCount = 0;
    int attribCount = 0;
    int polylinesPface = 0;
    int face3dsCount = 0;
    int raysCount = 0;
    int xlinesCount = 0;
    int mlinesCount = 0;
    int mtextsCount = 0;
    int imagesCount = 0;
    int solidsCount = 0;
    int splinesCount = 0;
    int circlesCount = 0;
    int linesCount = 0;
    int ellipsesCount = 0;
    int plineCount = 0;
    int pline3dCount = 0;
    int pointCount = 0;
    int arcCount = 0;
    int textCount = 0;
    cout << "Layers count: " << pCADFile->GetLayersCount() << "\n";

    size_t i,j;
    for ( i = 0; i < pCADFile->GetLayersCount(); ++i )
    {
        CADLayer &layer = pCADFile->GetLayer( i );
        cout << i+1 << ". Layer " << layer.getName () << " contains "
             << layer.getGeometryCount () << " geometries\n";

        auto attribs = layer.getAttributesTags ();
        if(!attribs.empty ())
            cout << "Layer attributes:\n";
        for( const auto& attr : attribs )
        {
            cout << "Attribute: [" << attr << "]\n";
        }

        for ( j = 0; j < layer.getGeometryCount (); ++j )
        {
            unique_ptr<CADGeometry> geom(layer.getGeometry (j));

            if ( geom == nullptr )
                continue;

            if(!bSummary)
            {
                geom->print();
                ios init(NULL);
                init.copyfmt(cout);
                cout << "Entity color: #";
                cout << hex << setw(2) << setfill('0') << (int)geom->getColor().R << (int)geom->getColor().G << (int)geom->getColor().B;
                cout << "ff" << endl;
                cout.copyfmt(init);
            }

            auto geom_attrs = geom->getBlockAttributes ();
            for( const auto& attdef : geom_attrs )
            {
                cout << "Attrib name: " << attdef.getTag ()
                     << "\nAttrib val: " << attdef.getTextValue () << "\n";
            }

            switch ( geom->getType() )
            {
            case CADGeometry::CIRCLE:
                ++circlesCount;
                break;

            case CADGeometry::LWPOLYLINE:
                ++plineCount;
                break;

            case CADGeometry::POLYLINE3D:
                ++pline3dCount;
                break;

            case CADGeometry::POLYLINE_PFACE:
                ++polylinesPface;
                break;

            case CADGeometry::ARC:
                ++arcCount;
                break;
            case CADGeometry::POINT:
                ++pointCount;
                break;
            case CADGeometry::ELLIPSE:
                ++ellipsesCount;
                break;

            case CADGeometry::LINE:
                ++linesCount;
                break;
            case CADGeometry::SPLINE:
                ++splinesCount;
                break;

            case CADGeometry::TEXT:
                ++textCount;
                break;

            case CADGeometry::SOLID:
                ++solidsCount;
                break;

            case CADGeometry::MTEXT:
                ++mtextsCount;
                break;

            case CADGeometry::MLINE:
                ++mlinesCount;
                break;

            case CADGeometry::XLINE:
                ++xlinesCount;
                break;

            case CADGeometry::RAY:
                ++raysCount;
                break;

            case CADGeometry::FACE3D:
                ++face3dsCount;
                break;
            case CADGeometry::ATTDEF:
                ++attdefCount;
                break;
            case CADGeometry::ATTRIB:
                ++attribCount;
                break;
            case CADGeometry::UNDEFINED:
            case CADGeometry::HATCH:
                break;
            }
        }

        for ( j = 0; j < layer.getImageCount (); ++j )
        {
            unique_ptr<CADImage> image(layer.getImage (j));

            if ( image == nullptr )
                continue;

            if(!bSummary)
                image->print ();

            ++imagesCount;

        }
    }

    cout << "\n============== geometry summary =============="
         << "\nPolylines Pface: " << polylinesPface
         << "\n3DFaces: " << face3dsCount
         << "\nRays: " << raysCount
         << "\nXLines: " << xlinesCount
         << "\nMLines: " << mlinesCount
         << "\nMTexts: " << mtextsCount
         << "\nImages: " << imagesCount
         << "\nSolids: " << solidsCount
         << "\nPoints: " << pointCount
         << "\nEllipses: " << ellipsesCount
         << "\nLines count: " << linesCount
         << "\nPlines count: " << plineCount
         << "\nPlines3d count: " << pline3dCount
         << "\nSplines count: " << splinesCount
         << "\nCircles count: " << circlesCount
         << "\nArcs count: " << arcCount
         << "\nTexts count: " << textCount
         << "\nAttdefs count: " << attdefCount
         << "\nAttribs count: " << attribCount << "\n";

    delete( pCADFile );
}
