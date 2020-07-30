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

enum BackgroundType {
        Solid,
        Skybox,
        SkyboxHDR,
};

enum SkyboxTexture {
        Orbital,
        Night,
};

enum SkyboxTextureHDR {
        WinterForest,
        Milkyway,
        Other,
};

#endif