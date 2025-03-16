#include <EGL/egl.h>
#include <GLES3/gl3.h>
#include <iostream>

// Window size
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

int main() {
    // Initialize EGL display
    EGLDisplay display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    eglInitialize(display, NULL, NULL);

    // Choose an EGL config
    EGLConfig config;
    EGLint numConfigs;
    EGLint configAttribs[] = {
        EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT,
        EGL_NONE
    };
    eglChooseConfig(display, configAttribs, &config, 1, &numConfigs);

    // Create an EGL rendering context
    EGLContext context = eglCreateContext(display, config, EGL_NO_CONTEXT, (EGLint[]){
        EGL_CONTEXT_CLIENT_VERSION, 3, EGL_NONE
    });

    // Create a window surface (For Native Window Replace 0)
    EGLSurface surface = eglCreateWindowSurface(display, config, (EGLNativeWindowType)0, NULL);
    eglMakeCurrent(display, surface, surface, context);

    // Main loop
    while (true) {
        glClear(GL_COLOR_BUFFER_BIT);
        eglSwapBuffers(display, surface);
    }

    // Cleanup
    eglDestroySurface(display, surface);
    eglDestroyContext(display, context);
    eglTerminate(display);

    return 0;
}
