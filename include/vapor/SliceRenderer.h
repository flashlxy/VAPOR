#ifndef SLICERENDERER_H
#define SLICERENDERER_H

#include <vapor/glutil.h>

#ifdef Darwin
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif

#include <vapor/DataMgr.h>
#include <vapor/utils.h>
#include <vapor/Renderer.h>

namespace VAPoR {

    class RENDER_API SliceRenderer : public Renderer
    {
    public:
        SliceRenderer(
            const ParamsMgr* pm,
            string winName,
            string dataSetName,
            string instName,
            DataMgr* dataMgr
        );

        static string GetClassType() {
            return("Slice");
        }

        virtual ~SliceRenderer();

    protected:
        virtual int _initializeGL();
        virtual int _paintGL(bool fast);

    private:
        struct{
            string varName;
            string heightVarName;
            size_t ts;
            int refinementLevel;
            int compressionLevel;
            int textureSampleRate;
            int orientation;
            std::vector<float> tf_lut;
            std::vector<double> tf_minMax;
            std::vector<double> boxMin, boxMax;
        } _cacheParams;

        void _initVAO();
        void _initTexCoordVBO();
        void _initVertexVBO();

        bool _isColormapCacheDirty() const;
        bool _isDataCacheDirty() const;
        int  _saveCacheParams();
        void _resetColormapCache();
        int  _resetDataCache();
        void _initTextures();
        int  _saveTextureData();
        void _getSampleCoordinates(
            std::vector<double> &coords,
            int i,
            int j
        ) const;

        void _configureShader();
        void _resetState();
        void _initializeState();

        void _setVertexPositions();
        void _setXYVertexPositions(
            std::vector<double> min, 
            std::vector<double> max
        );
        void _setXZVertexPositions(
            std::vector<double> min, 
            std::vector<double> max
        );
        void _setYZVertexPositions(
            std::vector<double> min, 
            std::vector<double> max
        );
        
        bool           _initialized;

        GLuint         _colorMapTextureID;
        GLuint         _dataValueTextureID;

        int            _textureWidth;
        int            _textureHeight;
        float*         _dataValues;
        std::vector<double>        _vertexPositions;

        GLuint _VAO;
        GLuint _vertexVBO;
        GLuint _texCoordVBO;
        GLuint _EBO;

        int _colorMapSize;
    };
            
};

#endif
