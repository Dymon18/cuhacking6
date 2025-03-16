#include <EGL/egl.h>
#include <GLES3/gl3.h>
#include <iostream>

// Window size
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;
void check_is_true(EGLBoolean rc, char* msg){
 if(rc != EGL_TRUE){
	fprintf(stderr, "failure %s\n", msg);
	exit(1);
  }
}
int main() {
    // Initialize EGL display
    EGLDisplay display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    EGLBoolean rc =     eglInitialize(display, NULL, NULL);
    check_is_true(rc, "initialize");

    // Choose an EGL config
    EGLConfig config;
    EGLint numConfigs;
    EGLint configAttribs[] = {
        EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT,
        EGL_NONE
    };
    rc = eglChooseConfig(display, configAttribs, &config, 1, &numConfigs);
	check_is_true(rc, "choose config");
    
EGLint context_data[] = {
        EGL_CONTEXT_CLIENT_VERSION, 3, EGL_NONE
    };    
    // Create an EGL rendering context
    EGLContext context = eglCreateContext(display, config, EGL_NO_CONTEXT, context_data);
	
    // Create a window surface (For Native Window Replace 0)
    EGLSurface surface = eglCreateWindowSurface(display, config, (EGLNativeWindowType)0, NULL);
    if(surface == NULL || context == NULL){
	fprintf(stderr, "surface/context was null %d", surface==NULL);
	exit(1);
}
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
