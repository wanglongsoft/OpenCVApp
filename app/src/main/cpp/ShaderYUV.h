//
// Created by 王龙 on 2020-03-25.
//

#ifndef OPENGLNDK_SHADERYUV_H
#define OPENGLNDK_SHADERYUV_H

#include <GLES3/gl3.h>
#include <EGL/egl.h>
#include <math.h>
#include "GlobalContexts.h"
#include "LogUtils.h"

#define GET_STR(x) #x

class ShaderYUV {
public:
    ShaderYUV(GlobalContexts *global_context);
    ~ShaderYUV();
    GLuint LoadShader(GLenum type, const char *shaderSrc);
    GLuint LoadProgram(const char *vShaderStr, const char *fShaderStr);
    int CreateProgram();
    void changeVideoRotation();
    void Render(uint8_t *data[]);
    void blackWhiteRender();
    void warmColorRender();
    void coolColorRender();
    void normalColorRender();
    void setVideoSize(int width, int height);
    void setWindowSize(int width, int height);
    void initDefMatrix();
    void orthoM(float m[], int mOffset,
                float left, float right, float bottom, float top,
                float near, float far);

    float adjustRatio(float ratio, float worldRatio);

    const char *vertex_shader_texture_code = GET_STR(
            attribute vec4 aPosition;//输入的顶点坐标，会在程序指定将数据输入到该字段
            attribute vec2 aTextCoord;//输入的纹理坐标，会在程序指定将数据输入到该字段
            varying vec2 vTextCoord;//输出的纹理坐标
            uniform mat4 uMatrix;
            void main() {
                //
                vTextCoord = aTextCoord;
                //直接把传入的坐标值作为传入渲染管线。gl_Position是OpenGL内置的
                gl_Position = aPosition * uMatrix;
            }
    );

    const char *fragment_shader_texture_code = GET_STR(
            precision mediump float;
            varying vec2 vTextCoord;
            uniform int vChangeType;
            uniform int vSplitScreen;
            //输入的yuv三个纹理
            uniform sampler2D yTexture;//采样器
            uniform sampler2D uTexture;//采样器
            uniform sampler2D vTexture;//采样器

            vec4 yuvToRGBAColor(sampler2D ySampler, sampler2D uSampler, sampler2D vSampler, vec2 textCoord) {
                vec3 yuv;
                vec3 rgb;
                //分别取yuv各个分量的采样纹理
                yuv.x = texture2D(ySampler, textCoord).g;
                yuv.y = texture2D(uSampler, textCoord).g - 0.5;
                yuv.z = texture2D(vSampler, textCoord).g - 0.5;
                rgb = mat3(
                        1.0, 1.0, 1.0,
                        0.0, -0.39465, 2.03211,
                        1.13983, -0.5806, 0.0
                ) * yuv;
                vec4 nColor = vec4(rgb.r, rgb.g, rgb.b, 1);
                return nColor;
            }

            void main() {
                vec2 uv = vTextCoord.xy;
                float x;
                float y;
                if(vSplitScreen == 1) {//是否4分屏显示
                    if (uv.x >= 0.0 && uv.x <= 0.50) {
                        x = uv.x + uv.x;
                    } else {
                        x = uv.x + uv.x - 1.0;
                    }

                    if (uv.y >= 0.0 && uv.y <= 0.50) {
                        y = uv.y + uv.y;
                    } else {
                        y = uv.y + uv.y - 1.0;
                    }
                } else {
                    x = uv.x;
                    y = uv.y;
                }

                vec4 nColor = yuvToRGBAColor(yTexture, uTexture, vTexture, vec2(x, y));
                float average_color = nColor.r * 0.40 + nColor.g * 0.20 + nColor.b * 0.20;
                if(vChangeType == 1) {
                    gl_FragColor = vec4(nColor.r, 0, 0, 1);
                } else if(vChangeType == 2) {
                    gl_FragColor = vec4(0, nColor.g, nColor.b, 1);
                }else if(vChangeType == 3) {
                    gl_FragColor = vec4(average_color, average_color, average_color, 1);
                } else {
                    gl_FragColor = vec4(nColor);
                }
            }
    );

    float vertex_coords[12] = {//世界坐标
            -1, -1, 0, // left bottom
            1, -1, 0, // right bottom
            -1, 1, 0,  // left top
            1, 1, 0,   // right top
    };

    float fragment_coords[8] = {//纹理坐标
            0, 1,//left bottom
            1, 1,//right bottom
            0, 0,//left top
            1, 0,//right top
    };

    float fragment_coords_90[8] = {//逆时针90 纹理坐标
            1, 1,
            1, 0,
            0, 1,
            0, 0,
    };

    float fragment_coords_270[8] = {//逆时针270, 上下交换位置 纹理坐标
            0, 1,
            0, 0,
            1, 1,
            1, 0,
    };

    float matrix_scale[16];
    GLint gl_program;
    GLint gl_position;
    GLint gl_textCoord;
    GLint gl_uMatrix;
    GLint gl_change_type;
    GLint gl_split_screen_type;
    GLint gl_video_width;
    GLint gl_video_height;
    GLint gl_window_width;
    GLint gl_window_height;
    GLuint gl_texture_id[3];
    bool isNeedRotation;
    GlobalContexts *context;
    uint8_t *y_data;
    uint8_t *u_data;
    uint8_t *v_data;
    int change_type;
    int split_screen_type;

    void splitScreenRender();
};


#endif //OPENGLNDK_SHADERYUV_H
