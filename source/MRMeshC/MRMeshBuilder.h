#pragma once
#include "MRMeshFwd.h"
#include "MRVector.h"

MR_EXTERN_C_BEGIN

MRMESHC_API int mrMeshBuilderUniteCloseVertices( MRMesh* mesh, float closeDist, bool uniteOnlyBd, MRVertMap* optionalVertOldToNew );

MRMESHC_API void mrMeshBuilderVertOld2NewFree( MRVertMap* vertOldToNew );
MR_EXTERN_C_END
