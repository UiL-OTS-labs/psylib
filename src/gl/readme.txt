
LOADING of opengl modules.

First one should open a SDL_Window with all flags required for opengl.
Once the window is opened, you should make a SDL_GLContext. Once
you have the context, you can start loading the extensions and
functions that are NULL by default.

The loader is genarated with glad using the following command:
glad    --omit-khrplatform      \
        --local-files           \
        --profile core          \
        --generator c           \
        --extensions /dev/null  \
        --api gl=3.3,gles2=2.0  \
        --out-path .            \
        --spec gl

glad can be obtained with pip: pip(3) install glad.
