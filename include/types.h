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
        Skybox,
        SkyboxHDR,
};

enum SkyboxTexture {
        Orbital,
        Night,
        Other,
};

enum SkyboxTextureHDR {
        WinterForest,
        Milkyway,
        OtherHDR,
};

#endif