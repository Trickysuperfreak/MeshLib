using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Runtime.InteropServices;
using System.Text;
using static MR.DotNet.MeshComponents;

namespace MR.DotNet
{
    public struct MeshComponentsMap
    {
        public Face2RegionMap FaceMap;
        public int NumComponents = 0;

        public MeshComponentsMap(Face2RegionMap faceMap, int numComponents )
        {
            FaceMap = faceMap;
            NumComponents = numComponents;
        }
    };

    public struct MeshRegions
    {
        public BitSet faces;
        public int numRegions = 0;

        public MeshRegions( BitSet faces, int numRegions )
        {
            this.faces = faces;
            this.numRegions = numRegions;
        }
    };

    public enum FaceIncidence
    {
        PerEdge, /// face can have neighbor only via edge
        PerVertex /// face can have neighbor via vertex
    };

    public class Face2RegionMap : List<RegionId>, IDisposable 
    {
        unsafe internal MRFace2RegionMap* mrFace2RegionMap_;
        private bool disposed_ = false;

        [DllImport("MRMeshC.dll", CharSet = CharSet.Auto)]
        unsafe private static extern void mrMeshComponentsAllComponentsMapFree(MRFace2RegionMap* map);

        unsafe internal Face2RegionMap( MRFace2RegionMap* mrFace2RegionMap )
            : base()
        {
            if (mrFace2RegionMap == null)
                return;

            mrFace2RegionMap_ = mrFace2RegionMap;
            for (int i = 0; i < (int)mrFace2RegionMap->size; i++)
            {
                Add(new RegionId(Marshal.ReadInt32(IntPtr.Add(mrFace2RegionMap->data, i * sizeof(int)))));
            }
        }

        public void Dispose()
        {
            Dispose(true);
            GC.SuppressFinalize(this);
        }

        unsafe protected virtual void Dispose(bool disposing)
        {
            if (!disposed_)
            {
                if (mrFace2RegionMap_ != (MRFace2RegionMap*)IntPtr.Zero)
                {
                    mrMeshComponentsAllComponentsMapFree(mrFace2RegionMap_);
                    mrFace2RegionMap_ = (MRFace2RegionMap*)IntPtr.Zero;
                }

                disposed_ = true;
            }
        }

        ~Face2RegionMap()
        {
            Dispose(false);
        }
    }

    public class MeshComponents
    {
        [StructLayout(LayoutKind.Sequential)]
        internal struct MRFace2RegionMap
        {
            public IntPtr data = IntPtr.Zero;
            public ulong size = 0;
            public IntPtr reserved = IntPtr.Zero;
            public MRFace2RegionMap() { }
        }

        [StructLayout(LayoutKind.Sequential)]
        unsafe internal struct MRMeshComponentsMap
        {
            public MRFace2RegionMap* faceMap;
            public int numComponents = 0;
            public MRMeshComponentsMap() { }
        };

        [StructLayout(LayoutKind.Sequential)]
        internal struct MRMeshRegions
        {
            public IntPtr faces;
            public int numRegions = 0;
            public MRMeshRegions() { }
        };

        [DllImport("MRMeshC.dll", CharSet = CharSet.Auto)]
        unsafe private static extern MRMeshComponentsMap mrMeshComponentsGetAllComponentsMap(ref MRMeshPart mp, FaceIncidence incidence);

        [DllImport("MRMeshC.dll", CharSet = CharSet.Auto)]
        unsafe private static extern MRMeshRegions mrMeshComponentsGetLargeByAreaRegions(ref MRMeshPart mp, MRFace2RegionMap* face2RegionMap, int numRegions, float minArea);



        static public void TestFunction()
        {
            Console.WriteLine("TestClass.TestFunction");
        }

        unsafe static public MeshComponentsMap GetAllComponentsMap(MeshPart mp, FaceIncidence incidence)
        {
            var mrMap = mrMeshComponentsGetAllComponentsMap(ref mp.mrMeshPart, incidence);
            var res = new MeshComponentsMap
            {
                FaceMap = new Face2RegionMap(mrMap.faceMap),
                NumComponents = (int)mrMap.numComponents
            };

            return res;
        }

        unsafe static public MeshRegions GetLargeByAreaRegions(MeshPart mp, Face2RegionMap face2RegionMap, int numRegions, float minArea)
        {
            var mrRegions = mrMeshComponentsGetLargeByAreaRegions(ref mp.mrMeshPart, face2RegionMap.mrFace2RegionMap_, numRegions, minArea);
            return new MeshRegions
            {
                faces = new BitSet(mrRegions.faces),
                numRegions = mrRegions.numRegions
            };
        }
    }
}
