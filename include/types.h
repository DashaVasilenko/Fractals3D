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
        Julia4,
        Sierpinski1,
        Sierpinski2,
        MengerSponge1,
};

// Type1 - one color without traps
// Type2 - three colors with traps: Oxy, Oyz, Oyz planes and point (0, 0, 0)
// Type3 - one color with traps: |z|² and z_x
// Type4 - Type1 + trap: albedo.x = 1.0-10.0*trap.x;
// Type5 - Type3 + trap: albedo.x = 1.0-10.0*trap.x;
// в menger и serpinski они отличаются от этих
// Type6 - во всех щейдерах разный
enum ColoringType {
        Type1,
        Type2,
        Type3,
        Type4, 
        Type5,
        Type6,
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