#ifndef __TYPES__
#define __TYPES__

enum ExportType {
        Png,
        Bmp,
        Jpg,
        Tga,
        //Obj,
};

enum FractalType {
        Test,
        Mandelbulb,
        Monster,
        Julia1,
        Julia2,
        Julia3,
};

// Type1 - one color without traps
// Type2 - three colors with traps: Oxy, Oyz, Oyz planes and point (0, 0, 0)
// Type3 - one color with traps: |z|² and z_x
enum ColoringType {
        Type1,
        Type2,
        Type3,
};

enum CubemapSide {
        Front,
        Back, 
        Up,
        Down,
        Left,
        Right,
};

enum BackgroundType {
        Solid,
        SolidWithSun,
        Skybox,
        SkyboxHDR,
};

enum SkyboxTexture {
        Orbital,
        Night,
        PalmTrees,
        CoitTower,
        MountainPath,
        NightPath,
        Vasa,
        Other,
};

enum SkyboxTextureHDR {
        WinterForest,
        Milkyway,
        GrandCanyon,
        IceLake,
        Factory,
        TopangaForest,
        TropicalBeach,
        OtherHDR,
};

#endif